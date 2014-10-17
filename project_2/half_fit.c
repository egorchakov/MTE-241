#include "half_fit.h"

// Initializing buckets and the initial memory
memmap_free_t* mprgmmap[NUM_BUCKETS] = { NULL }; 
unsigned char base_ptr[MAX_MEMORY] __attribute__ (
    (section(".ARM.__at_0x10000000"), zero_init));


/*******************************************************************************
                                
                            INTERNAL HELPERS/WRAPPERS
                            
*******************************************************************************/

__inline void* get_prev_block(memmap_t const* mmap){
    return (void*) ((unsigned char*)base_ptr + 
        (mmap->prev_block) * BLOCK_SIZE_MULTIPLE);
}

__inline void* get_next_block(memmap_t const* mmap){
    return (void*) ((unsigned char*)base_ptr + 
        (mmap->next_block) * BLOCK_SIZE_MULTIPLE);
}

__inline U32 get_block_size(memmap_t const* mmap){
    return ((U32) (mmap->block_size) == 0 ? 
        MAX_MEMORY : (U32) (mmap->block_size)* BLOCK_SIZE_MULTIPLE);
}

__inline BOOL get_allocated(memmap_t const* mmap){
    return mmap->alloc;
}

__inline void* get_prev_free(memmap_free_t const* mmap){
    return (void*) ((unsigned char*)base_ptr +
        (mmap->prev_free) * BLOCK_SIZE_MULTIPLE);
}

__inline void* get_next_free(memmap_free_t const* mmap){
    return (void*) ((unsigned char*)base_ptr +
        (mmap->next_free) * BLOCK_SIZE_MULTIPLE);
}

__inline void set_block_size(memmap_t* mmap, U32 size){
    #ifdef DEBUG_MEMORY
    if(FLOOR32(size) != size) 
        printf("WARN: Size is not a multiple of 32: %d bytes \n", size);
    #endif
    mmap->block_size = ((size == MAX_MEMORY) ? 0 : size / BLOCK_SIZE_MULTIPLE);
}

__inline void set_prev_block(memmap_t* mmap, void* ptr){
    if (ptr >= base_ptr)
        mmap->prev_block = (U16) ((unsigned char*)ptr -
            (unsigned char*)base_ptr) / BLOCK_SIZE_MULTIPLE;
    #ifdef DEBUG_MEMORY
    else printf("WARN: ptr < base_ptr | ptr:%d, base_ptr:%d\n", ptr, base_ptr);
    #endif
}   

__inline void set_next_block(memmap_t* mmap, void* ptr){
    if (ptr >= base_ptr)
        mmap->next_block = (U16) ((unsigned char*) ptr -
         (unsigned char*)base_ptr) / BLOCK_SIZE_MULTIPLE;

    #ifdef DEBUG_MEMORY
    else printf("WARN: ptr < base_ptr | ptr:%d, base_ptr:%d\n", ptr, base_ptr);
    #endif
}

__inline void set_allocated(memmap_t* mmap, BOOL alloc){
    mmap->alloc = alloc;
}

__inline void set_prev_free(memmap_free_t* mmap, void* ptr){
    if (ptr >= base_ptr)
        mmap->prev_free = (U16) ((unsigned char*)ptr -
            (unsigned char*)base_ptr) / BLOCK_SIZE_MULTIPLE;

    #ifdef DEBUG_MEMORY
    else printf("WARN: ptr < base_ptr | ptr:%d, base_ptr:%d\n", ptr, base_ptr);
    #endif
} 

__inline void set_next_free(memmap_free_t* mmap, void* ptr){
    if (ptr >= base_ptr)
        mmap->next_free = (U16) ((unsigned char*)ptr -
            (unsigned char*)base_ptr) / BLOCK_SIZE_MULTIPLE;

    #ifdef DEBUG_MEMORY
    else printf("WARN: ptr < base_ptr | ptr:%d, base_ptr:%d\n", ptr, base_ptr);
    #endif
}

__inline bool is_first_in_bucket(memmap_free_t* mmap){
    return get_prev_free(mmap) == mmap;
}

__inline bool is_last_in_bucket(memmap_free_t* mmap){
    return get_next_free(mmap) == mmap;
}

__inline bool is_first_in_memory(memmap_t* mmap){
    return get_prev_block(mmap) == mmap;
}

__inline bool is_last_in_memory(memmap_t* mmap){
    return get_next_block(mmap) == mmap;
}


/*******************************************************************************
                                
                                IMPLEMENTATION
                            
*******************************************************************************/

/*
    void memmap_init(emmap_t* const mmap, U32 size)
    
    Initializes a struct of type `memmap` with a given size.
*/
void memmap_init(memmap_t* const mmap, U32 size){
    // Initialize the values for the fields
    set_prev_block(mmap, mmap);
    set_next_block(mmap, mmap);
    set_block_size(mmap, size);
    set_allocated(mmap, __FALSE);
}

/*
    void memmap_free_init(memmap_free_t* const mmap, U32 size)

    Initializes a struct of type `memmap_free` with a given size.
*/
void memmap_free_init(memmap_free_t* const mmap, U32 size){
    // memmap is the first field in memmap_free_t
    memmap_t* memmap_alloc = (memmap_t*)(mmap);
    memmap_init(memmap_alloc, size);
    set_prev_free(mmap, mmap);
    set_next_free(mmap, mmap);
}

/*
    void half_init()
    
    Initializes the allocation algorithm.
*/
void half_init(){
    U8 i; // Index into bucket
    
    // Reinitialize each bucket
    for(i = 0; i < NUM_BUCKETS; ++i) 
        mprgmmap[i] = NULL;

    memmap_free_init((memmap_free_t*)base_ptr, MAX_MEMORY);
    mprgmmap[NUM_BUCKETS - 1] = (memmap_free_t*)base_ptr;
    #ifdef DEBUG_MEMORY
    free_memory = MAX_MEMORY;
    printf("\nHalf init succeeded: %d bytes free\n", free_memory);
    #endif
}

/*
    memmap_free_t* split_block(memmap_free_t* mmap_free, U32 required_size )
    
    Splits a block to match the required size.

    Returns a pointer to an additional block that results from splitting
    or NULL if splitting is not possible.
*/

memmap_free_t* split_block(memmap_free_t* mmap_free, U32 required_size ){
    memmap_t* mmap_alloc = (memmap_t*) mmap_free;
    U32 old_size;
    memmap_free_t* new_mmap_free; 
    memmap_t* new_mmap_alloc;

    /* splitting is only possible if the required size of the block is less than
       CEIL32((current size of the block) + 32 (minimum block size) + 
       HEADER_SIZE (for the new block)) 
    */
    if (get_block_size(mmap_alloc) < required_size + BLOCK_SIZE_MULTIPLE)
        return NULL;

    old_size = get_block_size(mmap_alloc);

    new_mmap_free = (memmap_free_t*) (CEIL32((U32)mmap_alloc + required_size));
    new_mmap_alloc = (memmap_t*) new_mmap_free;


    /* If mmap_alloc wasn't the last block in memory before splitting,
       we need to link the newly created block to whatever used to be 
       after mmap_alloc in memory
    */
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
    set_allocated(new_mmap_alloc, __FALSE);
    #ifdef DEBUG_MEMORY
    printf("split_block | old size: %d, required: %d, new sizes: %d and %d\n", 
        old_size, required_size, get_block_size(mmap_alloc), 
        get_block_size(new_mmap_alloc));
    #endif
    return new_mmap_free;
}

/*
    memmap_free_t* coalesce_block(memmap_free_t* mmap)
    
    A top-level function for coalescing blocks (with checks).
    Uses `merge_block` internally.

*/
memmap_free_t* coalesce_block(memmap_free_t* mmap){
    memmap_t* mmap_alloc = (memmap_t*)mmap;
    memmap_t* mmap_left = get_prev_block(mmap_alloc);
    memmap_t* mmap_right = get_next_block(mmap_alloc);

    if(!is_first_in_memory(mmap_alloc) && !get_allocated(mmap_left)) {
        #ifdef DEBUG_MEMORY
        printf("(Left) Merging %d and %d, ", get_block_size(mmap_left), 
            get_block_size(mmap_alloc));
        #endif
        mmap = merge_block((memmap_free_t*)mmap_left, mmap);
        #ifdef DEBUG_MEMORY
        printf("final size: %d\n", get_block_size((memmap_t*)mmap));
        #endif
    }

    if(!is_last_in_memory(mmap_alloc) && !get_allocated(mmap_right)) { 
        #ifdef DEBUG_MEMORY
        printf("(Right) Merging %d and %d, ", get_block_size(mmap_alloc), 
            get_block_size(mmap_right));
        #endif
        mmap = merge_block(mmap, (memmap_free_t*)mmap_right);
        #ifdef DEBUG_MEMORY
        printf("final size: %d\n", get_block_size((memmap_t*)mmap));
        #endif
    }
    return mmap;
}

/*
    memmap_free_t* merge_block(memmap_free_t* mmap_left, memmap_free_t* mmap_right)

    Merges two contiguous block of free memory.

*/
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
    printf("Left: %d, right: %d ,", get_block_size(mmap_left_alloc), 
        get_block_size(mmap_right_alloc));
    #endif 

    remove_free_block(mmap_right);
    remove_free_block(mmap_left);
    set_block_size(mmap_left_alloc, get_block_size(mmap_left_alloc) + 
        get_block_size(mmap_right_alloc));

    #ifdef DEBUG_MEMORY
    printf("new left:  %d\n", get_block_size(mmap_left_alloc));
    #endif
    return mmap_left;
}

/*
    __inline void remove_free_block(memmap_free_t* mmap)
    
    Removes a block from the free memory pool.

*/

__inline void remove_free_block(memmap_free_t* mmap){
    memmap_t* mmap_alloc = (memmap_t*)mmap;
    U8 index = get_free_bucket_index(get_block_size(mmap_alloc));
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

/*
    __inline void remove_free_block(memmap_free_t* mmap)
    
    Inserts a block into the free memory pool.

*/

__inline void insert_free_block(memmap_free_t* mmap){
    memmap_t* mmap_alloc = (memmap_t*)mmap;
    U32 block_size = get_block_size(mmap_alloc);
    U8 index = get_free_bucket_index(block_size);

    if (mprgmmap[index] == mmap){
        #ifdef DEBUG_MEMORY
        printf(" \
            WARN: The passed block is the same as the \
            first block in the bucket. This block might have \
            been previously inserted \
            ");
        #endif
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


/*******************************************************************************
                                
                            PUBLIC INTERFACE
                            
*******************************************************************************/

/*
    void* half_alloc(U32 requested_block_size)
    
    The main public method for memory allocation.

*/

void* half_alloc(U32 requested_block_size){
    U16 required_memory = CEIL32(requested_block_size + HEADER_SIZE);
    U8 i;
    memmap_free_t* selected_block_free;
    memmap_t* selected_block_alloc;
    memmap_free_t* additional_block;

    if (required_memory > MAX_MEMORY) return NULL;

    // from which bucket to allocate?
    i = get_alloc_bucket_index(required_memory);
    #ifdef DEBUG_MEMORY
    printf("half_alloc | Starting search at bucket %d ... ", i);
    #endif
    while (i < NUM_BUCKETS && mprgmmap[i] == NULL) i++;

    // no appropriate bucket found
    if (i >= NUM_BUCKETS || mprgmmap[i] == NULL) return NULL;

    #ifdef DEBUG_MEMORY
    printf(" allocating from bucket %d\n",i );
    #endif

    selected_block_free = mprgmmap[i];
    selected_block_alloc = (memmap_t*) selected_block_free;

    #ifdef DEBUG_MEMORY
    printf("half_alloc | Selected block size: %d\n", 
        get_block_size(selected_block_alloc));
    #endif
    
    remove_free_block(selected_block_free);
    
    //split the block if it's larger than requested by at least 32 bytes
    if (get_block_size(selected_block_alloc) - 
            required_memory > BLOCK_SIZE_MULTIPLE){

        additional_block = split_block(selected_block_free, required_memory);

        #ifdef DEBUG_MEMORY
        printf("half_alloc_2 | ");
        #endif

        insert_free_block(additional_block);
    }

    set_allocated(selected_block_alloc, __TRUE);
    return (void*)(((unsigned char*)selected_block_alloc) + HEADER_SIZE);
}

/*
    void half_free(void* ptr)
    
    The main public method for memory deallocation.

*/

void half_free(void* ptr){
    memmap_free_t* block_free = (memmap_free_t*) ((unsigned char*)ptr - 
        HEADER_SIZE);
    memmap_t* block_alloc = (memmap_t*) block_free;
    if (!get_allocated(block_alloc)) return;
    block_free = coalesce_block(block_free);
    #ifdef DEBUG_MEMORY
    printf("half_free | ");
    #endif
    insert_free_block(block_free);
    set_allocated(block_alloc, __FALSE);
}
