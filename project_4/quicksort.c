#include <LPC17xx.h>
#include <RTL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "quicksort.h"
#include "array_tools.h"

// You decide what the threshold will be
#define USE_INSERTION_SORT 25

typedef struct {
	array_t array;
	size_t a;
	size_t c;
} array_interval_t;

typedef struct{
	array_interval_t interval;
	unsigned char priority;
} qsort_task_parameters_t;


void swap(array_type* array, int iA, int iB) {
	array_type temp = array[iA];
	array[iA] = array[iB];
	array[iB] = temp;
}

// Returns the index of the median
array_type find_med_three(array_interval_t* interval, int* const iMin, int* const iMax, int* const iMed) { 
	int a = interval->a;
	int c = interval->c;
	int b = (c - a) / 2;

	if(interval->array.array[a] > interval->array.array[b]) {
		if(interval->array.array[b] > interval->array.array[c]) {
			*iMax = a;
			*iMin = c;
			*iMed = b;
			return interval->array.array[b];
		}
		else if(interval->array.array[a] > interval->array.array[c]) {
			*iMax = a;
			*iMin = b;
			*iMed = c;
			return interval->array.array[c];
		}
		else {
			*iMax = c;
			*iMin = b;
			*iMed = a;
			return interval->array.array[a];
		}
	} else {
		if (interval->array.array[a] > interval->array.array[c]) {
			*iMax = b;
			*iMin = c;
			*iMed = a;
			return interval->array.array[a];
		}
		else if (interval->array.array[b] > interval->array.array[c]) {
			*iMax = b;
			*iMin = a;
			*iMed = c;
			return interval->array.array[c];
		}
		else {
			*iMax = c;
			*iMin = a;
			*iMed = b;
			return interval->array.array[b];
		}
	}
}

void insertion_sort( array_interval_t interval ) {
	int i, j, curr;
	for(i = interval.a; i < interval.c; i++) {
 		curr = interval.array.array[i];
 		j = i - 1;
 		while(j >= interval.a) {
 			if(interval.array.array[j] > interval.array.array[j]) {
 				interval.array.array[j + 1] = interval.array.array[j];
 				j--;
 			} else {
 				interval.array.array[j + 1] = curr;
 				break;
 			}
 		}
 	}
}

__task void quick_sort_task( void* void_ptr){
	qsort_task_parameters_t* params = (qsort_task_parameters_t*)void_ptr;
	qsort_task_parameters_t task_param_left;
	qsort_task_parameters_t task_param_right;
	array_interval_t left_interval;
	array_interval_t right_interval;

	// Pointer to array for clarity
	array_type* array = params->interval.array.array;

	// Indices for min/max/med for Median of Three
	int iMin, iMax, iMed;
	array_type vMin, vMax, vMed;

	int low = params->interval.a, high = params->interval.c, mid = (high - low) / 2;

	if(params->interval.c - params->interval.a < USE_INSERTION_SORT) {
		insertion_sort(params->interval);
	} else {
		vMed = find_med_three(&params->interval, &iMin, &iMax, &iMed);
		vMin = array[iMin];
		vMax = array[iMax];

		// Move min to first index, max to middle index, med to end
		array[low] = vMin;
		array[mid] = vMax;
		array[high] = vMed;

		while(high > low) {
			while(array[low] < vMed) low++;
			while(array[high] > vMed) high--;
			swap(array, low, high);
		}

		// Move pivot to proper location
		swap(array, params->interval.c, low);

		// Create child lasts for partitions
		left_interval.array =  params->interval.array;
		left_interval.a     =  params->interval.a;
		left_interval.c     =  mid;
		task_param_left.interval = left_interval;
		task_param_left.priority = params->priority - 1;

		right_interval.array =  params->interval.array;
		right_interval.a     =  mid;
		right_interval.c     =  params->interval.c;
		task_param_right.interval = right_interval;
		task_param_right.priority = params->priority - 1;
	
		// start the quick_sort threading
		os_tsk_create_ex(quick_sort_task, task_param_left.priority, &task_param_left); 
		os_tsk_create_ex(quick_sort_task, task_param_right.priority, &task_param_right); 
	}
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
	task_param.priority = 200;
	
	//start the quick_sort threading
	os_tsk_create_ex( quick_sort_task, task_param.priority, &task_param ); 
}
