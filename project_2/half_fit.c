#include "half_fit.h"

memmap_free_t* mprgmmap[NUM_BUCKETS] = { NULL }; 
void* base_ptr = NULL;

void* get_prev_block(memmap_t const* mmap){
	return (void*) ((mmap->prev_block)*BLOCK_SIZE_MULTIPLE + base_ptr);
}

void* get_next_block(memmap_t const* mmap){
	return (void*) ((mmap->next_block)*BLOCK_SIZE_MULTIPLE + base_ptr );
}

size_t get_block_size(memmap_t const* mmap){
	return (size_t) (mmap->block_size)*BLOCK_SIZE_MULTIPLE;
}

bool get_allocated(memmap_t const* mmap){
	return mmap->alloc;
}

void* get_prev_free(memmap_free_t const* mmap){
	return (void*) ((mmap->prev_free)*BLOCK_SIZE_MULTIPLE + base_ptr);
}

void* get_next_free(memmap_free_t const* mmap){
	return (void*) ((mmap->next_free)*BLOCK_SIZE_MULTIPLE + base_ptr);
}

void set_block_size(memmap_t* mmap, size_t size){
	mmap->block_size = FLOOR32(size)/BLOCK_SIZE_MULTIPLE;
}

void set_prev_block(memmap_t* mmap, void* ptr){
	if (ptr >= base_ptr)
		mmap->prev_block = (unsigned int) (ptr - base_ptr)/BLOCK_SIZE_MULTIPLE;
	#ifdef DEBUG_MEMORY
	else printf("[WARNING]: ptr < base_ptr | ptr:%d, base_ptr:%d\n", ptr, base_ptr);
	#endif
}	

void set_next_block(memmap_t* mmap, void* ptr){
	if (ptr >= base_ptr)
		mmap->next_block = (unsigned int) (ptr - base_ptr)/BLOCK_SIZE_MULTIPLE;
	#ifdef DEBUG_MEMORY
	else printf("[WARNING]: ptr < base_ptr | ptr:%d, base_ptr:%d\n", ptr, base_ptr);
	#endif
}

void set_allocated(memmap_t* mmap, bool alloc){
	mmap->alloc = alloc;
}

void set_prev_free(memmap_free_t* mmap, void* ptr){
	if (ptr >= base_ptr)
		mmap->prev_free = (unsigned int) (ptr - base_ptr)/BLOCK_SIZE_MULTIPLE;
	#ifdef DEBUG_MEMORY
	else printf("[WARNING]: ptr < base_ptr | ptr:%d, base_ptr:%d\n", ptr, base_ptr);
	#endif
} 

void set_next_free(memmap_free_t* mmap, void* ptr){
	if (ptr >= base_ptr)
		mmap->next_free = (unsigned int) (ptr - base_ptr)/BLOCK_SIZE_MULTIPLE;
	#ifdef DEBUG_MEMORY
	else printf("[WARNING]: ptr < base_ptr | ptr:%d, base_ptr:%d\n", ptr, base_ptr);
	#endif
}

bool is_first_in_bucket(memmap_free_t* mmap){
	return get_prev_free(mmap) == mmap;
}

bool is_last_in_bucket(memmap_free_t* mmap){
	return get_next_free(mmap) == mmap;
}

bool is_first_in_memory(memmap_t* mmap){
	return get_prev_block(mmap) == mmap;
}

bool is_last_in_memory(memmap_t* mmap){
	return get_next_block(mmap) == mmap;
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
    S16 index = ceil_log2(size) - FIRST_BUCKET_POWER;
    return (index >=0 ) ? index : 0; 
}

S16 get_free_bucket_index(size_t size){
    S16 index = floor_log2(size) - FIRST_BUCKET_POWER;
    return (index >=0) ? index : 0;
}

void memmap_init(memmap_t* const mmap, size_t size){
	// Initialize the values for the fields
	set_prev_block(mmap, mmap);
	set_next_block(mmap, mmap);
	set_block_size(mmap, size);
	set_allocated(mmap, false);
}

void memmap_free_init(memmap_free_t* const mmap, size_t size){
	// memmap is the first field in memmap_free_t
	memmap_t* memmap_alloc = (memmap_t*)(mmap);
	memmap_init(memmap_alloc, size);
	set_prev_free(mmap, mmap);
	set_next_free(mmap, mmap);
}

void half_init(){
	#ifdef _WIN32
	memmap_free_t* block = (memmap_free_t*) _aligned_malloc(BLOCK_SIZE_MULTIPLE, MAX_MEMORY);
	#else
	memmap_free_t* block = (memmap_free_t*) aligned_alloc(BLOCK_SIZE_MULTIPLE, MAX_MEMORY);
	#endif
	base_ptr = block;
	memmap_free_init(block, MAX_MEMORY);
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

memmap_free_t* split_block(memmap_free_t* mmap_free, size_t required_size ){
	memmap_t* mmap_alloc = (memmap_t*) mmap_free;

	// splitting is only possible if the required size of the block is less than
	// CEIL32((current size of the block) + 32 (minimum block size) + HEADER_SIZE (for the new block)) 
	if (get_block_size(mmap_alloc) < required_size + BLOCK_SIZE_MULTIPLE)
		return NULL;

	size_t old_size = get_block_size(mmap_alloc);

	memmap_free_t* new_mmap_free = (memmap_free_t*) (CEIL32((U32)mmap_alloc + (U32)required_size));
	memmap_t* new_mmap_alloc = (memmap_t*) new_mmap_free;

	/*

	=========================================================================================================
	 								mmap_alloc 									some other block
	
		|[prev][next][size][alloc] .......................................|  |[prev][next][size][alloc] ...|
	=========================================================================================================

	↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	=========================================================================================================
	 			mmap_alloc 						new_mmap_alloc					some other block
	
		 |[prev][next][size][alloc] ...| 	|[prev][next][size][alloc] ...|  |[prev][next][size][alloc] ...|
	=========================================================================================================
	
	*/

	// If mmap_alloc wasn't the last block in memory before splitting,
	// we need to link the newly created block to whatever used to be 
	// after mmap_alloc in memory
	if (!is_last_in_memory(mmap_alloc)){
		set_next_block(new_mmap_alloc, get_next_block(mmap_alloc));
		set_prev_block((memmap_t*) get_next_block(mmap_alloc), new_mmap_alloc);
	}
	// Otherwise, just link the newly created block to itself (i.e. terminate)
	else {
		set_next_block(new_mmap_alloc, new_mmap_alloc);
	}
	
	// Linking mmap_alloc and the newly created new_mmap_alloc
	set_prev_block(new_mmap_alloc, mmap_alloc);
	set_next_block(mmap_alloc, new_mmap_alloc);

	// Finally, block sizes (only mmap_alloc and new_mmap_alloc are affected)
	set_block_size(mmap_alloc, required_size);
	set_block_size(new_mmap_alloc, old_size - required_size);
	#ifdef DEBUG_MEMORY
	printf("split_block | old size: %d, required_size: %d, new sizes: %d and %d\n", 
		old_size, required_size, get_block_size(mmap_alloc), get_block_size(new_mmap_alloc));
	#endif
	return new_mmap_free;
}

memmap_free_t* coalesce_block(memmap_free_t* mmap){
	memmap_t* mmap_alloc = (memmap_t*)mmap;
	memmap_t* mmap_left = get_prev_block(mmap_alloc);
	memmap_t* mmap_right = get_next_block(mmap_alloc);

	if(!is_first_in_memory(mmap_alloc) && !get_allocated(mmap_left)) {
		printf("(Left) Merging %d and %d, ", get_block_size(mmap_left), get_block_size(mmap_alloc));
		mmap = merge_block((memmap_free_t*)mmap_left, mmap);
		printf("final size: %d\n", get_block_size((memmap_t*)mmap));
	}

	if(!is_last_in_memory(mmap_alloc) && !get_allocated(mmap_right)) { 
		printf("(Right) Merging %d and %d, ", get_block_size(mmap_alloc), get_block_size(mmap_right));
		mmap = merge_block(mmap, (memmap_free_t*)mmap_right);
		printf("final size: %d\n", get_block_size((memmap_t*)mmap));
	}
	return mmap;
}

memmap_free_t* merge_block(memmap_free_t* mmap_left, memmap_free_t* mmap_right){
	memmap_t* mmap_left_alloc = (memmap_t*) mmap_left;
	memmap_t* mmap_right_alloc = (memmap_t*) mmap_right;
	if(is_last_in_memory(mmap_right_alloc)) {
		set_next_block(mmap_left_alloc, mmap_left_alloc);
	}
	else {
		set_next_block(mmap_left_alloc, get_next_block(mmap_right_alloc));
		set_prev_block(get_next_block(mmap_right_alloc), mmap_left_alloc);
	}
	#ifdef DEBUG_MEMORY
	printf("Left: %d, right: %d ,", get_block_size(mmap_left)_alloc, get_block_size(mmap_right_alloc);
	#endif 
	remove_free_block(mmap_right);
	remove_free_block(mmap_left);
	set_block_size(mmap_left_alloc, get_block_size(mmap_left_alloc) + get_block_size(mmap_right_alloc));
	#ifdef DEBUG_MEMORY
	printf("new left:  %d\n", get_block_size(mmap_left_alloc));
	#endif
	return mmap_left;
}

void remove_free_block(memmap_free_t* mmap){
	memmap_t* mmap_alloc = (memmap_t*)mmap;
	int index = get_free_bucket_index(get_block_size(mmap_alloc));
	if (!is_last_in_bucket(mmap)){
		if (!is_first_in_bucket(mmap)){
			set_next_free(get_prev_free(mmap), get_next_free(mmap));
			set_prev_free(get_next_free(mmap), get_prev_free(mmap));
		}

		else {
			mprgmmap[index] = get_next_free(mmap);
			set_prev_free(get_next_free(mmap), get_next_free(mmap));
		}
	}

	else {
		if (!is_first_in_bucket(mmap)){
			set_next_free(get_prev_free(mmap), get_prev_free(mmap));
		}

		else {
			mprgmmap[index] = NULL;
		}
	}

	// "Isolating" the block
	set_prev_free(mmap, mmap);
	set_next_free(mmap, mmap);
}

void insert_free_block(memmap_free_t* mmap){
	memmap_t* mmap_alloc = (memmap_t*)mmap;
	size_t block_size = get_block_size(mmap_alloc);
	int index = get_free_bucket_index(block_size);

	if (mprgmmap[index] == mmap){
		printf(" \
			[WARNING]: The passed block is the same as the \
			first block in the bucket. This block might have \
			been previously inserted \
			");
		return;
	}
	// some blocks present in the bucket
	if (mprgmmap[index]) {
		set_prev_free(mprgmmap[index], mmap);
		set_next_free(mmap, mprgmmap[index]);
	}
	else{
		set_next_free(mmap, mmap);
	}

	set_prev_free(mmap, mmap);
	mprgmmap[index] = mmap; 
	
	#ifdef DEBUG_MEMORY
		printf("Reinsert memory size: %d\n", block_size);
	#endif
}

void* half_alloc_2(size_t requested_block_size){
	unsigned short required_memory = CEIL32(requested_block_size + HEADER_SIZE);
	if (required_memory > MAX_MEMORY) return NULL;

	// from which bucket to allocate?
	int i = get_alloc_bucket_index(required_memory);
	#ifdef DEBUG_MEMORY
	printf("half_alloc | Starting search at bucket %d ... ", i);
	#endif
	while (i<NUM_BUCKETS && mprgmmap[i] == NULL) i++;

	// no appropriate bucket found
	if (i >= NUM_BUCKETS || mprgmmap[i] == NULL) return NULL;

	#ifdef DEBUG_MEMORY
	printf(" allocating from bucket %d\n",i );
	#endif
	memmap_free_t* selected_block_free = mprgmmap[i];
	memmap_t* selected_block_alloc = (memmap_t*) selected_block_free;
	#ifdef DEBUG_MEMORY
	printf("half_alloc | Selected block size: %d\n", get_block_size(selected_block_alloc));
	#endif
	
	remove_free_block(selected_block_free);
	set_allocated(selected_block_alloc, true);

	//split the block if it's larger than requested by at least 32 bytes
	if (get_block_size(selected_block_alloc) - required_memory > BLOCK_SIZE_MULTIPLE){
		memmap_free_t* additional_block = split_block(selected_block_free, required_memory);
		additional_block = coalesce_block(additional_block);
		#ifdef DEBUG_MEMORY
		printf("half_alloc_2 | ");
		#endif
		insert_free_block(additional_block);
	}

	return ((void*)selected_block_alloc) + HEADER_SIZE;
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
	remove_free_block(mmap);

	#ifdef DEBUG_MEMORY
	free_memory -= (m + sizeof(memmap_t));
	#endif

	return (((void*)mmap) + sizeof(memmap_free_t));
}

void half_free(void* ptr){
	memmap_free_t* block_free = (memmap_free_t*) (ptr - HEADER_SIZE);
	memmap_t* block_alloc = (memmap_t*) block_free;
	if (!get_allocated(block_alloc)) return;
	block_free = coalesce_block(block_free);
	#ifdef DEBUG_MEMORY
	printf("half_free | ");
	#endif
	insert_free_block(block_free);
	set_allocated(block_alloc, false);
}