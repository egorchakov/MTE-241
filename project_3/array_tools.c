#include <stdio.h>
#include <stdlib.h>

#include "array_tools.h"

//After the refactoring, it'd better to be moved into array_tools.h.
#define  SEGMENTS    ( 1 << 3  )

// Local prototypes
// static array_t generate_random_array( void );


// static array_t generate_random_array( void ) {
array_t generate_random_array( void ) {
	
  size_t i, j, blks;
  array_type max_u;
  array_t result;
  // printf("\nsize: %d\n", sizeof(result));

  if( ( LARGE_ARRAY * sizeof(array_type) )  > ( 1 << 14 ) ){
    result.length = 0;
    result.array = 0;
    printf( "The array size is too large.\n" );
    return result;
  }
  
  result.length = LARGE_ARRAY;
  // Statically allocating memory like: array = (array_type *) malloc(sizeof(array_type) * LARGE_ARRAY);
  // result.array = (array_type *) malloc(sizeof(array_type) * LARGE_ARRAY);
  result.array = (array_type *) (0x2007C000);
  //How many blocks are going to randomly assigned.
  blks = LARGE_ARRAY / SEGMENTS;
  //The maximum unsigned value that a variable of `array_type' 
  //can take is:
  max_u = ( 1 << ( sizeof(array_type) * 8 - 1) );
  
  for( i = 0; i <  SEGMENTS; ++i ){
    
    //What does has to be generated in the next block.
    //3 - random equal numbers. The probability of being 3 is ~15%.
		//    and ~28% to be 0,1,2.
    //0 - random increasing numbers.
    //1 - random decreasing numbers.
		//2 - random number.
    array_type cntl   = ( rand() % 7 ) % 4;
    array_type  eq_rnd = rand() % max_u;
    
    size_t min_blk = i * blks;
    size_t max_blk = (i + 1) * blks - 1;
    
    for( j = min_blk; j <= max_blk; ++j){

      switch (cntl){
        case 3:
          result.array[j] = eq_rnd;
          break;
        case 0:
          result.array[j] = ( j - min_blk ) % max_u ;
          break;
        case 1:
          result.array[j] = ( min_blk - j - 1 ) % max_u ;
          break;
				case 2:
          result.array[j] = rand() % max_u ;
          break;
      }//end of case
    }//end of for-j
  }//end of for-i
  
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

  //result.array = (array_type *) malloc(sizeof(array_type) * result.length);
	result.array = (array_type *) (0x2007C000);

	for(i = 0; i < result.length; ++i)
		scanf("%d", &( result.array[i]) );

	return result;
}

void print_array( array_t* a ) {
	size_t i;

	for ( i = 0; i < a->length; ++i ) {
		printf( "%d ", a->array[i] );
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
