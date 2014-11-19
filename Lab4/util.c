#include "util.h"

void fill_rectangle(unsigned short* pBitmap, unsigned int w, unsigned int h, void* args) {
    unsigned short* val = (unsigned short*)args;
    unsigned short* pCurrent = pBitmap;

    int i, j;
    for(i = 0; i < w; i++) {
        for(j = 0; j < h; j++) {
            pCurrent[i * w + j] = *val; 
        }
    }       
}

unsigned short* gen_bitmap(size_t w, size_t h, void(*fill_fn)(unsigned short*, unsigned int, unsigned int, void*), void* args) {
    unsigned short* pBitmap = (unsigned short*)malloc(sizeof(unsigned short) * w * h);
    fill_fn(pBitmap, w, h, args);
    return pBitmap;
}

void draw_circle(circle_t c) {
    // STUB
}

void draw_rectangle(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned short val) {
    unsigned short* pBitmap = gen_bitmap(w, h, fill_rectangle, &val);
    GLCD_Bitmap(x, y, w, h, (unsigned char*)pBitmap);
    free(pBitmap);
 }
