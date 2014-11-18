#ifndef _TASKS_H
#define _TASKS_H

#include <LPC17xx.h>
#include <RTL.h>
#include "GLCD.h"

__task void init_task(void);
__task void redraw_objects(void* args);

#endif