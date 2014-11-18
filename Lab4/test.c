#include <stdio.h>

#include "quicksort_linux.h"
#include "array_tools.h"

void main(void){
    array_t array;
    // array = generate_random_array();
    array = generate_array();
    quicksort_sem(array);
    if( is_sorted_array( &array ) )
        printf("The array is sucessfully sorted\n");
    else {
        printf("The array is not sorted!\n");
        return;
    }
}