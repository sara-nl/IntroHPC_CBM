
#include <stdlib.h>
#include <hdf5.h>
#include <mpi.h>

#include "grid.h"
#include "worker_grid_struct.h"
#include "hdfout.h"
#include "thdf.h"

#define DEBUG 0

void hdfout_init(
  const char        * h5_filename,    /* the name of the HDF5 output file */
  const GRID        * g,              /* the grid metadata                */
  const int           irank,
  const int           nt_o,           /* number of output timesteps       */
  HDFOUT            * hdfout
  
) {
  const int           ndims = 2;

  int                 sizes    [ ndims ];
  int                 subsizes [ ndims ];
  int                 starts   [ ndims ];

  hsize_t             dims     [ ndims ];

  int                 i;

  
  hdfout -> irank = irank;
  hdfout -> isize = g -> isize;

  if (irank == 0) {
    /* master only */
    
    /* allocate space for the recv subarray datatypes */
    hdfout -> recv_subarrays = 
      (MPI_Datatype *) malloc (hdfout -> isize * sizeof (MPI_Datatype) );

    /* define the subarrays for receiving data from nodes */
    for (i = 0; i < g -> isize; i++) {
      /* global grid size */
      sizes[0]  = g -> m;    
      sizes[1]  = g -> n;

      /* region to be received */
      subsizes[0] = g -> icounts[i];
      subsizes[1] = g -> n;

      /* where to receive the region */
      starts[0]   = g -> istarts[i];
      starts[1]   = 0;

#if DEBUG > 4
      printf("recv: rank %d: size   %3d x %3d; subsize %3d x %3d;\n"
             "               starts %3d x %3d\n",
             i, sizes[0], sizes[1], subsizes[0], subsizes[1],
             starts[0], starts[1]);
#endif

      MPI_Type_create_subarray(ndims, sizes, subsizes, starts,
        MPI_ORDER_C, MPI_DOUBLE, &( hdfout -> recv_subarrays[i]) );

      MPI_Type_commit ( & (hdfout -> recv_subarrays[i]) );
    }
  }
  else {
    /* workers */
    hdfout -> recv_subarrays = NULL;
  }

  /* set up the MPI subarrays for sending results to the master */
  /* define the region to be sent to the master */
  sizes[0]    = g -> icounts [irank] + 2; /* this array is (m + 2) x n */
  sizes[1]    = g -> n;

  subsizes[0] = g -> icounts [irank]; /* send the array w/o the halo regions */
  subsizes[1] = g -> n;

  starts[0]   = 1;       /* start from m=1, so w/o halo region */
  starts[1]   = 0;

#if DEBUG > 4
      printf("send: rank %d: size   %3d x %3d; subsize %3d x %3d;\n"
             "               starts %3d x %3d\n",
             hdfout -> irank, sizes[0], sizes[1], subsizes[0], subsizes[1],
             starts[0], starts[1]);
#endif

  /* create the subarray datatype */
  MPI_Type_create_subarray( ndims, sizes, subsizes, starts,	
    MPI_ORDER_C, MPI_DOUBLE, &( hdfout -> send_subarray ) );

  MPI_Type_commit (& (hdfout -> send_subarray) );

  /* wait for all workers to reach this point */
  MPI_Barrier(MPI_COMM_WORLD);



  if (irank == 0) {
    /* create the HDF output file and metadata */
    hdfout -> hdf_file = hdf_create_new_file(h5_filename);

    dims[0] = g -> m;
    dims[1] = g -> n;
    
    /* in the data arrays, include t=0 */
    hdf_create_tarray_double(hdfout -> hdf_file, "/u", ndims, dims, nt_o + 1);
    hdf_create_tarray_double(hdfout -> hdf_file, "/v", ndims, dims, nt_o + 1);
  }

  return;
}

void hdfout_finalize(
  HDFOUT        * hdfout
) {
  int i;

  /* free the send subarray type */
  MPI_Type_free( & ( hdfout -> send_subarray ) );

  /* the master also frees the recv subarray types, and the array itself */
  if (hdfout -> irank == 0) {
    for (i=0; i < hdfout -> isize; i++) {
      MPI_Type_free( & (hdfout -> recv_subarrays[i]) );
    }
    free(hdfout -> recv_subarrays);
  }

  if (hdfout -> irank == 0) {
    /* close and finalize the hdf5 output file */
    hdf_close_file(hdfout -> hdf_file);
  }

  return;
} 

        

  
void hdfout_write(
  HDFOUT        * hdfout,
  GRID          * g,
  WORKER_GRID   * wg,
  int             t
) {

  MPI_Request  * requests;
  double       * buf;
  int            i;

  MPI_Barrier(MPI_COMM_WORLD);

  if (hdfout -> irank == 0) {
   
    /* master collects all results */
    buf      = (double*) malloc ((g->m) * (g->n) * sizeof (double) );
    requests = (MPI_Request*) malloc ((g->isize) * sizeof (MPI_Request));

    
    /* initiate receiving data */
    for (i = 0; i < g -> isize; i++) {
#if DEBUG > 4
      printf("Rank 0 receiving from rank %d\n", i);
#endif
      MPI_Irecv(buf,
                1,
                hdfout -> recv_subarrays[i],
                i,
                MPI_ANY_TAG,
                MPI_COMM_WORLD,
                &( requests[i] )
      );
    }
  }
  else {
    buf      = NULL;
    requests = NULL;
  }

  /* send the data from the worker using the subarray definition */
#if DEBUG > 4
      printf("Rank %d sending to rank 0\n", wg -> irank);
#endif
  MPI_Send(wg -> u,
           1,                        /* 1 subarray */
           hdfout -> send_subarray,
           0,                        /* to rank 0 */
           wg -> irank,              /* as tag */
           MPI_COMM_WORLD);

  /* blocks until send is finished */

  if (hdfout -> irank == 0) {

#if DEBUG > 4
      printf("Waiting for all transfers to finish...\n");
#endif
    /* wait until all receive requests are finished */
    MPI_Waitall(g->isize, requests, MPI_STATUSES_IGNORE);

#if DEBUG > 4
    printf("Transfers finished.\n");
#endif

    /* write the buffer to a file */
    hdf_write_tarray_double(hdfout -> hdf_file, "/u", t, buf);
 

    free(buf);
    free(requests);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  return;
}
 
