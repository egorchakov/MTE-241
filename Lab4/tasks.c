#include "tasks.h"

object_t objects[MAX_OBJECTS];
OS_TID object_tasks[MAX_OBJECTS];
volatile int active_objects = 0;

OS_SEM add_random_object_sem;
OS_SEM redraw_objects_sem;
OS_SEM redraw_objects_done_sem;
OS_MUT GLCD_access;

volatile unsigned short int ADC_Value;
volatile unsigned char ADC_Done = 0; 
volatile U32 last_added_time = 0;

volatile BOOL add_object_flag = __FALSE;
volatile BOOL needs_update = __FALSE;

unsigned char malloc_failed[13] = "MALLOC FAILED";

/*
	__task void add_random_object(void)

	Creates an object with random parameters(color, speed, radius).
	Triggered with a push button interrupt. 
*/
__task void add_random_object(void){
	object_t* obj_ptr;
	while(1){

		// Wait until the corresponding flag is set
		while (!add_object_flag){
			os_tsk_pass();	
		}

		// Check if number of objects exceeds the maximum and if the last
		// object addition happened sufficiently long ago (to account for 
		// possible push button jitter)

		if ( 	active_objects < MAX_OBJECTS &&
				os_time_get() - last_added_time > MIN_ADD_DELAY
		){
			obj_ptr = &(objects[active_objects]);
			// Assign random coordinates, radius, speed, color
			obj_ptr->x = rand() % X_MAX;
			obj_ptr->y = rand() % Y_MAX;
			obj_ptr->radius = MIN_RADIUS + rand() % (MAX_RADIUS - MIN_RADIUS);
			obj_ptr->dx = MIN_DX + rand() % (MAX_DX - MIN_DX);
			obj_ptr->dy = MIN_DY + rand() % (MAX_DY - MIN_DY);
			obj_ptr->color = colors[rand() % NUM_RANDOM_COLORS];
			obj_ptr->pBitmap = ((unsigned short* ) malloc(
				sizeof(unsigned short) * obj_ptr->radius * obj_ptr->radius * 4));
			
			// If memory cannot be allocated for the object's bitmap, fail
			if (!obj_ptr->pBitmap){
				GLCD_Clear(BG_COLOR);
				GLCD_DisplayString(3, 0, 1, malloc_failed);
			}
			
			// Update object addition timestamp and the object count LEDs 
			last_added_time = os_time_get();
	        update_LEDs(active_objects + 1);

	        // Finally, create a task for the newly created object;
			os_tsk_create_ex(object_task, LOWEST_PRIO+1, &(objects[active_objects++]));

		}
		
		// Reset the flag
		add_object_flag = __FALSE;
	}
}

/*
	__task void object_task(void* args)
	
	Responsible for updating its object's direction and redrawing.
	
*/
__task void object_task(void* args){
	object_t* object = (object_t*) args; 

	while(1){			

		// First, erase object's current position
		fill_circle(object, object->pBitmap, BG_COLOR);

		// The following operations are perfomed while holding a GLCD mutex
		os_mut_wait(&GLCD_access, FOREVER);{
			// Draw the "erased" circle
			draw_circle(object, object->pBitmap);

			// Update object's direction
			object->x += object->dx;
			object->y += object->dy;

			// Detect if a wall collision occured and reverse object's 
			// direction if so
			if((object->x + object->radius) > X_MAX || (object->x - object->radius) < X_MIN)
				object->dx *= -1;
			
			if(object->y + object->radius > Y_MAX || object->y - object->radius < Y_MIN)
				object->dy *= -1;
			
			// Redraw the circle with a new position
			fill_circle(object, object->pBitmap, object->color);
			draw_circle(object, object->pBitmap);
			needs_update = __TRUE;

		} os_mut_release(&GLCD_access);

		// Wait for however long the potentiometer says to wait
		os_dly_wait(get_delay_interval(ADC_value()));
	}
}
/*
	__task void readPoti_task(void)

	Source: Keil IO example code by Vajih Montaghami
*/
__task void readPoti_task(void){	
	while( 1 ){
		ADC_convert();
		os_dly_wait( 100 );
	}
}

/*
	__task void init_task(void)

	Initializes peripherals and semaphores. Also responsible for detecting 
	collisions.
	
*/
__task void init_task(void){
	int i, j;
	os_tsk_prio_self(HIGHEST_PRIO);

	INT0_init();
    ADC_init();
    LED_init(); 

	GLCD_Init();
	GLCD_Clear(BG_COLOR);

	os_sem_init(&add_random_object_sem, 0);
	os_mut_init(&GLCD_access);
	    
	os_tsk_create(add_random_object, LOWEST_PRIO+1);
	os_tsk_create(readPoti_task, LOWEST_PRIO+1);
		
	os_tsk_prio_self(LOWEST_PRIO + 1);
	while(1) {
		if(!needs_update) {
			os_tsk_pass();	
		} else {
			os_mut_wait(&GLCD_access, FOREVER); {
					for(i = 0; i < active_objects; ++i) { 
						for(j = i + 1; j < active_objects; ++j) {
							if(check_intersect(objects[i].x, objects[i].y, objects[i].radius, objects[j].x, objects[j].y, objects[j].radius)) {
								calculate_velocities(&objects[i], &objects[j]);
							}
						}
					}
					
					needs_update = __FALSE;
			} os_mut_release(&GLCD_access); 
		}
	}
}
