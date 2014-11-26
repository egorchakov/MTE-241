#include <rtl.h>
#include "tasks.h"
#define M_PI 3.1415926

OS_SEM add_random_object_sem;
OS_SEM redraw_objects_sem;
OS_SEM redraw_objects_done_sem;
OS_MUT GLCD_access;
unsigned char malloc_failed[13] = "MALLOC FAILED";
volatile U32 last_added_time = 0;
volatile BOOL needs_update = __FALSE;

__task void add_random_object(void){
      while(1){
        
			while (!add_object_flag){
				os_tsk_pass();	
			}

			// will possibly need a mutex for active_objects
			if (active_objects < MAX_OBJECTS && os_time_get() - last_added_time > MIN_ADD_DELAY){
				objects[active_objects].x = rand() % X_MAX;
				objects[active_objects].y = rand() % Y_MAX;
				objects[active_objects].radius = 10 ;//MIN_RADIUS + rand() % (MAX_RADIUS - MIN_RADIUS);
				objects[active_objects].dx = DX;
				objects[active_objects].dy = DY;
				objects[active_objects].color = colors[rand() % NUM_RANDOM_COLORS];
				objects[active_objects].pBitmap = (unsigned short* ) malloc(sizeof(unsigned short) * objects[active_objects].radius * objects[active_objects].radius * 4);
				
				if (!objects[active_objects].pBitmap){
					GLCD_DisplayString(3, 0, 1, malloc_failed);
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
				needs_update = __TRUE;
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

// Return 1 if circles are intersecting, 0 if no intersecting
BOOL check_intersect(int x1, int y1, int r1, int x2, int y2, int r2) {
	int dist = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
	return dist >= (r1 - r2) * (r1 - r2) && dist <= (r1 + r2) * (r1 + r2);
}

void calculate_velocities(object_t* obj1, object_t* obj2) {
	double phi = atan2(obj1->y - obj2->y, obj1->x - obj2->x);
	double theta1 = atan2(obj1->dy, obj1->dx);
	double theta2 = atan2(obj2->dy, obj2->dx);	
	double v1 = sqrt(obj1->dy * obj1->dy + obj1->dx * obj1->dx);
	double v2 = sqrt(obj2->dy * obj2->dy + obj2->dx * obj2->dx);
	obj1->dx = v2 * cos(theta2 - phi) * cos(phi) + v1 * sin(theta1 - phi) * cos(phi + M_PI/2);
	obj1->dy = v2 * cos(theta2 - phi) * sin(phi) + v1 * sin(theta1 - phi) * sin(phi + M_PI/2);
	
	obj2->dx = v1 * cos(theta1 - phi) * cos(phi) + v2 * sin(theta2 - phi) * cos(phi + M_PI/2);
	obj2->dy = v1 * cos(theta1 - phi) * sin(phi) + v2 * sin(theta2 - phi) * sin(phi + M_PI/2);
}

__task void init_task(void){
	int i, j;
  os_tsk_prio_self(HIGHEST_PRIO);
	
	GLCD_Init();
  GLCD_Clear(INIT_COLOR);
	
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
