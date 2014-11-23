#ifndef _ANIMATION_H
#define _ANIMATION_H

#include <LPC17xx.h>
#include <RTL.h>
#include <stdio.h>
#include <stdlib.h>

#include "GLCD.h"
#include "config.h"
#include "util.h"

typedef circle_t object_t;

extern volatile int active_objects;
extern object_t objects[MAX_OBJECTS];

extern const unsigned short colors[NUM_RANDOM_COLORS];
#endif
