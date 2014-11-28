#include "peripherals.h"

const unsigned char ledPosArray[8] = { 28, 29, 31, 2, 3, 4, 5, 6 };

/*
    void INT0_init( void ) 

    Source: Keil IO example code by Vajih Montaghami

*/
void INT0_init( void ) {
    LPC_SC->PCONP |= (1 << 12);
    // P2.10 is related to the INT0 or the push button.
    // P2.10 is selected for the GPIO 
    LPC_PINCON->PINSEL4 &= ~(3<<20); 

    // P2.10 is an input port
    LPC_GPIO2->FIODIR   &= ~(1<<10); 

    // P2.10 reads the falling edges to generate the IRQ
    // - falling edge of P2.10
    LPC_GPIOINT->IO2IntEnF |= (1 << 10);

    // IRQ is enabled in NVIC. The name is reserved and defined in `startup_LPC17xx.s'.
    // The name is used to implemet the interrupt handler above,
    NVIC_EnableIRQ( EINT3_IRQn );

}

/*
    void EINT3_IRQHandler(void)

    Source: Keil IO example code by Vajih Montaghami

*/
void EINT3_IRQHandler(void){
    // [SRC]: io_example project by the TA
    // Check whether the interrupt is called on the falling edge. GPIO Interrupt Status for Falling edge.
    if ( LPC_GPIOINT->IO2IntStatF && (0x01 << 10) ) {
        LPC_GPIOINT->IO2IntClr |= (1 << 10); // clear interrupt condition
        
        add_object_flag = __TRUE;
        
    }
}

/*
    void ADC_init( void ) 

    Source: Keil IO example code by Vajih Montaghami

*/
void ADC_init( void ) {


	// Enabled the Power controler in PCONP register. According the Table 46. the 12th bit is PCADC
	LPC_SC->PCONP |= (1 << 12);

	// Poti is connected to port P0.25. We have to put the port P0.25 into the AD0.2 moe for anlaoge to digital conterting.
	LPC_PINCON->PINSEL1 &= ~(0x3 << 18); // Remove all bits, Port P0.25 gets GPIO
	LPC_PINCON->PINSEL1 |=  (0x1 << 18); // Switch P0.25 to AD0.2

	// No pull-up no pull-down (function 10) on the AD0.2 pin.
	LPC_PINCON->PINMODE1 &= ~(0x3 << 18);
	LPC_PINCON->PINMODE1 |=  (0x1 << 18);

	// A/D Control Register (Section 29.5.1)
	LPC_ADC->ADCR = ( 1 <<  2)  |    // SEL=1        select channel 0~7 on AD0.2 
	                ( 4 <<  8)  |    // ADC clock is 25 MHz/5          
	                ( 0 << 16 ) |    // BURST = 0    no BURST, software controlled 
	                ( 0 << 24 ) |    // START = 0    A/D conversion stops */
	                ( 0 << 27 ) |    // EDGE = 0     CAP/MAT singal falling,trigger A/D conversion
	                ( 1 << 21);      // PDN = 1      normal operation, Enable ADC                

	// Enabling A/D Interrupt Enable Register for all channels (Section 29.5.3)
	LPC_ADC->ADINTEN = ( 1 <<  8);        

	// Registering the interrupt service for ADC
	NVIC_EnableIRQ( ADC_IRQn );                  
}

/*
    void ADC_convert (void) 

    Source: Keil IO example code by Vajih Montaghami

*/
void ADC_convert (void) {
	// Stop reading and converting the port channel AD0.2.
  LPC_ADC->ADCR &= ~( 7 << 24); 
	ADC_Done = 0;
	// Start reading and converting the analog input from P0.25, where Poti is connected
	//to the challen Ad0.2
  LPC_ADC->ADCR |=  ( 1 << 24) | (1 << 2);              /* start conversion              */
}

/*
    void ADC_IRQHandler( void ) 

    Source: Keil IO example code by Vajih Montaghami

*/
void ADC_IRQHandler( void ) {
	volatile unsigned int aDCStat;

	// Read ADC Status clears the interrupt
	aDCStat = LPC_ADC->ADSTAT;

	// Read the value and and witht a max value as 12-bit.
	ADC_Value = (LPC_ADC->ADGDR >> 4) & 0xFFF; 

	ADC_Done = 1;
}

/*
    void INT0_init( void ) 

    Source: Keil IO example code by Vajih Montaghami

*/
unsigned short ADC_value( void ) {

	// Busy wainting until the conversion is done
	while ( !ADC_Done ) {
		// Wait for IRQ handler
	}

	return ADC_Value;
}

/*
    void INT0_init( void ) 

    Source: Keil IO example code by Vajih Montaghami

*/
void LED_init( void ) {

	// LPC_SC is a general system-control register block, and PCONP referes
	// to Power CONtrol for Peripherals.
	//  - Power/clock control bit for IOCON, GPIO, and GPIO interrupts (Section 4.8.9)
	//    This can also be enabled from `system_LPC17xx.c'
	LPC_SC->PCONP     |= (1 << 15);            

	// The ports connected to p1.28, p1.29, and p1.31 are in mode 00 which
	// is functioning as GPIO (Section 8.5.5)
	LPC_PINCON->PINSEL3 &= ~(0xCF00);

	// The port connected to p2.2, p2.3, p2.4, p2.5, and p2.6 are in mode 00
	// which is functioning as GPIO (Section 8.5.5)
	LPC_PINCON->PINSEL4 &= (0xC00F);

	// LPC_GPIOx is the general control register for port x (Section 9.5)
	// FIODIR is Fast GPIO Port Direction control register. This register 
	// individually controls the direction of each port pin (Section 9.5)
	//
	// Set the LEDs connected to p1.28, p1.29, and p1.31 as output
	LPC_GPIO1->FIODIR |= 0xB0000000;           

	// Set the LEDs connected to p2.2, p2.3, p2.4, p2.5, and p2.6 as output port
	LPC_GPIO2->FIODIR |= 0x0000007C;           
}

/*
    void INT0_init( void ) 

    Source: Keil IO example code by Vajih Montaghami

*/
void LED_turn_on( unsigned char led ) {
	unsigned int mask = (1 << ledPosArray[led]);

	// The first two LEDs are connedted to the port 28, 29 and 30
	if ( led < 3 ) {
		// Fast Port Output Set register controls the state of output pins.
		// Writing 1s produces highs at the corresponding port pins. Writing 0s has no effect (Section 9.5)
		LPC_GPIO1->FIOSET |= mask;
	} else {
		LPC_GPIO2->FIOSET |= mask;
	}

}

/*
    void INT0_init( void ) 

    Source: Keil IO example code by Vajih Montaghami

*/
void LED_turn_off( unsigned char led ) {
	unsigned int mask = (1 << ledPosArray[led]);

	// The first two LEDs are connedted to the port 28, 29 and 30
	if ( led < 3 ) {
		// Fast Port Output Clear register controls the state of output pins. 
		// Writing 1s produces lows at the corresponding port pins (Section 9.5)
		LPC_GPIO1->FIOCLR |= mask;
	} else {
		LPC_GPIO2->FIOCLR |= mask;
	}
}

void update_LEDs(unsigned int value){
	int i;
	for (i=0; i<8; i++){
		if ((value >> i) & 1)
			LED_turn_on(7-i);
		else
			LED_turn_off(7-i);
	}
}