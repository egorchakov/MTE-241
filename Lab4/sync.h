#include <rtl.h>

typedef struct { 
	OS_SEM turnstile; 
} turnstile_t; 

typedef struct { 
	size_t capacity; 
	size_t waiting; 
	OS_SEM mutex; 
	turnstile_t enter; 
	turnstile_t exit; 
} rendezvous_t; 

void rendezvous_init( rendezvous_t *rv, size_t n );
void rendezvous_wait( rendezvous_t *rv );