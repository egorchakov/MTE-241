#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "type.h"
#include "memhalf.h"


void print_bucket(memmap_free_t* head){
    memmap_free_t* tmp = head;
    while (tmp){
        printf("[%d] --> ", get_block_size((memmap_t*) tmp->memmap));
        tmp = tmp->next_free;
    }
    printf("\n");
}

void print_mem_layout(memmap_free_t* buckets[], short size){
    short i;
    printf("==================== START MEMORY LAYOUT ====================\n");
    for (i=0; i<size; i++) {
        printf("%d : ",i);
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