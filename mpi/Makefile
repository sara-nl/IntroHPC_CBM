
CFLAGS=-Wall -O2

LIBS_HDF5=-lhdf5

CC=mpicc

wave: wave.o grid.o worker_grid.o hdfout.o libthdf.a
	mpicc -g -o wave wave.o grid.o worker_grid.o hdfout.o \
                   libthdf.a $(LIBS_HDF5) -lm 

OBJS_THDF= \
hdf_close_file.o	          hdf_write_array.o	        hdf_write_string.o       \
hdf_create_group.o	        hdf_write_array_double.o  hdf_write_tarray.o       \
hdf_create_new_file.o	      hdf_write_array_int.o     hdf_write_tarray_double.o\
hdf_create_tarray.o	        hdf_write_double.o	      hdf_write_tarray_int.o   \
hdf_create_tarray_double.o  hdf_write_int.o           \
hdf_create_tarray_int.o     hdf_write_scalar.o


libthdf.a: libthdf.a($(OBJS_THDF))

clean:
	rm -f ./*.o ./wave libthdf.a

