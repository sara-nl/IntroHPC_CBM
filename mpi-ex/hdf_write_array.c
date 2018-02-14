
#include <hdf5.h>
#include <stdlib.h>
#include "thdf.h"

void hdf_write_array       (HDF_FILE          * hdf_file,
                            const char        * path,
                            const int           rank,
                            const hsize_t     * dims,
                            const datatype   datatype,
                            const void        * buffer) {

  herr_t  ierr;
  hid_t   dataspace_id, dataset_id;

  hid_t   h5_datatype;


  switch (datatype) {

    case TYPE_INT :
      h5_datatype = H5T_NATIVE_INT;
      break;

    case TYPE_DOUBLE :
      h5_datatype = H5T_NATIVE_DOUBLE;
      break;

    default:
      h5_datatype = -1;
   }

 

  /* create the dataspace */
  dataspace_id = H5Screate_simple (rank, dims, NULL);

  /* create the dataset */
#if H5_USE_16_API 
  dataset_id = H5Dcreate (hdf_file -> cwd,
                          path,
                          h5_datatype,
                          dataspace_id,
                          H5P_DEFAULT);
#else
  dataset_id = H5Dcreate (hdf_file -> cwd,
                          path, 
                          h5_datatype,
                          dataspace_id,
                          H5P_DEFAULT,
                          H5P_DEFAULT,
                          H5P_DEFAULT);
#endif

  /* write the data to the h5 file */
  ierr = H5Dwrite (dataset_id,
                   h5_datatype,      /* mem_type */
                   dataspace_id,     /* mem space */
                   H5S_ALL,          /* file space, whole array */
                   H5P_DEFAULT,      /* transfer list, default settings */
                   buffer);          /* buffer */

  /* close the dataset */
  ierr = H5Dclose (dataset_id);

  /* close the dataspace */
  ierr = H5Sclose (dataspace_id);

  return;
}
  
