#include <rtl.h>
#include "tasks.h"

OS_SEM add_random_object_sem;
OS_SEM redraw_objects_sem;
OS_SEM redraw_objects_done_sem;
OS_MUT GLCD_access;
unsigned char malloc_failed[13] = "MALLOC FAILED";
volatile U32 last_added_time = 0;


__task void add_random_object(void){
        while(1){
        
			while (!add_object_flag){
				os_tsk_pass();	
			}

			// will possibly need a mutex for active_objects
			if (active_objects < MAX_OBJECTS && os_time_get() - last_added_time > MIN_ADD_DELAY){
				objects[active_objects].x = rand() % X_MAX;
				objects[active_objects].y = rand() % Y_MAX;
				objects[active_objects].radius =10 ;//MIN_RADIUS + rand() % (MAX_RADIUS - MIN_RADIUS);
				objects[active_objects].dx = DX;
				objects[active_objects].dy = DY;
				objects[active_objects].color = colors[rand() % NUM_RANDOM_COLORS];
				objects[active_objects].pBitmap = (unsigned short* ) malloc(sizeof(unsigned short) * objects[active_objects].radius * objects[active_objects].radius * 4);
				
				if (!objects[active_objects].pBitmap){
					GLCD_DisplayString(3,0,1, malloc_failed);
				}
				
				//fill_circle(&(objects[active_objects]), bitmaps[active_objects], colors[rand() % NUM_RANDOM_COLORS]);
				
				last_added_time = os_time_get();
				update_LEDs(active_objects + 1);
				os_tsk_create_ex(object_task, LOWEST_PRIO+1, &(objects[active_objects++]));
				
				
			
			}
					
			add_object_flag = __FALSE;
    }
}

/*
__task void redraw_objects(void){
    int i;
		char active_objects_str[10];
		while(1) {
			sprintf(active_objects_str, "%d", active_objects);
			os_sem_wait(&render_signal, FOREVER);
			GLCD_Clear(INIT_COLOR);
			GLCD_DisplayString(0,0,1, active_objects_str);
			for (i = 0; i< active_objects; i++){
					// assuming objects are rectangles for now
					draw_circle(&(objects[i]), bitmaps[i]);
			}

		}
}
*/
__task void object_task(void* args){
		
		object_t* object = (object_t*) args; 
		while(1){
			
			fill_circle(object, object->pBitmap, INIT_COLOR);
			
			// TODO: replace with draw_rectangle
			os_mut_wait(&GLCD_access, FOREVER);{
				
				draw_circle(object, object->pBitmap);
			
			
				object->x += object->dx;
				object->y += object->dy;
				// wall collision detection (reversing direction)
				if((object->x + object->radius) > X_MAX || (object->x - object->radius) < X_MIN)
					object->dx *= -1;
				
				if(object->y + object->radius > Y_MAX || object->y - object->radius < Y_MIN)
					object->dy *= -1;
				
				fill_circle(object, object->pBitmap, object->color);
				
			
				draw_circle(object, object->pBitmap);
			} os_mut_release(&GLCD_access);
			os_dly_wait(get_delay_interval(ADC_value()));
			
			
		}
}

__task void readPoti_task(void){
	
	while( 1 ){
		ADC_convert();
		//Now wiat for the other threads.
		
		os_dly_wait( 100 );
	}
}

__task void init_task(void){
	int i, tmp;
	unsigned int mask;
    os_tsk_prio_self(HIGHEST_PRIO);
	
	GLCD_Init();
    GLCD_Clear(INIT_COLOR);
	
	os_sem_init(&add_random_object_sem, 0);
	os_mut_init(&GLCD_access);
        
    os_tsk_create(add_random_object, LOWEST_PRIO+1);
	os_tsk_create (readPoti_task, LOWEST_PRIO+1);
	
	os_tsk_prio_self(LOWEST_PRIO);
	while(1);
}
