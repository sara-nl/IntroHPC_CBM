
#include <stdio.h>
#include <stdlib.h>
#include <hdf5.h>

#include "thdf.h"

void       hdf_close_file      (HDF_FILE * h5file) {
  
  herr_t   h5err;

  h5err = H5Fclose ( h5file -> file_id );

  free (h5file);

  return;
}


