#ifndef _TASKS_H
#define _TASKS_H

#include <LPC17xx.h>
#include <RTL.h>
#include <math.h>
//#include "GLCD.h"
//#include "config.h"
#include "animation.h"
//#include "util.h"
#include "sync.h"

extern OS_SEM add_random_object_sem;
extern OS_SEM redraw_objects_sem;
extern OS_SEM redraw_objects_done_sem;
extern volatile BOOL add_object_flag;

extern volatile unsigned short int ADC_Value;
extern volatile unsigned char ADC_Done; 

extern OS_TID object_tasks[MAX_OBJECTS];

extern volatile unsigned int delay_interval;

__task void init_task(void);
__task void redraw_objects(void);
__task void add_object(void* args);
__task void add_random_object(void);
__task void object_task(void* args);

#endif
