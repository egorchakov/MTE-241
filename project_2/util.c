#include "util.h"

S16 floor_log2(size_t size){
    S16 i = 0;
    while (size >>= 1) i++;
    return i;
}

S16 ceil_log2(size_t size){
    return ( size == 1 ) ? 0 : floor_log2(size - 1) + 1;
}

S16 get_alloc_bucket_index(size_t size){
    // 2^6 = 64 is the first bucket, therefore indices are offset by 6
    S16 index = ceil_log2(size) - 6;
    return (index >=0 ) ? index : 0; 
}

S16 get_free_bucket_index(size_t size){
    // 2^6 = 64 is the first bucket, therefore indices are offset by 6
    S16 index = floor_log2(size) - 6;
    return (index >=0) ? index : 0;
}