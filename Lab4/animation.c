#include "animation.h"

const unsigned short colors[NUM_RANDOM_COLORS] = \
    {Black, Purple, Olive, Blue, Cyan, Red, Magenta, Yellow, Green};

int main(void){
    SystemInit();
    SystemCoreClockUpdate();

	srand(234);
    os_sys_init(init_task);
	
    while(1);
}
