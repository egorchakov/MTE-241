#include "util.h"

unsigned int get_delay_interval(unsigned short ADC_value){
	return (int) MIN_DELAY + ADC_value * (MAX_DELAY - MIN_DELAY)/(MAX_ADC_VALUE - MIN_ADC_VALUE);
}

