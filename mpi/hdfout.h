#ifndef _HDFOUT_H_
#define _HDFOUT_H_

#include "thdf.h"

typedef struct {

  /* MPI comminication related */
  MPI_Datatype   * recv_subarrays;    /* array of length irank, for recieving */
  MPI_Datatype     send_subarray;     /* for sending data */

  int              irank;             /* this is me */
  int              isize;

  /* HDF output related */
  HDF_FILE       * hdf_file;  
  
} HDFOUT;


/* Initializes the hdf5 output subsystem.
 *
 * For the master with irank == 0, creates and opens an HDF5 file for
 * writing output, and sets up the MPI subarrays for recieving results
 * from workers.
 * 
 * All ranks set up the subarray for sending results to the master,
 * including the master itself.
 */
 
void hdfout_init(
  const char        * h5_filename,    /* the name of the HDF5 output file */
  const GRID        * g,              /* the grid metadata                */
  const int           irank,
  const int           nt,             /* number of timesteps              */
  HDFOUT            * hdfout
);

void hdfout_finalize(
  HDFOUT        * hdfout
);
 
void hdfout_write(
  HDFOUT        * hdfout,
  GRID          * g,
  WORKER_GRID   * wg,
  int             t
);
     
#endif

