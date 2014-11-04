#include <LPC17xx.h>
#include <RTL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "quicksort.h"
#include "array_tools.h"

// You decide what the threshold will be
#define USE_INSERTION_SORT 5

typedef struct {
	array_t array;
	size_t a;
	size_t c;
} array_interval_t;

typedef struct{
	array_interval_t interval;
	unsigned char priority;
} qsort_task_parameters_t;


void insertion_sort( array_interval_t interval ) {
  int i, j;
  array_type cur;
  array_type* array = interval.array.array;

  for (i = interval.a; i<=interval.c; i++){
  	cur = array[i];
  	j = i;
  	while(j>0 && array[j-1] > cur){
  		array[j] = array[j-1];
  		j--;
  	}
  	array[j] = cur;
  }
}

int get_median_of_three( array_interval_t* interval ){
    int a,b,c;

    array_type* const arr = interval->array.array;
    a = interval->a;
    c = interval->c;
    b = FLOOR(a/2 + c/2);

    if (arr[a] >= arr[b]){
        if (arr[b] >= arr[c]) return b;
        else if (arr[c] >= arr[a]) return a;
        else return c;               
    }
    else {
        if (arr[a] >= arr[c]) return a;
        else if (arr[b] >= arr[c]) return c;
        else return b;
    }
}

__task void quick_sort_task( void* void_ptr){
  // Your implementation here
}

void quicksort( array_t array ) {
	array_interval_t interval;
	qsort_task_parameters_t task_param;
	
	// Based on MTE 241 course notes--you can change this if you want
	//  - in the course notes, this sorts from a to c - 1
	interval.array =  array;
	interval.a     =  0;
	interval.c     =  array.length;
	
	task_param.interval = interval;

	// If you are using priorities, you can change this
	task_param.priority = 10;
	
	//start the quick_sort threading
	os_tsk_create_ex( quick_sort_task, task_param.priority, &task_param ); 
}
