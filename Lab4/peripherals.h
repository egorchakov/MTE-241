#ifndef _PERIPHERALS_H
#define _PERIPHERALS_H

#include <LPC17xx.h>

void INT0_init(void);
void EINT3_IRQHandler(void);

void ADC_init(void);
void ADC_convert(void);
unsigned short int ADC_value(void);
void ADC_IRQHandler(void);

#endif
