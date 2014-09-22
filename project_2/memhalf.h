#ifndef __MEMHALF_H__
#define __MEMHALF_H__

#include "util.h"

#define NUM_BUCKETS 10

typedef struct memmap {
	unsigned short prev_block:10;
	unsigned short next_block:10;
	unsigned short block_size:10;
	bool alloc:1;
	bool padding:1;
} memmap_t;

typedef struct memmap_free {
	S32 memmap:32;
	unsigned short prev_free:10;
	unsigned short next_free:10;
} memmap_free_t;

memmap_free_t* rgmmap[NUM_BUCKETS];

// Internal Helpers
void* get_prev_block( memmap_t const* );
void* get_next_block( memmap_t const* );
size_t get_block_size( memmap_t const* );
bool get_allocated( memmap_t const* );
void* get_prev_free( memmap_free_t const* );
void* get_next_free( memmap_free_t const* );

void memmap_free_init(memmap_free_t* const, size_t );
void memmap_init(memmap_t* const, size_t );

// Public interface
void half_init();
void *half_alloc( size_t n );
void half_free( void* );

#endif