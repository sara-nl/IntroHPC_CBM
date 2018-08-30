#ifndef _WORKER_GRID_STRUCT_H
#define _WORKER_GRID_STRUCT_H

/* definition of the grid for one worker; the state itself;
 * subarray definitions for halo regions
 */
typedef struct {
  int m, n;
  double dx, dy;
  double *u;     /* u      (t=i)   */
  double *v;     /* du/dt  (t=i)   */
  double *uk;    /* u      (t=i+1) */
  double *vk;    /* du/dt  (t=i+1) */
  int i_north_send;
  int i_north_recv;
  int i_south_send;
  int i_south_recv;
  int msg_count;
  int irank;
} WORKER_GRID;

#endif
