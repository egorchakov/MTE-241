#ifndef _PERIPHERALS_H
#define _PERIPHERALS_H

#include <LPC17xx.h>

void INT0_init(void);
void EINT3_IRQHandler(void);

void ADC_init(void);
void ADC_convert(void);
unsigned short int ADC_value(void);
void ADC_IRQHandler(void);

void LED_init(void);
void LED_turn_on( unsigned char led );
void LED_turn_off( unsigned char led ) ;
void update_LEDs(unsigned int value);
#endif
