
#include <hdf5.h>
#include <stdlib.h>
#include "thdf.h"

void hdf_write_tarray      (HDF_FILE          * hdf_file,
                            const char        * path,
                            int                 i_time, 
                            const datatype   datatype,
                            const void        * buffer) {

  herr_t    ierr;
  hid_t     f_dataspace_id;  /* file dataspace id   */
  hid_t     m_dataspace_id;  /* memory dataspace id */
  hid_t     dataset_id;

  int       t_rank;       /* original rank increased with one for time dim */
  hsize_t * t_dims;       /* dimensions augmented with the time dimension  */
  hid_t     h5_datatype;

  int       rank;         /* rank of the memory dataset */ 
  hsize_t * dims;         /* dimensions for the memory dataset */

  hsize_t * start;        /* start indices per dimension of the hyperslab */
  hsize_t * count;        /* count per dimension of the hyperslab */

  int       i;

  /* open the dataset */
#if H5_USE_16_API 
  dataset_id = H5Dopen (hdf_file -> cwd,
                        path);
#else
  dataset_id = H5Dopen (hdf_file -> cwd,
                        path,
                        H5P_DEFAULT);
#endif

  /* get the dataspace associated with this dataset */
  f_dataspace_id = H5Dget_space (dataset_id);

  /* get the rank of the dataspace */
  t_rank = H5Sget_simple_extent_ndims (f_dataspace_id);

  /* allocate space to store the dimensions of this dataspace, then get them */
  t_dims = (hsize_t*) malloc (t_rank * sizeof (hsize_t));
  H5Sget_simple_extent_dims (f_dataspace_id, t_dims, NULL);

  /* allocate memory for the memory dataspace */
  rank = t_rank - 1;
  dims = (hsize_t *) malloc (rank * sizeof (hsize_t) );

  /* copy the dimensions without the time dimension */
  for (i=0; i < rank; i++) {
    dims [i] = t_dims [i];
  }

  /* create a memory dataspace which indicates how the array is stored in
   * memory */
  m_dataspace_id = H5Screate_simple (rank, dims, NULL);


 
  /* create and select a hyperslab */
  start = (hsize_t *) malloc (t_rank * sizeof (hsize_t) );
  count = (hsize_t *) malloc (t_rank * sizeof (hsize_t) );

  /* set all start dimensions to 0, except the time / last dimension;
   * set the count to the size of the dimension */
  for (i=0; i < rank; i++) {
    start[i] = 0;
    count[i] = dims[i];
  }

  /* select the time */
  start[t_rank - 1] = i_time;
  count[t_rank - 1] = 1;
  
  H5Sselect_hyperslab (f_dataspace_id,
                       H5S_SELECT_SET,   /* define a new hyperslab */
                       start,
                       NULL,             /* stride, default = 1 */
                       count,
                       NULL);            /* block size, default = 1 */
  
  /* create a memory data type */
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

 

  /* write the data to the h5 file */
  ierr = H5Dwrite (dataset_id,
                   h5_datatype,      /* mem_type */
                   m_dataspace_id,   /* mem space */
                   f_dataspace_id,   /* file space (timeslice) */
                   H5P_DEFAULT,      /* transfer list, default settings */
                   buffer);          /* buffer */

  /* close the memory data type */
  H5Tclose (h5_datatype);

  /* close the dataset */
  ierr = H5Dclose (dataset_id);

  /* close the dataspaces */
  ierr = H5Sclose (f_dataspace_id);
  ierr = H5Sclose (m_dataspace_id);

  /* free the memory for the time-augmented dimension array */
  free (dims);
  free (t_dims);
  free (start);
  free (count);

  return;
}
  
