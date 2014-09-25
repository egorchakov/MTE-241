#ifndef __UTILMEM_H__
#define __UTILMEM_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "type.h"

#define CEIL32(n) ((n) + (32 - (n) % 32)) 

#endif

S16 floor_log2(size_t size);
S16 ceil_log2(size_t size);

S16 get_alloc_bucket_index(size_t size);
S16 get_free_bucket_index(size_t size);