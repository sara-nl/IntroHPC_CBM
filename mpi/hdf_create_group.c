

#include <hdf5.h>
#include <stdlib.h>
#include "thdf.h"

void hdf_create_group (HDF_FILE   * hdf_file,
                       const char * path) {

  herr_t  ierr;

#if H5_USE_16_API 
  ierr = H5Gcreate ( hdf_file -> file_id,
                     path,
                     H5P_DEFAULT);
#else
  ierr = H5Gcreate ( hdf_file -> file_id,
                     path,
                     H5P_DEFAULT,
                     H5P_DEFAULT,
                     H5P_DEFAULT);
#endif
  return;
}

