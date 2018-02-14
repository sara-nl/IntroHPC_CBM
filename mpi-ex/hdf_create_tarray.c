
#include <hdf5.h>
#include <stdlib.h>
#include "thdf.h"

void hdf_create_tarray     (HDF_FILE          * hdf_file,
                            const char        * path,
                            const int           rank,
                            const hsize_t     * dims,
                            const int           n_timesteps,
                            const datatype   datatype) {

  herr_t     ierr;
  hid_t      dataspace_id, dataset_id;

  hsize_t  * t_dims;
  int        t_rank;

  hid_t      h5_datatype;

  int        i;

  /* create a new set of dimensions from the original dimensions,
   * augmented with the time dimension */
  t_rank = rank + 1;

  t_dims = (hsize_t *) malloc (t_rank * sizeof (hsize_t) );

  for (i=0; i < rank; i++) {
    t_dims[i] = dims[i];
  }

  t_dims[t_rank - 1] = n_timesteps;

  dataspace_id = H5Screate_simple (t_rank, t_dims, NULL);

  switch (datatype) {

    case TYPE_INT :
      h5_datatype = H5Tcopy ( H5T_NATIVE_INT );
      break;

    case TYPE_DOUBLE :
      h5_datatype = H5Tcopy ( H5T_NATIVE_DOUBLE );
      break;

    default:
      h5_datatype = -1;
   }

 
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

  /* close the datatype */
  ierr = H5Tclose (h5_datatype);

  /* close the dataset */
  ierr = H5Dclose (dataset_id);

  /* close the dataspace */
  ierr = H5Sclose (dataspace_id);

  /* free memory used for the dimensions */
  free (t_dims);

  return;
}
  
