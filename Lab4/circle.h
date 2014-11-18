#ifndef _CIRCLE_H
#define _CIRCLE_H
#include <stdio.h>

typedef struct circle {
	unsigned int radius;
	unsigned int x;
	unsigned int y;
} circle_t;

void fill_circle(unsigned short* pBitmap, unsigned int w, unsigned int h, void* circle);
#endif


