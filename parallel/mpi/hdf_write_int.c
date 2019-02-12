#include "thdf.h"

void hdf_write_int   (HDF_FILE      * hdf_file,
                      const char    * path,
                      const int       idata) {

  hdf_write_scalar (hdf_file, path, TYPE_INT, & idata);

  return;
}

