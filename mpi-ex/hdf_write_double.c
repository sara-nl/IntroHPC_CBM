#include "thdf.h"

void hdf_write_double(HDF_FILE      * hdf_file,
                      const char    * path,
                      const double    ddata) {

  hdf_write_scalar (hdf_file, path, TYPE_DOUBLE, & ddata);

  return;
}

