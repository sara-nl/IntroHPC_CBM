
#ifndef _WORKER_GRID_H_
#define _WORKER_GRID_H_

#include <mpi.h>
#include "grid_struct.h"
#include "worker_grid_struct.h"

void worker_grid_init(
  const int irank, /* rank of this domain */
  GRID        * g,
  WORKER_GRID * wg,
  double     (* f0)   (double, double),
  double     (* fdot0)(double, double)
);
 
void worker_grid_halo_exchange(
  WORKER_GRID * wg,
  int           i_rank,
  int           i_size,
  MPI_Comm      comm
); 




#endif

