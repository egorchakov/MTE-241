#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "quicksort_linux.h"
#include "array_tools.h"

// You decide what the threshold will be
#define USE_INSERTION_SORT 5
#define SINGLE_TASK

#define MAX_QSORT_TASK_COUNT 4
#define print printf
#define FLOOR(n)  (n) - ((n) % 1)
// #define MUTEXPRINT(...) {os_mut_wait(&printing, 0xffff); printf(__VA_ARGS__); os_mut_release(&printing);}

// typedef struct {
// 	array_t array;
// 	size_t a;
// 	size_t c;
// } array_interval_t;

// typedef struct{
// 	array_interval_t interval;
// 	unsigned char priority;
// } qsort_task_parameters_t;

volatile int num_tasks;

void insertion_sort( array_interval_t* interval ) {
  int i, j;
  array_type cur;
  array_type* array = interval->array.array;

  for (i = interval->a; i<=interval->c; i++){
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

    array_type* arr = interval->array.array;
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

inline void swap(array_type* arr, int a, int b){
    array_type tmp;
    tmp = arr[a];
    arr[a] = arr[b];
    arr[b] = tmp;
}

int partition(array_interval_t* interval){
    int pivot_index, pivot_value, tmp_index, i;

    tmp_index = interval->a;
    pivot_index = get_median_of_three(interval);
    pivot_value = interval->array.array[pivot_index];
    swap(interval->array.array, pivot_index, interval->c);
    for (i = interval->a; i<interval->c; i++){

        if (interval->array.array[i] < pivot_value){
            swap(interval->array.array, i, tmp_index);
            ++tmp_index;
        }
    }

    swap(interval->array.array, tmp_index, interval->c);
    return tmp_index;
}

// int array_interval_init(array_interval_t* interval, array_t* array, int a, int c){
//     interval->array = *array;
//     interval->a = a;
//     interval->c = c;
//     interval->array.length = c - a + 1; 
//     return 1;
// }

void quick_sort_task( void* void_ptr){

    int pivot_index;
    array_interval_t* interval;
    array_interval_t* left_interval;
    array_interval_t* right_interval;
    qsort_task_parameters_t* cur_params;
    qsort_task_parameters_t* left_task_params;
    qsort_task_parameters_t* right_task_params;

    cur_params = (qsort_task_parameters_t*) void_ptr;
    interval = &(cur_params->interval);

    printf("============================================\n");
    printf("PASSED: a = %d, c = %d, length = %d\n array = ", interval->a, interval->c, interval->array.length);
    print_array(&(interval->array));
    printf("============================================\n");

    left_interval = (array_interval_t*) malloc(sizeof(array_interval_t));
    right_interval = (array_interval_t*) malloc(sizeof(array_interval_t));
    left_task_params = (qsort_task_parameters_t*) malloc(sizeof(qsort_task_parameters_t));
    right_task_params = (qsort_task_parameters_t*) malloc(sizeof(qsort_task_parameters_t));
   
    if (interval->array.length > 1) {

        pivot_index = partition(interval);

        left_interval->array = interval->array;
        left_interval->a = interval->a;
        left_interval->c = pivot_index - 1;
        left_interval->array.length = left_interval->c - left_interval->a + 1;


        right_interval->array = interval->array;
        right_interval->a = pivot_index + 1;
        right_interval->c = interval->c;
        right_interval->array.length = right_interval->c - right_interval->a + 1;


        left_task_params->interval = *left_interval;
      
        right_task_params->interval = *right_interval;

        quick_sort_task(left_task_params);
        quick_sort_task(right_task_params);
        
    }

}

void quicksort_sem( array_t array ) {

	array_interval_t interval;
	qsort_task_parameters_t task_param;
    // Based on MTE 241 course notes--you can change this if you want
    //  - in the course notes, this sorts from a to c - 1
    interval.array =  array;
    interval.a     =  0;
    interval.c     =  array.length-1;
    task_param.interval = interval;

    quick_sort_task(&task_param);
}
