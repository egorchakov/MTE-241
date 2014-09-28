#include "memhalf.h"

void* get_prev_block(memmap_t const* mmap){
	return (void*)mmap->prev_block;
}

void* get_next_block(memmap_t const* mmap){
	return (void*)mmap->next_block;
}

size_t get_block_size(memmap_t const* mmap){
	return (size_t) (mmap->block_size)*BLOCK_SIZE_MULTIPLE;
}

void set_block_size(memmap_t* mmap, size_t size){
	mmap->block_size = FLOOR32(size)/BLOCK_SIZE_MULTIPLE;
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

void memmap_init(memmap_t* const mmap, size_t size){
	// Initialize the values for the fields
	mmap->prev_block = 0;
	mmap->next_block = 0;
	set_block_size(mmap, size);
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
	memmap_free_init(block, MAX_MEMORY - HEADER_SIZE);
	mprgmmap[NUM_BUCKETS - 1] = block;
	#ifdef DEBUG_MEMORY
	free_memory = MAX_MEMORY;
	#endif
}

void* half_alloc(size_t n){
	unsigned short m = CEIL32(n + HEADER_SIZE);
	int i = 0;
	if(m > MAX_MEMORY) return NULL;
	i = get_alloc_bucket_index(m);
	while (i<NUM_BUCKETS && mprgmmap[i] == NULL) i++;
	if (i >= NUM_BUCKETS || mprgmmap[i] == NULL) return NULL;
	if(i >= NUM_BUCKETS || mprgmmap[i] == NULL) return NULL; // Out of memory
	memmap_free_t* mmap = mprgmmap[i];
	memmap_t* mmap_alloc = (memmap_t*)mmap;
	void* ptr_next = get_next_free(mmap);

	// Remove allocated block from LL
	if(ptr_next){
		memmap_free_t* mmap_next_free = (memmap_free_t*)(ptr_next - sizeof(memmap_free_t));
		mmap_next_free->prev_free = NULL;
		mprgmmap[i] = mmap_next_free; 
	}

	size_t mmap_size = get_block_size(mmap_alloc);
	mmap_alloc->alloc = __TRUE;
	if(mmap_size - m > 32) {
		void* new_block = ((void*)mmap) + sizeof(memmap_free_t) + m;
		half_free(new_block);
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
	S16 bucket_index = get_free_bucket_index(((memmap_t*) block->memmap)->block_size);

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
	printf("%d\n", get_alloc_bucket_index(1000));
	printf("%d\n", get_free_bucket_index(1000));

	half_init();
	void* dummy = half_alloc(32);
	printf("\nFree memory: %d\n", free_memory);
}