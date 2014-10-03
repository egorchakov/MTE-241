#include "debug.h"

void print_bucket(memmap_free_t* head){
    memmap_free_t* tmp_free = head;
    memmap_t* tmp_alloc = NULL;
    while (tmp_free){
        tmp_alloc = (memmap_t*) tmp_free;
        printf("|%d|-> ", get_block_size(tmp_alloc));
        if (is_last_in_bucket(tmp_free)) break;
        tmp_free = get_next_free(tmp_free);
    }
    printf("\n");
}

void print_memory_layout(void* base_ptr, memmap_free_t* buckets[], short size){
    memmap_t* block = (memmap_t*) base_ptr;
    printf( GREEN "==================== START MEMORY LAYOUT ====================\n\n" RESET);
    while (block){
        if (get_allocated(block)) printf(BOLDGREEN "[%d]->" RESET, get_block_size(block));
        else printf(GREEN "|%d|->" RESET, get_block_size(block));
        if (is_last_in_memory(block)) break;
        block = get_next_block(block);
    }
    printf( GREEN "\n\n\n");

    short i;
    for (i=0; i<size; i++) {
        #ifdef PRINT_IGNORE_EMPTY_BUCKETS
        if (!buckets[i]) continue;
        #endif
        printf("%d (%d -- %d ): ", i, 1 << (i+5), (1 << (i+6)) -1);
        print_bucket(buckets[i]);
    }
    printf("\n===================== END MEMORY LAYOUT =====================\n\n" RESET);
}

// void main(void){
//     half_init();
//     void* dummy[100];
//     int i;
//     for (i=0; i<8; i++) {
//         dummy[i] = half_alloc(1000+32*i);
//     }
//     printf("after alloc ");
//     print_memory_layout(base_ptr, mprgmmap, 10);
//     int free_order[8] = {0,2, 4, 3, 1, 5, 6, 7};

//     for (i=0; i<8; i++) {
//         printf("Freeing %d \n", get_block_size(dummy[free_order[i]] - HEADER_SIZE));
//         half_free(dummy[free_order[i]]);
//         print_memory_layout(base_ptr, mprgmmap, 10);   
//     }
    
// } 