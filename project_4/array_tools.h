#ifndef __ARRAY_TOOLS_H__
#define __ARRAY_TOOLS_H__

#include <stdbool.h>
#include <stdlib.h>

typedef  unsigned char array_type;

#define LARGE_ARRAY (1 << 13)

typedef struct {
	array_type *array;
	size_t length;
} array_t;

array_t generate_array( void );
void print_array( array_t * );
bool is_sorted_array( array_t * );

#endif
