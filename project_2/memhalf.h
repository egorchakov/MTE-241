#ifndef __MEMHALF_H__
#define __MEMHALF_H__

#include "util.h"

#define NUM_BUCKETS 11
#define FIRST_BUCKET_POWER 5
#define MAX_MEMORY 32768

// Header is 31 useful bits + 1 padding bit = 4 bytes
#define HEADER_SIZE 4
#define BLOCK_SIZE_MULTIPLE 32
// Compile flags
#define DEBUG_MEMORY

// Set alignment to 4 bytes
#pragma pack(push)
#pragma pack(HEADER_SIZE) 
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
#pragma pack(pop) 

extern memmap_free_t* mprgmmap[];
extern void* base_ptr;

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

void set_block_size( memmap_t*, size_t );
void set_prev_block( memmap_t*, void* );
void set_next_block( memmap_t*, void* );
void set_allocated( memmap_t*, bool );
void set_prev_free( memmap_free_t*, void* );
void set_next_free( memmap_free_t*, void* );

void remove_free_block( memmap_free_t*, size_t );
void insert_free_block( memmap_free_t* );

bool is_first_in_bucket(memmap_free_t*);
bool is_last_in_bucket(memmap_free_t*);

bool is_first_in_memory(memmap_t*);
bool is_last_in_memory(memmap_t*);

// Temporarily move from util
S16 floor_log2(size_t );
S16 ceil_log2(size_t );

S16 get_alloc_bucket_index(size_t );
S16 get_free_bucket_index(size_t );

void memmap_free_init(memmap_free_t* const, size_t );
void memmap_init(memmap_t* const, size_t );

memmap_free_t* split_block(memmap_free_t* const, size_t);
memmap_free_t* coalesce_block(memmap_free_t*);
memmap_free_t* merge_block(memmap_free_t*, memmap_free_t*);

// Public interface
void half_init();
void *half_alloc( size_t n );
void half_free( void* );

#endif