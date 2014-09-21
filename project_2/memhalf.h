#ifndef __MEMHALF_H__
#define __MEMHALF_H__

#include "util.h"

typedef struct memmap {
	short prev_block:10;
	short next_block:10;
	short block_size:10;
	bool alloc:1;
	bool padding:1;
} memmap_t;

typedef struct memmap_alloc {
	S32 memmap:32;
	short prev_free:10;
	short next_free:10;
} memmap_alloc_t;

void *half_alloc( size_t n );
void half_free( void* );

#endif