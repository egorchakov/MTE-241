#include <LPC17xx.h>
#include <RTL.h>
#include <stdio.h>
#include "GLCD.h"
#include "util.h"

// 320×240


__task void move_ball_task(void* args) {
	circle_t* circle = (circle_t*)circle;
	GLCD_Clear( White );

	while( 1 ) {
		GLCD_Clear( White );
	  //draw_rectangle(circle->x, circle->y, 30, 30, Green);
		circle->x += 5;
		circle->y += 5;
		if(circle->x > 320)
			circle->x = 20;
		if(circle->y > 240)
			circle->y = 20;
	}
}

void EINT3_IRQHandler ( void ) {
		// Read ADC Status clears the interrupt condition
		circle_t* circle = (circle_t*)malloc(sizeof(circle_t));
		circle->radius = 5;
		circle->x = 50;
		circle->y = 50;
	
	  os_tsk_create_ex(move_ball_task, 1, circle);
		LPC_GPIOINT->IO2IntClr |=  (1 << 10);
}

int main_old( void ) {
    SystemInit();
    SystemCoreClockUpdate();
		GLCD_Init();
		GLCD_Clear( Blue );
	
		// Initialize ISR for push button
		LPC_PINCON->PINSEL4    &= ~( 3 << 20 ); 
	  LPC_GPIO2->FIODIR      &= ~( 1 << 10 );
    LPC_GPIOINT->IO2IntEnF |=  ( 1 << 10 );
  	NVIC_EnableIRQ( EINT3_IRQn );
		LPC_GPIOINT->IO2IntClr |=  (1 << 10);
	
		// We will change the value of the seed for the test
    srand( 10 );

    os_sys_init( move_ball_task );
		while ( 1 ) {
			// Endless loop
    }
}
