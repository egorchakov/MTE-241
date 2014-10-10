#include "util.h"

S16 floor_log2(U32 size){
    S16 i = 0;
    while (size >>= 1) i++;
    return i;
}

S16 ceil_log2(U32 size){
    return ( size == 1 ) ? 0 : floor_log2(size - 1) + 1;
}

S16 get_alloc_bucket_index(U32 size){
    S16 index = ceil_log2(size) - FIRST_BUCKET_POWER;
    return (index >=0 ) ? index : 0; 
}

S16 get_free_bucket_index(U32 size){
    S16 index = floor_log2(size) - FIRST_BUCKET_POWER;
    return (index >=0) ? index : 0;
}