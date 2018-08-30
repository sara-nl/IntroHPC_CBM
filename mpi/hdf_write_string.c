
#include <hdf5.h>
#include <hdf5_hl.h>
#include <stdlib.h>
#include "thdf.h"

void hdf_write_string(HDF_FILE      * hdf_file,
                      const char    * path,
                      const char    * str) {

  H5LTmake_dataset_string (hdf_file -> cwd, path, str);

  return;
}
  
