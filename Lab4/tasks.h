#ifndef _TASKS_H
#define _TASKS_H

#include <LPC17xx.h>
#include <RTL.h>
#include "GLCD.h"
#include "config.h"
#include "circle.h"
#include "animation.h"
#include "util.h"

extern OS_SEM add_random_object_sem;
extern OS_SEM redraw_objects_sem;
extern OS_SEM redraw_objects_done_sem;
extern OS_SEM done;

__task void init_task(void);
__task void redraw_objects(void);
__task void add_object(void* args);
__task void add_random_object(void);

#endif
