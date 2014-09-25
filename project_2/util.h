#ifndef __UTILMEM_H__
#define __UTILMEM_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "type.h"

#define CEIL32(n) (n + (((n) % 32 == 0) ? 0 : (32 - (n) % 32))) 

#endif