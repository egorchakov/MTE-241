#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <stdlib.h>  
#include <stdio.h>  
#include "GLCD.h"
#include "config.h"

typedef struct circle {
    int radius;
    float x;
    float y;
    float dx;
    float dy;
    int color;
    unsigned short * pBitmap;
} circle_t;

typedef circle_t object_t;

void fill_circle(circle_t* circle, unsigned short* pBitmap, unsigned short color);
void draw_rectangle(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned short val);
unsigned short* gen_bitmap(size_t w, size_t h, void(*fill_fn)(unsigned short*, unsigned int, unsigned int, void*), void* args);
void draw_circle(circle_t* c, unsigned short* pBitmap);

#endif
