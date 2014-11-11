#ifndef __QUICKSORT_H__
#define __QUICKSORT_H__

#include "array_tools.h"

typedef struct {
    array_t array;
    size_t a;
    size_t c;
} array_interval_t;

typedef struct{
    array_interval_t interval;
} qsort_task_parameters_t;

void quicksort( array_t );
// Uncomment this when you implement Project 4
void quicksort_sem( array_t );

#endif
