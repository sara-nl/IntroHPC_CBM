#ifndef _GRID_STRUCT_H
#define _GRID_STRUCT_H 

/* definition of the global grid, with the division of work */
typedef struct {
  int      m;
  int      n;
  double   dx;
  double   dy;
  int      isize;

  /* for each worker indicates at which m the domain starts, and
     how many elements in m-direction this domain has
   */
  int    * istarts;
  int    * icounts;

} GRID;

#endif
