#ifndef _CONFIG_H
#define _CONFIG_H

#define MAX_OBJECTS 10
#define MAX_BITMAP_SIZE 600 // 20 x 20
#define BG_COLOR White

#define HIGHEST_PRIO 253
#define LOWEST_PRIO 1

#define ACTIVE 0x0001
#define FOREVER 0xffff

#define MIN_DX 1
#define MAX_DX 5

#define MIN_DY 1
#define MAX_DY 5

#define X_MIN 0
#define X_MAX 305
#define Y_MIN 0 
#define Y_MAX 225

#define NUM_RANDOM_COLORS 9
#define MIN_RADIUS 5
#define MAX_RADIUS 15

// max and min ADC values are measured empirically
#define MAX_ADC_VALUE 4090 
#define MIN_ADC_VALUE 5

#define MAX_DELAY 10000
#define MIN_DELAY 0

#define MIN_ADD_DELAY 10000 // 10 ms

#endif

