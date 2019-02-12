
#include <hdf5.h>
#include <stdlib.h>
#include "thdf.h"

void hdf_write_tarray_double (HDF_FILE          * hdf_file,
                              const char        * path,
                              int                 i_time, 
                              const void        * buffer) {


  hdf_write_tarray (hdf_file, path, i_time, TYPE_DOUBLE, buffer);

  return;
}
  
