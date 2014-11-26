	#include "animation.h"
#include "tasks.h"
#include "peripherals.h"


object_t objects[MAX_OBJECTS];
const unsigned short colors[NUM_RANDOM_COLORS] = {Black, Purple, Olive, Blue, Cyan, Red, Magenta, Yellow, Green};
volatile int active_objects = 0;
volatile BOOL add_object_flag = __FALSE;

volatile unsigned short int ADC_Value;
volatile unsigned char ADC_Done = 0; 

OS_TID object_tasks[MAX_OBJECTS];
volatile unsigned int delay_interval;

int main(void){
    SystemInit();
    SystemCoreClockUpdate();
	printf("Foo\n");
	srand(234);
    INT0_init();
    ADC_init();
	LED_init();
	os_sys_init(init_task);
	while(1){
		// Endless loop
	}			
}
