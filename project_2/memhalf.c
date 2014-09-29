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

void split_block(memmap_free_t* block, size_t required_size ){
	if (get_block_size((memmap_t*) block->memmap) < required_size + HEADER_SIZE + 32)
		return NULL;

	size_t old_size = get_block_size((memmap_t*) block->memmap);
	memmap_free_t* new_block = (memmap_free_t*) (block + HEADER_SIZE + required_size);

	set_block_size((memmap_t*) block->memmap, required_size);
	set_block_size((memmap_t*) new_block->memmap, old_size - required_size - HEADER_SIZE);

	((memmap_t*) new_block->memmap)->prev_block = (memmap_t*) block->memmap;
	((memmap_t*) new_block->memmap)->next_block = (memmap_t*) ((memmap_t*) block->memmap)->next_block;
	((memmap_t*) block->memmap)->next_block = (memmap_t*) new_block->memmap;
}

void* half_alloc_2(size_t requested_block_size){
	size_t block_size = CEIL32(requested_block_size);
	printf("half_alloc | Will be allocated: %d\n", block_size);
	unsigned short required_memory = block_size + HEADER_SIZE;
	if (required_memory > MAX_MEMORY) return NULL;
	// from which bucket to allocate?
	int i = get_alloc_bucket_index(block_size);
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
		next_unallocated_block->prev_free = NULL;
		mprgmmap[i] = next_unallocated_block;
	}

	//split the block if it's larger than requested by at least 32 bytes
	if (get_block_size((memmap_t*) selected_block->memmap) - block_size > 32){
		split_block(selected_block, block_size);
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
	
	// cast the preceding 32 bits as memmap_free_t for further operations
	memmap_free_t* block = (memmap_free_t*) (ptr - HEADER_SIZE);
	printf("a\n");
	printf("d\n", ((memmap_t*) block->memmap)->alloc);
	((memmap_t*) block->memmap)->alloc = __FALSE;
	printf("b\n");
	
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