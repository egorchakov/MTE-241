#include "animation.h"
#include "tasks.h"
#include "peripherals.h"


object_t objects[MAX_OBJECTS];
volatile int active_objects = 0;

int main(void){

    SystemInit();
    SystemCoreClockUpdate();
    INT0_init();
    os_sys_init(init_task);
}
