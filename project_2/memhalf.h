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

typedef struct memmap_free {
	S32 memmap:32;
	short prev_free:10;
	short next_free:10;
} memmap_free_t;

void* get_prev_block( memmap_t const* );
void* get_next_block( memmap_t const* );
size_t get_block_size( memmap_t const* );
bool get_allocated( memmap_t const* );
void* get_prev_free( memmap_free_t const* );
void* get_next_free( memmap_free_t const* );

void *half_alloc( size_t n );
void half_free( void* );

#endif