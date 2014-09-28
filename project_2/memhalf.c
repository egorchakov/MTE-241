#include "memhalf.h"

void* get_prev_block(memmap_t const* mmap){
	return (void*)mmap->prev_block;
}

void* get_next_block(memmap_t const* mmap){
	return (void*)mmap->next_block;
}

size_t get_block_size(memmap_t const* mmap){
	return (size_t)mmap->block_size;
}

bool get_allocated(memmap_t const* mmap){
	return mmap->alloc;
}

void* get_prev_free(memmap_free_t const* mmap){
	return (void*)mmap->prev_free;
}

void* get_next_free(memmap_free_t const* mmap){
	return (void*)mmap->next_free;
}

// Temporarily move from util.c
S16 floor_log2(size_t size){
    S16 i = 0;
    while (size >>= 1) i++;
    return i;
}

S16 ceil_log2(size_t size){
    return ( size == 1 ) ? 0 : floor_log2(size - 1) + 1;
}

S16 get_alloc_bucket_index(size_t size){
    // 2^6 = 64 is the first bucket, therefore indices are offset by 6
    S16 index = ceil_log2(size) - 6;
    return (index >=0 ) ? index : 0; 
}

S16 get_free_bucket_index(size_t size){
    // 2^6 = 64 is the first bucket, therefore indices are offset by 6
    S16 index = floor_log2(size) - 6;
    return (index >=0) ? index : 0;
}

void memmap_init(memmap_t* const mmap, size_t size){
	// Initialize the values for the fields
	mmap->prev_block = 0;
	mmap->next_block = 0;
	mmap->block_size = size;
	mmap->alloc = false;
}

void memmap_free_init(memmap_free_t* const mmap, size_t size){
	// memmap is the first field in memmap_free_t
	memmap_t* memmap_alloc = (memmap_t*)(mmap);
	memmap_init(memmap_alloc, size);
	mmap->memmap = (U32)memmap_alloc;
	mmap->prev_free = 0;
	mmap->next_free = 0;
}

void half_init(){
	memmap_free_t* block = (memmap_free_t*)malloc(MAX_MEMORY);
	memmap_free_init(block, MAX_MEMORY);
	mprgmmap[NUM_BUCKETS - 1] = block;
	#ifdef DEBUG_MEMORY
	free_memory = MAX_MEMORY;
	printf("\nHalf init succeeded: %d bytes free\n", free_memory);
	#endif
}

void* half_alloc(size_t n){
	unsigned short m = CEIL32(n + HEADER_SIZE);
	int i = 0;
	if(m > MAX_MEMORY) return NULL;
	i = get_alloc_bucket_index(m);
	while (i < NUM_BUCKETS && mprgmmap[i] == NULL) i++;

	#ifdef DEBUG_MEMORY
	printf("Requested m bytes: %d\n", m);
	printf("Getting memory from bucket: %d\n", i);
	#endif

	if(i >= NUM_BUCKETS || mprgmmap[i] == NULL) return NULL; // Out of memory
	memmap_free_t* mmap = mprgmmap[i];
	memmap_t* mmap_alloc = (memmap_t*)mmap;
	void* ptr_next = get_next_free(mmap);

	#ifdef DEBUG_MEMORY
	printf("Allocated block memory: %p\n", mmap);
	#endif

	// Remove allocated block from LL
	if(ptr_next){
		#ifdef DEBUG_MEMORY
		printf("Removed block from LL: %p\n", ptr_next);
		#endif
		memmap_free_t* mmap_next_free = (memmap_free_t*)(ptr_next - sizeof(memmap_free_t));
		mmap_next_free->prev_free = NULL;
		mprgmmap[i] = mmap_next_free; 
	}

	size_t mmap_size = get_block_size(mmap_alloc);
	mmap_alloc->alloc = __TRUE;
	if(mmap_size - m > 32) {
		void* new_block = ((void*)mmap) + m;
		// Can't call half_free cause half_free assumes header exists
		#ifdef DEBUG_MEMORY
		printf("Allocated memory size: %d\n", m);
		#endif
	}

	#ifdef DEBUG_MEMORY
	free_memory -= (m + sizeof(memmap_t));
	#endif

	return (((void*)mmap) + sizeof(memmap_free_t));
}

void half_free(void* ptr){
	// NOT WORKING YET
	
	// cast the preceding 32 bytes as memmap_free_t for further operations
	memmap_free_t* block = (memmap_free_t*) (ptr - 32);
	((memmap_t*) block->memmap)->alloc = false;
	
	// which bucket to insert into?
	S16 bucket_index = get_free_bucket_index((size_t)((memmap_t*) block->memmap)->block_size);

	// prepend to the corresponding linked list
	memmap_free_t* top_block = mprgmmap[bucket_index];
	top_block->prev_free = block;
	block->prev_free = NULL;
	block->next_free = top_block;
	mprgmmap[bucket_index] = block;
}

int main( int argc, char *argv[] ){
	printf("520 -> %d\n", CEIL32(520));
	printf("32 -> %d\n", CEIL32(32));
	printf("%d\n", get_alloc_bucket_index((size_t)1000));
	printf("%d\n", get_free_bucket_index((size_t)1000));

	half_init();
	void* dummy = half_alloc(32);
	printf("\nFree memory: %d\n", free_memory);
	fgetc(stdin);
	return 0;
}