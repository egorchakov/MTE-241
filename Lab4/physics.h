#ifndef _PHYSICS_H
#define _PHYSICS_H

#include "math.h"
#include "animation.h"
#define M_PI 3.1415926

BOOL check_intersect(int x1, int y1, int r1, int x2, int y2, int r2);
void calculate_velocities(object_t* obj1, object_t* obj2);

#endif