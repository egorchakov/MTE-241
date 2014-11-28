#include <stdbool.h>
#include "physics.h"

BOOL check_intersect(int x1, int y1, int r1, int x2, int y2, int r2) {
    int dist = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
    return dist >= (r1 - r2) * (r1 - r2) && dist <= (r1 + r2) * (r1 + r2);
}

void calculate_velocities(object_t* obj1, object_t* obj2) {
    double phi = atan2(obj1->y - obj2->y, obj1->x - obj2->x);
    double theta1 = atan2(obj1->dy, obj1->dx);
    double theta2 = atan2(obj2->dy, obj2->dx);  
    double v1 = sqrt(obj1->dy * obj1->dy + obj1->dx * obj1->dx);
    double v2 = sqrt(obj2->dy * obj2->dy + obj2->dx * obj2->dx);
    obj1->dx = v2 * cos(theta2 - phi) * cos(phi) + v1 * sin(theta1 - phi) * cos(phi + M_PI/2);
    obj1->dy = v2 * cos(theta2 - phi) * sin(phi) + v1 * sin(theta1 - phi) * sin(phi + M_PI/2);
    
    obj2->dx = v1 * cos(theta1 - phi) * cos(phi) + v2 * sin(theta2 - phi) * cos(phi + M_PI/2);
    obj2->dy = v1 * cos(theta1 - phi) * sin(phi) + v2 * sin(theta2 - phi) * sin(phi + M_PI/2);
}