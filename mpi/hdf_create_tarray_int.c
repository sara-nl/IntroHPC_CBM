#include "thdf.h"

void hdf_create_tarray_int (HDF_FILE          * hdf_file,
                            const char        * path,
                            const int           rank,
                            const hsize_t     * dims,
                            const int           n_timesteps) {

  hdf_create_tarray     (hdf_file, path, rank, dims, n_timesteps, TYPE_INT);

  return;
}

  
