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

bool get_allocated(memmap_t const* mmap){
	return mmap->alloc;
}

void* get_prev_free(memmap_free_t const* mmap){
	return (void*)mmap->prev_free;
}

void* get_next_free(memmap_free_t const* mmap){
	return (void*)mmap->next_free;
}

void set_block_size(memmap_t* mmap, size_t size){
	mmap->block_size = FLOOR32(size)/BLOCK_SIZE_MULTIPLE;
}

void set_prev_block(memmap_t* mmap, void* ptr){
	mmap->prev_block = (unsigned int)ptr;
}

void set_next_block(memmap_t* mmap, void* ptr){
	mmap->next_block = (unsigned int)ptr;
}

void set_allocated(memmap_t* mmap, bool alloc){
	mmap->alloc = alloc;
}

void set_prev_free(memmap_free_t* mmap, void* ptr){
	mmap->prev_free = (unsigned short)ptr;
}

void set_next_free(memmap_free_t* mmap, void* ptr){
	mmap->next_free = (unsigned short)ptr;
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
	set_block_size(mmap, size);
	mmap->alloc = false; 
}

void memmap_free_init(memmap_free_t* const mmap, size_t size){
	// memmap is the first field in memmap_free_t
	memmap_t* memmap_alloc = (memmap_t*)(mmap);
	memmap_init(memmap_alloc, size);
	mmap->prev_free = 0;
	mmap->next_free = 0;
}

void half_init(){
	memmap_free_t* block = (memmap_free_t*)malloc(MAX_MEMORY);
	memmap_free_init(block, MAX_MEMORY - HEADER_SIZE);
	mprgmmap[NUM_BUCKETS - 1] = block;
	#ifdef DEBUG_MEMORY
	free_memory = MAX_MEMORY;
	printf("\nHalf init succeeded: %d bytes free\n", free_memory);
	#endif
}

/*
	split_block returns a pointer to an additional block that results from splitting
	or NULL if splitting is not possible
*/

void* split_block(memmap_free_t* mmap_free, size_t required_size ){
	memmap_t* mmap_alloc = (memmap_t*) mmap_free;

	// splitting is only possible if the required size of the block is less than
	// (current size of the block) + 32 (minimum block size) + HEADER_SIZE (for the new block) 
	if (get_block_size(mmap_alloc) < required_size + HEADER_SIZE + BLOCK_SIZE_MULTIPLE)
		return NULL;


	size_t old_size = get_block_size(mmap_alloc);

	memmap_free_t* new_mmap_free = (memmap_free_t*) (mmap_alloc + HEADER_SIZE + required_size);
	memmap_t* new_mmap_alloc = (memmap_t*) new_mmap_free;

	set_block_size(mmap_alloc, required_size);
	set_block_size(new_mmap_alloc, old_size - required_size - HEADER_SIZE);

	// 			mmap_alloc 						new_mmap_alloc					some other block
	//
	// |[prev][next][size][alloc] ...| 	|[prev][next][size][alloc] ...|  |[prev][next][size][alloc] ...|
	new_mmap_alloc->prev_block = mmap_alloc;
	new_mmap_alloc->next_block = mmap_alloc->next_block;

	if (new_mmap_alloc){
		((memmap_t*) new_mmap_alloc->next_block)->prev_block = new_mmap_alloc;
	}

	mmap_alloc->next_block = new_mmap_alloc;

	return new_mmap_free;
}

void remove_free_block(memmap_free_t* mmap, size_t index){
	memmap_t* mmap_alloc = (memmap_t*)mmap;
	void* ptr_next = get_next_free(mmap);

	// Remove allocated block from LL
	if(ptr_next){
		#ifdef DEBUG_MEMORY
		printf("Removed block from LL: %p\n", ptr_next);
		#endif
		memmap_free_t* mmap_next_free = (memmap_free_t*)(ptr_next);
		mmap_next_free->prev_free = 0;
		mprgmmap[index] = mmap_next_free; 
	}
}

void insert_free_block(memmap_free_t* mmap){
	memmap_t* mmap_alloc = (memmap_t*)mmap;
	size_t block_size = get_block_size(mmap_alloc);
	int index = get_free_bucket_index(block_size);
	set_next_free(mmap, mprgmmap[index]);
	set_prev_free(mmap, NULL);
	mprgmmap[index] = mmap; 
	#ifdef DEBUG_MEMORY
		printf("Reinsert memory size: %d\n", block_size);
	#endif
}

void* half_alloc_2(size_t requested_block_size){
	unsigned short required_memory = CEIL32(requested_block_size + HEADER_SIZE);
	if (required_memory > MAX_MEMORY) return NULL;
	// from which bucket to allocate?
	int i = get_alloc_bucket_index(requested_block_size);
	printf("half_alloc | Starting search at bucket %d ... ", i);
	while (i<NUM_BUCKETS && mprgmmap[i] == NULL) i++;

	// no appropriate bucket found
	if (i >= NUM_BUCKETS || mprgmmap[i] == NULL) return NULL;

	printf("allocating from bucket %d\n",i );

	memmap_free_t* selected_block = mprgmmap[i];

	printf("half_alloc | Selected block size: %d\n", get_block_size((memmap_t*) selected_block->memmap));

	void* ptr_next_unallocated_block = get_next_free(selected_block);

	if (ptr_next_unallocated_block){
		memmap_free_t* next_unallocated_block = (
			(memmap_free_t*)(ptr_next_unallocated_block - sizeof(memmap_free_t)));
		next_unallocated_block->prev_free = 0;
		mprgmmap[i] = next_unallocated_block;
	}

	//split the block if it's larger than requested by at least 32 bytes
	if (get_block_size((memmap_t*) selected_block->memmap) - requested_block_size > 32){
		split_block(selected_block, requested_block_size);
		// half_free(((memmap_t*) selected_block->memmap)->next_block+HEADER_SIZE);
	}


	return ((void*)selected_block) + HEADER_SIZE;
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

	#ifdef DEBUG_MEMORY
	printf("Allocated block memory: %p\n", mmap);
	#endif

	// Remove allocated block from LL
	remove_free_block(mmap, i);

	#ifdef DEBUG_MEMORY
	free_memory -= (m + sizeof(memmap_t));
	#endif

	return (((void*)mmap) + sizeof(memmap_free_t));
}

void half_free(void* ptr){
	// NOT WORKING YET

	// cast the preceding 32 bits as memmap_t
	memmap_t* block = (memmap_t*)(ptr - HEADER_SIZE);
	block->alloc = __FALSE;

	// which bucket to insert into?
	S16 bucket_index = get_free_bucket_index(get_block_size(block));

	// recast the block as memmap_free_t for further operations
	memmap_free_t* free_block = (memmap_free_t*) block;

	// prepend to the corresponding linked list
	memmap_free_t* top_block = mprgmmap[bucket_index];

	top_block->prev_free = free_block;
	free_block->prev_free = NULL;
	free_block->next_free = top_block;
	mprgmmap[bucket_index] = free_block;
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