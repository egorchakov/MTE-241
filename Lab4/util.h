#ifndef _UTIL_H
#define _UTIL_H
#include <stdio.h>
#include "GLCD.h"
#include "circle.h"

void draw_circle(circle_t c);
void draw_rectangle(unsigned int, unsigned int, unsigned int , unsigned int , void*);
unsigned short* gen_bitmap(size_t w, size_t h, void(*fill_fn)(unsigned short*, unsigned int, unsigned int, void*), void* args);

#endif
