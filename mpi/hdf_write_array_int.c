
#include <hdf5.h>
#include <stdlib.h>
#include "thdf.h"

void hdf_write_array_int   (HDF_FILE      * hdf_file,
                            const char    * path,
                            const int       rank,
                            const hsize_t * dims,
                            const int     * idata) {

  hdf_write_array (hdf_file, 
                   path,
                   rank,
                   dims,
                   TYPE_INT,
                   idata);

  return;
}
  
