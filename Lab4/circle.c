#include "circle.h"

void fill_circle(unsigned short* pBitmap, unsigned int w, unsigned int h, void* args) {
	circle_t* circle = (circle_t*)args;
	unsigned short* pCurrent = pBitmap;

	int i, j;
	for(i = 0; i < w; i++) {
		for(j = 0; j < h; j++) {
			pCurrent[i * w + j] = 100; 
		}
	}		
}
