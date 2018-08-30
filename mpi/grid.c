
#include <stdlib.h>
#include "grid.h"

void grid_init(
  const int m,
  const int n,
  const double dx,
  const double dy,
  const int isize,
  GRID * g
) {

  int i, inext;

  g -> m = m;
  g -> n = n;

  g -> dx = dx;
  g -> dy = dy;

  g -> isize = isize;
  g -> istarts = (int*) malloc (isize * sizeof (int) );
  g -> icounts = (int*) malloc (isize * sizeof (int) );

  /* compute the division of work */
  for (i=0; i < isize; i++) {
    g -> istarts[i] = (g -> m * (i    )) / isize;
    inext           = (g -> m * (i + 1)) / isize;
    g -> icounts[i] = inext - (g -> istarts[i]);
  }
 
  return;
}

void grid_finalize(
  GRID * g
) {

  free ( g -> istarts );
  free ( g -> icounts );

  return;
}

