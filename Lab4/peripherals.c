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
        // this should work, but doesn't
        os_sem_send(&add_random_object_sem);
        
        // this part never gets called, and I don't know why
        GLCD_Clear(Red);    
    }
}
