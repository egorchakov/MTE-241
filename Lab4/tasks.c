#include <rtl.h>
#include "tasks.h"

OS_SEM add_random_object_sem;
OS_SEM redraw_objects_sem;
OS_SEM redraw_objects_done_sem;

__task void add_random_object(void){
    int i,j;
    object_t obj;
        
        while(1){
        os_sem_wait(&add_random_object_sem, FOREVER);
        // will possibly need a mutex for active_objects
        if (active_objects < MAX_OBJECTS){
            srand(os_time_get() % 100);
            obj.x = rand() % 300;
            obj.y = rand() % 300;
            obj.radius = 5 + rand() % 10;
            objects[active_objects++] = obj;
        }
    }
}

__task void main_task(void){
    int i,x;
    while(1){
        for (i=0; i<active_objects; i++){
            // just moving along the x axis
            x = objects[i].x;
            objects[i].x = (x < 320 ? x + 20 : 20);
        }
        os_sem_send(&redraw_objects_sem);
        os_sem_wait(&redraw_objects_done_sem, FOREVER);
    }
}

__task void redraw_objects(void){
    int i;
    object_t obj; 


    os_sem_wait(&redraw_objects_sem, FOREVER);
    
    GLCD_Clear(INIT_COLOR);
    for (i = 0; i< active_objects; i++){
        obj = objects[i];
        // assuming objects are rectangles for now
        draw_rectangle(obj.x, obj.y, obj.radius*2, obj.radius*2, Red);
    }

    os_sem_send(&redraw_objects_done_sem);
}

__task void init_task(void){
    object_t obj;
    os_tsk_prio_self(HIGHEST_PRIO);

    // create a dummy object
    obj.x = 20;
    obj.y = 20;
    obj.radius = 10;
    objects[0] = obj;
    active_objects++;   

    GLCD_Init();
    GLCD_Clear(INIT_COLOR);
        
    os_tsk_create(main_task, LOWEST_PRIO+1);
    os_tsk_create(add_random_object, LOWEST_PRIO+2);
    os_tsk_create(redraw_objects, LOWEST_PRIO+1);
    
    os_sem_init(&add_random_object_sem, 0);
    os_sem_init(&redraw_objects_sem, 0);
    os_sem_init(&redraw_objects_done_sem, 0);
    
    os_tsk_prio_self(LOWEST_PRIO);
    while(1);
}
