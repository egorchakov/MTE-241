#ifndef _TASKS_H
#define _TASKS_H

#include <LPC17xx.h>
#include <RTL.h>
#include "GLCD.h"
#include <math.h>
#include "animation.h"
#include "graphics.h"
#include "physics.h"
#include "util.h"

extern volatile BOOL add_object_flag;
extern volatile unsigned short int ADC_Value;
extern volatile unsigned char ADC_Done; 

__task void init_task(void);
__task void redraw_objects(void);
__task void add_object(void* args);
__task void add_random_object(void);
__task void object_task(void* args);

#endif
