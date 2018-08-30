
#include <stdio.h>
#include <stdlib.h>
#include "grid.h"
#include "worker_grid.h"

#define DEBUG 0

void worker_grid_init(
  const int irank, /* rank of this domain */
  GRID        * g,
  WORKER_GRID * wg,
  double     (* f0)   (double, double),
  double     (* fdot0)(double, double)
) {
 
  int     np;            /* number of grid points */
  int     i, j;
  double  x, y;          /* computed location of this grid point */


  /* divide the work in i-direction */
  wg -> m = g -> icounts[irank];
  wg -> n = g -> n;

#if DEBUG > 4
  printf("wg %d: size m x n = %d x %d\n", irank, wg -> m, wg -> n);
#endif

  /* allocate space; in m- / i-direction, add halo rows */
  np = ( ( wg -> m ) + 2) * ( wg -> n );

  wg -> u  = (double*) malloc (np * sizeof (double) );
  wg -> v  = (double*) malloc (np * sizeof (double) );
  wg -> uk = (double*) malloc (np * sizeof (double) );
  wg -> vk = (double*) malloc (np * sizeof (double) );

  wg -> irank = irank;

  /* set the initial condition; do not set initial conditions for halos */
  for (i = 1; i <= ( wg -> m ); i++) {

#if DEBUG > 4
    printf("rank %d : i = %d\n", irank, i);
    printf("(dx, dy) = (%6.2f, %6.2f)\n", g -> dx, g -> dy);
#endif

    for (j = 0; j < ( wg -> n ); j++) {

      x = (double) (
        g -> istarts[irank]   +          /* start index of this worker grid */
        (i - 1)
      ) * g -> dx;

      y = (double) (j) * g -> dy;

#if DEBUG > 4
      printf("%d: %d, %d => %6.2f, %6.2f\n", irank, i, j, x, y);
#endif

      /* set the initial state */
      wg -> u[i * (wg -> n) + j] = f0(x, y);   /* set u     (t=0, x, y) */
      wg -> v[i * (wg -> n) + j] = fdot0(x,y); /* set du/dt (t=0, x, y) */

    }
  }

  /* initialize the halo region definitions */

  /* north send: send the first compute row, row 1 */
  wg -> i_north_send = (     1     ) * wg -> n;
  
  /* north receive: the halo row before the first compute row, row 0 */
  wg -> i_north_recv = (     1 - 1 ) * wg -> n;

  /* south send: send the last compute row, row m - 1 */
  wg -> i_south_send = (wg -> m - 1 ) * wg -> n;

  /* south receive: send the row after the last compute row, row m */
  wg -> i_south_recv = (wg -> m     ) * wg -> n;

  /* the number of elements in the message buffer */
  wg -> msg_count = wg -> n;

  return; 
}

static void array_halo_exchange(
  double      * arr,
  const int     i_south_send,
  const int     i_north_send,
  const int     i_south_recv,
  const int     i_north_recv,
  const int     count,
  int           i_rank,
  int           i_size,
  MPI_Comm      comm
) {
  
  /* send request handles; if null, any waits will be skipped */
  MPI_Request ireqs_s = MPI_REQUEST_NULL;
  MPI_Request ireqs_n = MPI_REQUEST_NULL; 
  MPI_Request ireqr_s = MPI_REQUEST_NULL;
  MPI_Request ireqr_n = MPI_REQUEST_NULL;

  /* for all ranks but the last rank, send the south compute row to the
   * next worker; send asynchronously, to avoid deadlocks */
  if (i_rank < i_size - 1) {
    MPI_Isend (
      & ( arr [ i_south_send ] ),
      count,
      MPI_DOUBLE,
      i_rank + 1,  /* send to the next rank */
      i_rank,      /* tag */
      comm,
      &ireqs_s
    );
  }

  /* for all ranks but the first rank, send the north compute row to the
   * previous worker; send asynchronously, to avoid deadlocks */
  if (i_rank > 0) {
    MPI_Isend (
      & ( arr [ i_north_send ] ),
      count,
      MPI_DOUBLE,
      i_rank - 1,  /* send to the previous rank */
      i_rank,      /* tag */
      comm,
      &ireqs_n
    );
  }

  /* for all ranks but the last rank, receive the south halo row from
   * the next worker */
  if (i_rank < i_size - 1) {
    MPI_Irecv (
      & ( arr [ i_south_recv ] ),
      count,
      MPI_DOUBLE,
      i_rank + 1,  /* receive from the next rank */
      MPI_ANY_TAG,
      comm,
      &ireqr_s
    );
  }

  /* for all ranks but the first rank, receive the north halo row from
   * the previous worker */
  if (i_rank > 0) {
    MPI_Irecv (
      & ( arr [ i_north_recv ] ),
      count,
      MPI_DOUBLE,
      i_rank - 1,  /* receive from the previous rank */
      MPI_ANY_TAG,
      comm,
      &ireqr_n
    );
  }

  MPI_Wait(&ireqs_s, MPI_STATUS_IGNORE);
  MPI_Wait(&ireqs_n, MPI_STATUS_IGNORE);
  MPI_Wait(&ireqr_s, MPI_STATUS_IGNORE);
  MPI_Wait(&ireqr_n, MPI_STATUS_IGNORE);

  return;
}

void worker_grid_halo_exchange (
  WORKER_GRID * wg,
  int           i_rank,
  int           i_size,
  MPI_Comm      comm
) {
  /* send u */
  array_halo_exchange(wg -> u, wg -> i_south_send, wg -> i_north_send,
                      wg -> i_south_recv, wg -> i_north_recv,
                      wg -> msg_count, i_rank, i_size, comm);

  /* send du/du */
  array_halo_exchange(wg -> v, wg -> i_south_send, wg -> i_north_send,
                      wg -> i_south_recv, wg -> i_north_recv,
                      wg -> msg_count, i_rank, i_size, comm);

  return;
}


