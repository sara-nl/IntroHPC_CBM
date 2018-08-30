#ifndef _THDF_H
#define _THDF_H

#include <hdf5.h>

typedef enum {
  TYPE_INT,
  TYPE_DOUBLE
} datatype;


typedef struct {
  hid_t  file_id;
  hid_t  cwd;       /* current 'directory' */
} HDF_FILE;


/* file functions */
HDF_FILE * hdf_create_new_file (const char * filename);
 
void       hdf_close_file      (HDF_FILE * h5file);

/* group functions */
void hdf_create_group (HDF_FILE   * hdf_file,
                       const char * path);

/* scalar functions */
void hdf_write_scalar(HDF_FILE         * hdf_file,
                      const char       * path,
                      const datatype  type,
                      const void       * data);

void hdf_write_int   (HDF_FILE      * hdf_file,
                      const char    * path,
                      const int       idata);

void hdf_write_double(HDF_FILE      * hdf_file,
                      const char    * path,
                      const double    ddata);

void hdf_write_string(HDF_FILE      * hdf_file,
                      const char    * path,
                      const char    * str);

/* generic array routines */
void hdf_write_array       (HDF_FILE          * hdf_file,
                            const char        * path,
                            const int           rank,
                            const hsize_t     * dims,
                            const datatype   datatype,
                            const void        * buffer);

void hdf_create_tarray     (HDF_FILE          * hdf_file,
                            const char        * path,
                            const int           rank,
                            const hsize_t     * dims,
                            const int           n_timesteps,
                            const datatype   datatype);

void hdf_write_tarray      (HDF_FILE          * hdf_file,
                            const char        * path,
                            int                 i_time, 
                            const datatype   datatype,
                            const void        * buffer);
 
/* type-specific routines (wrappers for generic routines) */
void hdf_write_array_int   (HDF_FILE      * hdf_file,
                            const char    * path,
                            const int       rank,
                            const hsize_t * dims,
                            const int     * idata); 

void hdf_write_array_double (HDF_FILE      * hdf_file,
                             const char    * path,
                             const int       rank,
                             const hsize_t * dims,
                             const double  * ddata); 

void hdf_create_tarray_int (HDF_FILE          * hdf_file,
                            const char        * path,
                            const int           rank,
                            const hsize_t     * dims,
                            const int           n_timesteps);
 
void hdf_create_tarray_double (HDF_FILE          * hdf_file,
                               const char        * path,
                               const int           rank,
                               const hsize_t     * dims,
                               const int           n_timesteps);

void hdf_write_tarray_int  (HDF_FILE          * hdf_file,
                            const char        * path,
                            int                 i_time, 
                            const void        * buffer);

void hdf_write_tarray_double (HDF_FILE          * hdf_file,
                              const char        * path,
                              int                 i_time, 
                              const void        * buffer); 
#endif

