#ifndef __MEMHALF_H__
#define __MEMHALF_H__

#include "util.h"

#define NUM_BUCKETS 10
#define MAX_MEMORY 32768

// Header is 31 useful bits + 1 padding bit = 4 bytes
#define HEADER_SIZE 4
#define BLOCK_SIZE_MULTIPLE 32
// Compile flags
#define DEBUG_MEMORY

typedef struct memmap {
	unsigned int prev_block:10;
	unsigned int next_block:10;
	unsigned int block_size:10;
	bool alloc:1;
	bool padding:1;
} memmap_t;

typedef struct memmap_free {
	U32 memmap:32;
	unsigned short prev_free:10;
	unsigned short next_free:10;
} memmap_free_t;

static memmap_free_t* mprgmmap[NUM_BUCKETS] = { NULL };
static unsigned short rglut[NUM_BUCKETS] = { 64, 128, 256, 512, 1024, 2048, 4096, 8182, 16384, 32768 };
#ifdef DEBUG_MEMORY
static size_t free_memory = 0;
#endif

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