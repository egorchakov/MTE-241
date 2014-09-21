#include "memhalf.h"

void* get_prev_block( memmap_t const* mmap){
	return (void*)mmap->prev_block;
}

void* get_next_block( memmap_t const* mmap){
	return (void*)mmap->next_block;
}

size_t get_block_size( memmap_t const* mmap){
	return (size_t)mmap->block_size;
}

bool get_allocated( memmap_t const* mmap){
	return mmap->alloc;
}

void* get_prev_free( memmap_free_t const* mmap){
	return (void*)mmap->prev_free;
}

void* get_next_free( memmap_free_t const* mmap){
	return (void*)mmap->next_free;
}

int main( int argc, char *argv[] ){
	printf("%d\n", CEIL32(520));
	printf("%d\n", CEIL32(28));
}