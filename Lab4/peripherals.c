#include "peripherals.h"
#include "tasks.h"

void INT0_init( void ) {
    // [SRC]: io_example project by the TA
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

void EINT3_IRQHandler(void){
    // [SRC]: io_example project by the TA
    // Check whether the interrupt is called on the falling edge. GPIO Interrupt Status for Falling edge.
    if ( LPC_GPIOINT->IO2IntStatF && (0x01 << 10) ) {
        LPC_GPIOINT->IO2IntClr |= (1 << 10); // clear interrupt condition
        
        add_object_flag = __TRUE;
        
    }
}

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

void ADC_convert (void) {
	// Stop reading and converting the port channel AD0.2.
  LPC_ADC->ADCR &= ~( 7 << 24); 
	ADC_Done = 0;
	// Start reading and converting the analog input from P0.25, where Poti is connected
	//to the challen Ad0.2
  LPC_ADC->ADCR |=  ( 1 << 24) | (1 << 2);              /* start conversion              */
}

void ADC_IRQHandler( void ) {
	volatile unsigned int aDCStat;

	// Read ADC Status clears the interrupt
	aDCStat = LPC_ADC->ADSTAT;

	// Read the value and and witht a max value as 12-bit.
	ADC_Value = (LPC_ADC->ADGDR >> 4) & 0xFFF; 

	ADC_Done = 1;
}

unsigned short ADC_value( void ) {

	// Busy wainting until the conversion is done
	while ( !ADC_Done ) {
		// Wait for IRQ handler
	}

	return ADC_Value;
}