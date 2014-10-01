#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "type.h"
#include "memhalf.h"


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

void print_mem_layout(memmap_free_t* buckets[], short size){
    short i;
    printf("==================== START MEMORY LAYOUT ====================\n");
    for (i=0; i<size; i++) {
        printf("%d (%d -- %d ): ", i, 1 << i+5, (1 << i+6) -1);
        print_bucket(buckets[i]);
    }
    printf("===================== END MEMORY LAYOUT =====================\n");
}
void main(void){
    printf("===================== MAIN =====================\n");
    memmap_free_t* free_block = (memmap_free_t*) malloc(32768);
    
    memmap_t* allocated_block = (memmap_t*) free_block;

    allocated_block->prev_block = 666;
    allocated_block->next_block = 777;
    set_block_size(allocated_block, 100);
    allocated_block->alloc = false;
    
    // printf("prev_block: %u\n", allocated_block->prev_block);
    // printf("next_block: %u\n", allocated_block->next_block);
    printf("block_size: %u\n", get_block_size(allocated_block));
    free_block->memmap = allocated_block;

    printf("block_size: %u\n", get_block_size(allocated_block));
    // printf("block_size: %u\n", get_block_size((memmap_t*) free_block->memmap));

}