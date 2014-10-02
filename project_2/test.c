#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "type.h"
#include "memhalf.h"

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#define PRINT_IGNORE_EMPTY_BUCKETS

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
    printf("\n===================== END BUCKETS LAYOUT =====================\n\n" RESET);
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
        printf("%d (%d -- %d ): ", i, 1 << i+5, (1 << i+6) -1);
        print_bucket(buckets[i]);
    }
    printf("\n===================== END MEMORY LAYOUT =====================\n\n" RESET);
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