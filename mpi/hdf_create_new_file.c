
#include <stdio.h>
#include <stdlib.h>
#include <hdf5.h>

#include "thdf.h"


HDF_FILE * hdf_create_new_file (const char * filename)
{

  HDF_FILE * hdf_file;

  hdf_file = (HDF_FILE*) malloc (sizeof (HDF_FILE));

  hdf_file -> file_id = H5Fcreate (filename,
                                   H5F_ACC_TRUNC,  /* overwrite existing */
                                   H5P_DEFAULT,
                                   H5P_DEFAULT);

  /* set the current path to the file itself, or, the root */
  hdf_file -> cwd = hdf_file -> file_id;

  return hdf_file;
}


                                   

  
  

