#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "type.h"
#include "memhalf.h"

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

#define PRINT_IGNORE_EMPTY_BUCKETS

void print_bucket(memmap_free_t* head){
    memmap_free_t* tmp_free = head;
    memmap_t* tmp_alloc = NULL;
    while (tmp_free){
        tmp_alloc = (memmap_t*) tmp_free;
        printf("[%d] --> ", get_block_size(tmp_alloc));
        if (is_last_in_bucket(tmp_free)) break;
        tmp_free = get_next_free(tmp_free);
    }
    printf("\n");
}

void print_buckets(memmap_free_t* buckets[], short size){
    short i;
    printf(MAGENTA "==================== START BUCKETS LAYOUT ====================\n\n");
    for (i=0; i<size; i++) {
        #ifdef PRINT_IGNORE_EMPTY_BUCKETS
        if (!buckets[i]) continue;
        #endif
        printf("%d (%d -- %d ): ", i, 1 << i+5, (1 << i+6) -1);
        print_bucket(buckets[i]);
    }
    printf("\n===================== END BUCKETS LAYOUT =====================\n" RESET);
}

void print_memory_layout(void* base_ptr){
    memmap_t* block = (memmap_t*) base_ptr;
    printf( GREEN "==================== START MEMORY LAYOUT ====================\n\n");
    while (block){
        if (get_allocated(block)) printf("( %d )->", get_block_size(block));
        else printf("[ %d ]->", get_block_size(block));
        if (is_last_in_memory(block)) break;
        block = get_next_block(block);
    }
    printf("\n\n==================== END MEMORY LAYOUT ====================\n" RESET);
}
    
void main(void){
    half_init();
    void* dummy1 = half_alloc_2(1000);
    void* dummy2 = half_alloc_2(1000);
    void* dummy3 = half_alloc_2(1000);
    print_buckets(mprgmmap, 10);
    half_free(dummy1);
    print_buckets(mprgmmap, 10);
    half_free(dummy2);
    print_buckets(mprgmmap, 10);
    half_free(dummy3);
    print_buckets(mprgmmap, 10);
    printf("%p\n", base_ptr);
}