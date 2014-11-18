#include "animation.h"
#include "tasks.h"
#include "peripherals.h"

void main(void){
    SystemInit();
    SystemCoreClockUpdate();
    INT0_init();
    os_sys_init(init_task);
}