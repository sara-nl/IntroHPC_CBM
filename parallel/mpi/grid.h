#ifndef _GRID_H_
#define _GRID_H_

#include "grid_struct.h"

void grid_init(
  const int m,
  const int n,
  const double dx,
  const double dy,
  const int isize,
  GRID * g
);

void grid_finalize(
  GRID * g
);

/* Work division */
void grid_divide_work(
  const int   isize,           /* number of workers                        */
  GRID * g
);

#endif
