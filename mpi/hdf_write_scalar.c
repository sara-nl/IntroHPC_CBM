
#include <hdf5.h>
#include <stdio.h>
#include <stdlib.h>
#include "thdf.h"

void hdf_write_scalar(HDF_FILE         * hdf_file,
                      const char       * path,
                      const datatype  type,
                      const void       * data) {

  herr_t  ierr;
  hid_t   dataspace_id, dataset_id, datatype_id;
  hid_t   datatype;

  /* create the datatype */
  switch (type) {
    case TYPE_INT     :
      datatype    = H5T_NATIVE_INT;
      datatype_id = H5Tcopy (H5T_NATIVE_INT);
      break;

    case TYPE_DOUBLE  :
      datatype    = H5T_NATIVE_DOUBLE;
      datatype_id = H5Tcopy (H5T_NATIVE_DOUBLE);
      break;

    default:
      datatype_id = -1;
  }

  /* create the dataspace */
  dataspace_id = H5Screate (H5S_SCALAR);

  /* create the dataset */
#if H5_USE_16_API
  dataset_id = H5Dcreate (hdf_file -> cwd,
                          path, 
                          datatype_id,
                          dataspace_id,
                          H5P_DEFAULT);
#else
  dataset_id = H5Dcreate (hdf_file -> cwd,
                          path, 
                          datatype_id,
                          dataspace_id,
                          H5P_DEFAULT,
                          H5P_DEFAULT,
                          H5P_DEFAULT);
#endif


  /* write the data to the h5 file */
  ierr = H5Dwrite (dataset_id,
                   datatype,         /* mem_type */
                   dataspace_id,     /* mem space */
                   H5S_ALL,          /* file space, whole array */
                   H5P_DEFAULT,      /* transfer list, default settings */
                   data);            /* buffer */
  
  /* close the datatype */
  ierr = H5Tclose (datatype_id);

  /* close the dataset */
  ierr = H5Dclose (dataset_id);

  /* close the dataspace */
  ierr = H5Sclose (dataspace_id);

  return;
}
  
