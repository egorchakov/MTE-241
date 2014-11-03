#include <stdio.h>
#include <stdlib.h>

#include "array_tools.h"

// Local prototypes
static array_t generate_random_array( void );


static array_t generate_random_array( void ) {
	array_t result;
	size_t i,blk   = LARGE_ARRAY >> 3; //The default is 8k

	result.length = LARGE_ARRAY;
	result.array  = (array_type *) (0x2007C000);

	// The first blk is in reverse order
	for ( i = 0; i < blk ; ++i) {
		// To be generalized
		//     result.array[i] = (~(1 << sizeof( array_type )*8) - i) % (1 << sizeof( array_type )*8);

		result.array[i] = (0xFF - i) % 0x100;
	}

	// The next two blocks are random
	for ( i = blk; i < 3*blk ; ++i)
		result.array[i] =  rand() % 0x100;

	// The fourth block is constant
	for ( i = 3*blk; i < 4*blk ; ++i)
		result.array[i] =  0x0F;

	// The fifth block is ascending
	for ( i = 4*blk; i < 5*blk ; ++i)
		result.array[i] =  i % 0x100;

	// The last three are random
	for ( i = 5*blk; i < 8*blk ; ++i)
		result.array[i] =  rand() % 0x100;

	return result;
}

array_t generate_array( void ) {
	array_t result;
	size_t i;
	int n;

	printf("Enter the bag size or '0' for a large memory block: ");

	scanf("%d", &n);

	if ( n <= 0 ) {
		return generate_random_array();
	}

	result.length = n;

	printf("Enter each %d numbers, and hit Enter then: ", result.length);

	result.array = (array_type *) (0x2007C000);

	for(i = 0; i < result.length; ++i)
		scanf("%d", &( result.array[i]) );

	printf("\n");

	return result;
}

void print_array( array_t* a ) {
	size_t i;

	// for ( i = 0; i < a->length; ++i ) {
	// 	printf( " [%d]=%d", i, a->array[i] );
	// }

	for ( i = 0; i < a->length; ++i ) {
		printf( "%d\t", a->array[i] );
	}

	printf( "\n" );
}

/**
 *  This function tests whether or not an array is sorted.
 */

bool is_sorted_array( array_t *a ) {
	size_t i;

	for ( i = 1; i < a->length; ++i ) {
		if ( a->array[i - 1] > a->array[i] ) {
			printf("The array is not sorted in [%d]=%d > [%d]=%d\n", i-1, a->array[i-1], i, a->array[i]);
			return false;
		}
	}
  
	return true;
}
