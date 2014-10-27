#include <LPC17xx.h>
#include <RTL.h>
#include <stdio.h>

#include "quicksort.h"
#include "array_tools.h"

#undef PRINT_ARRAY

// Change 'undef' to 'define' when you have implemented quicksort_sem
#undef TEST_SEMAPHORE_IMPLEMENTATION

/**
*    The 'time' variable stores the current time in micro second. Any time period
*  after it can be calculated by subtracting `time' from a future time.
**/

volatile double time = 0;

/**
 *    'bask_task' is the main task that never stops. It caoomunicates with
 *  the user and creates new thread(s) to sort, check, and print the sorted
 *  array.
 **/

__task void base_task( void ) {
	array_t array;
	// Set the priority of the base task
	//  - lowest priority:     1
	//  - highest priority:  254

	os_tsk_prio_self( 1 );

	while ( 1 ) {
		array = generate_array();

		time = os_time_get();

		// Sort array
		#ifdef TEST_SEMAPHORE_IMPLEMENTATION
			quicksort_sem( array );
		#else
			quicksort( array );
		#endif

		time = os_time_get() - time;

		if( is_sorted_array( &array ) )
			printf("The array is sucessfully sorted in %.1f us.\n", time);
		//else
		//	printf("The array is not sorted!\n");

		#ifdef PRINT_ARRAY
			print_array( &array );
		#endif
	}
}

int main( void ) {

	SystemInit();
	SystemCoreClockUpdate();

	printf(" ");

	// We will change the value of the seed for the test
	srand( 10 );

	os_sys_init( base_task );

	while ( 1 ) {
		// Endless loop
	}
}
