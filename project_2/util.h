#ifndef __UTILMEM_H__
#define __UTILMEM_H__

#define FIRST_BUCKET_POWER 5

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "type.h"

#define CEIL32(n) (((n-1) >> 5) +1) << 5
#define FLOOR32(n) n >> 5 << 5

__inline S16 floor_log2(U32 size){
    S16 i = 0;
    while (size >>= 1) i++;
    return i;
}

__inline S16 ceil_log2(U32 size){
    return ( size == 1 ) ? 0 : floor_log2(size - 1) + 1;
}

__inline S16 get_alloc_bucket_index(U32 size){
    S16 index = ceil_log2(size) - FIRST_BUCKET_POWER;
    return (index >=0 ) ? index : 0; 
}

__inline S16 get_free_bucket_index(U32 size){
    S16 index = floor_log2(size) - FIRST_BUCKET_POWER;
    return (index >=0) ? index : 0;
}

#endif
