#ifndef __MEMHALF_H__
#define __MEMHALF_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "type.h"

void *half_alloc( size_t n );
void half_free( void* );

#endif