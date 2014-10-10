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

S16 floor_log2(U32 );
S16 ceil_log2(U32 );

S16 get_alloc_bucket_index(U32 );
S16 get_free_bucket_index(U32 );

#endif