
#include <hdf5.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thdf.h"

#include "grid.h"
#include "worker_grid.h"
#include "hdfout.h"

#ifdef _OPENMP
#  include <omp.h>
#endif

#define MAX_PATH_LENGTH 2048

/************************************************************************
 

         j ->

           0       1       2                n

       +-------+-------+-------+-------+-- -- --
       |       |       |       |       |
  i 0  |   0   |   m   |   2m  |  ...  |
       |       |       |       |       |
  |    +-------+-------+-------+-------+-- -- --
  V    |       |       |       |       |
    1  |   1   |  m+1  | 2m+1  |  ...  |
       |       |       |       |       |
       +-------+-------+-------+-------+-- -- --
       |       |       |       |       |
    2  |   2   |  m+2  | 2m+2  |  ...  |
       |       |       |       |       |




*************************************************************************/

/* function for initial condition u(x,y,t=0) */
double f0(double x, double y) {
  
  extern double f0x0;
  extern double f0y0;
  extern double f0r0;

  /* make a bump with radius r0, around (x0, y0) */
  const double pi = 4.0 * atan(1.0);

  double r = sqrt( (x - f0x0) * (x - f0x0) + (y - f0y0) * (y - f0y0) );

  if (r > f0r0)
    return 0;
  else 
    return cos((r / f0r0) * (pi / 2.0));
}

/* function for initial condition du/dt(x,y,t=0) */
double fdot0(double x, double y) {
  return 0.0;
}


/* integration step forward in time */
void timestep(
  const int      m,
  const int      n,
  double         u [m + 2][n],
  double         v [m + 2][n],
  double         uk[m + 2][n],
  double         vk[m + 2][n],
  const double   c,
  const double   zeta,
  const double   dx,
  const double   dy,
  const double   dt,
  const int      irank,
  const int      isize
) {

  int i, j;
  double uxx, uyy;

  /* loop in i-direction, this is from 1 to m; 0 and m+1 are halos */
  for (i = 1; i <= m ; i++) {
  

    /* if OpenMP-parallellisation is enabled, perform this loop in parallel */
#   ifdef _OPENMP
#     pragma omp parallel for private (uxx, uyy) shared (u, v, uk, vk)
#   endif

    /* loop in j-direction; this is from 0 to n-1 */
    for (j = 0; j < n ; j++) {

      /* determine d^ u / dx^2 */

      if (j == 0) {
        
        /* n boundary: dirichlet boundary condition u = 0 */
        uxx = -3 * u[i][0] + u[i][1];
      }
      else if (j == n - 1) {
      
       /* s boundary: neumann boundary condition u' = 0 */
        uxx = u[i][n-2] - u[i][n-1];
      }
      else {

        /* interior point in j-direction */
        uxx = u[i][j-1] - 2 * u[i][j] + u[i][j+1];
      }

      uxx /= (dx * dx);


      /* determine d^2 u / dy^2 */
        
      if (irank == 0 && i == 1) {
        /* w boundary: dirichlet boundary condition */
        uyy = -3 * u[1][j] + u[2][j];

      }
      else if (irank == isize - 1 && i == m) {
        /* e boundary: dirichlet boundary condition */
        uyy = u[m-1][j] - 3 * u[m][j];
      }
      else {
        /* interior point in j-direction */
        uyy = u[i-1][j] - 2 * u[i][j] + u[i+1][j];
      }
     
      uyy /= (dy * dy);

      /* integrate forward in time */

      uk[i][j] = u[i][j] + dt * (1.0 - zeta) * v[i][j]; /* damping */
      vk[i][j] = v[i][j] + dt * c * c * (uxx + uyy); 
    }
  }

  /* copy the new state into the original state */
  memcpy(u, uk, (m + 2) * n * sizeof(double));
  memcpy(v, vk, (m + 2) * n * sizeof(double));

  return;
} 

/* make these variables global, to keep code simple */
double f0x0, f0y0, f0r0;


int main(int argc, char** argv) {

  /* problem definition */
  double c;
  double zeta;

  /* global, spatial grid definition */
  int m;
  int n;

  double dx;
  double dy;

  /* time domain definition */
  int    nt;      /* number of steps in time */
  double dt;
  int    nt_i;    /* output interval */
  int    nt_o;    /* number of output timesteps */

  /* NOTE: make sure dat dt / dx < c, for numerical stability !!! */


  /* process-related variables */

  int          i_t;  /* time step */

  GRID g;            /* global grid metadata */
  WORKER_GRID wg;    /* (worker-)local grid, including u and du/dt */

  /* output-related variables */
  HDFOUT hdfout;
  char hdfout_filename[MAX_PATH_LENGTH];

  /* mpi variables */
  int irank, isize;                /* rank and size of communicator */
  int ierr;    


  /* = start of the program = */

  /* == initialize MPI == */

  ierr = MPI_Init(&argc, &argv);

  if (ierr != 0) {
    fprintf(stderr, "[ERROR] : MPI_Init failed.\n");
    exit(-1);
  }

  /* determine rank and size */
  MPI_Comm_rank(MPI_COMM_WORLD, &irank);
  MPI_Comm_size(MPI_COMM_WORLD, &isize); 

  /* parse command-line arguments */
  if (argc < 13) {
    if (irank == 0) {
      fprintf(stderr, "*** error: missing arguments; see description.\n");
    }
    MPI_Finalize();
    exit(-1);
  }

  /* global, spatial grid definition */
  m             = atoi(argv[1]);
  n             = atoi(argv[2]);

  dx            = atof(argv[3]);
  dy            = atof(argv[4]);

  /* time domain definition */
  nt            = atoi(argv[5]);       /* number of steps in time */
  dt            = atof(argv[6]);

  /* wave propagation speed, damping */
  c             = atof(argv[7]);
  zeta          = atof(argv[8]);

  /* initial condition */
  f0x0          = atof(argv[9]);
  f0y0          = atof(argv[10]);
  f0r0          = atof(argv[11]);

  /* output specification */
  nt_i          = atoi(argv[12]);          /* output interval */
  strncpy(hdfout_filename, argv[13], MAX_PATH_LENGTH);

  nt_o   = nt / nt_i; /* compute the number of output steps */



  /* == start the calculation == */

  /* initialize the global grid metadata */
  grid_init(m, n, dx, dy, isize, &g);

  /* initialize the worker grid */
  worker_grid_init(irank, &g, &wg, f0, fdot0);
  
  /* initialize the output file and create a dataset */
  hdfout_init(hdfout_filename, &g, irank, nt_o, &hdfout);

  i_t = 0;

  /* write the first timestep to the output file */
  hdfout_write(&hdfout, &g, &wg, i_t);

  /* integrate in time */

  for (i_t=1; i_t <= nt; i_t++) {

    /* communicate the halo regions */
    worker_grid_halo_exchange(&wg, irank, isize, MPI_COMM_WORLD);

    timestep(
      wg.m, wg.n, 
      ( double(*) [ ( wg.m + 2 ) * wg.n]) wg.u,
      ( double(*) [ ( wg.m + 2 ) * wg.n]) wg.v,
      ( double(*) [ ( wg.m + 2 ) * wg.n]) wg.uk,
      ( double(*) [ ( wg.m + 2 ) * wg.n]) wg.vk,
      c, zeta, dx, dy, dt, irank, isize
    );

    if (i_t % nt_i == 0) {

      if (irank == 0) {
        printf("Writing timestep %4d/%4d ...\n", i_t / nt_i, nt_o);
      }
      hdfout_write(&hdfout, &g, &wg, i_t / nt_i);
    }
 
  } /* next timestep */

  hdfout_finalize(&hdfout);
 
  MPI_Finalize();

  return 0;
}

