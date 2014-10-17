#ifndef __HALF_FIT_H__
#define __HALF_FIT_H__

#include "util.h"

#define NUM_BUCKETS 11
#define MAX_MEMORY 32768

// Header is 31 useful bits + 1 padding bit = 4 bytes
#define HEADER_SIZE 4
#define BLOCK_SIZE_MULTIPLE 32

// Set alignment to 4 bytes
#pragma pack(push)
#pragma pack(4) 
typedef struct memmap {
	U16 prev_block:10;
	U16 next_block:10;
	U16 block_size:10;
	BOOL alloc:1;
	BOOL padding:1;
}__attribute__ ((__packed__))  memmap_t;

typedef struct memmap_free {
	U32 memmap:32;
	U16 prev_free:10;
	U16 next_free:10;
} __attribute__ ((__packed__)) memmap_free_t;
#pragma pack(pop) 

extern memmap_free_t* mprgmmap[];
extern unsigned char base_ptr[];

#ifdef DEBUG_MEMORY
static U32 free_memory = 0;
#endif

// Internal Helpers
__inline void* get_prev_block( memmap_t const* );
__inline void* get_next_block( memmap_t const* );
__inline U32 get_block_size( memmap_t const* );
__inline BOOL get_allocated( memmap_t const* );
__inline void* get_prev_free( memmap_free_t const* );
__inline void* get_next_free( memmap_free_t const* );

__inline void set_block_size( memmap_t*, U32 );
__inline void set_prev_block( memmap_t*, void* );
__inline void set_next_block( memmap_t*, void* );
__inline void set_allocated( memmap_t*, BOOL );
__inline void set_prev_free( memmap_free_t*, void* );
__inline void set_next_free( memmap_free_t*, void* );

void remove_free_block( memmap_free_t* );
void insert_free_block( memmap_free_t* );

__inline bool is_first_in_bucket(memmap_free_t*);
__inline bool is_last_in_bucket(memmap_free_t*);

__inline bool is_first_in_memory(memmap_t*);
__inline bool is_last_in_memory(memmap_t*);

void memmap_free_init(memmap_free_t* const, U32 );
void memmap_init(memmap_t* const, U32 );

memmap_free_t* split_block(memmap_free_t* const, U32);
memmap_free_t* coalesce_block(memmap_free_t*);
memmap_free_t* merge_block(memmap_free_t*, memmap_free_t*);

// Public interface
void half_init();
void *half_alloc( U32 n );
void half_free( void* );

#endif