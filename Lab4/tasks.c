#include <rtl.h>
#include "tasks.h"

#define HIGHEST 254
#define LOWEST 1
#define INIT_COLOR Green

__task void init_task(void){
    os_tsk_prio_self(HIGHEST);
    GLCD_Init();
    GLCD_Clear(INIT_COLOR);
    os_tsk_prio_self(LOWEST);
}