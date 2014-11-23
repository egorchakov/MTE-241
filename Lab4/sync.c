#include "config.h"
#include "sync.h"
#define true __TRUE
#define false __FALSE

#define TURNSTILE_LOCKED true 
#define TURNSTILE_UNLOCKED false 

 
void turnstile_init( turnstile_t *ts, bool locked ) { 
 os_sem_init( &( ts->turnstile ), 0, locked ? 0 : 1 ); 
} 
 
void turnstile_unlock( turnstile_t *ts ) { 
 os_sem_send( &( ts->turnstile ) ); 
} 
 
void turnstile_lock( turnstile_t *ts ) { 
 os_sem_wait( &( ts->turnstile ), FOREVER); 
} 
 
void turnstile_pass( turnstile_t *ts ) { 
 os_sem_wait( &( ts->turnstile ), FOREVER); 
 os_sem_send( &( ts->turnstile ) ); 
}

void rendezvous_init( rendezvous_t *rv, size_t n ) { 
 rv->capacity = n; 
 rv->waiting = 0; 
 
 os_sem_init( &( rv->mutex ), 0, 1 ); 
 turnstile_init( &( rv->enter ), TURNSTILE_LOCKED ); 
 turnstile_init( &( rv->exit ), TURNSTILE_UNLOCKED ); 
} 
 
void rendezvous_wait( rendezvous_t *rv , OS_SEM* sem_on_ready) { 
	os_sem_wait( &( rv->mutex ) , FOREVER); { 
		++rv->waiting; 

		if ( rv->waiting == rv->capacity ) { 
	
			turnstile_lock( &( rv->exit ) ); 
			turnstile_unlock( &( rv->enter ) ); 
		} 
		} os_sem_send( &( rv->mutex ) ); 

		turnstile_pass( &( rv->enter ) ); 

		os_sem_wait( &( rv->mutex ) , FOREVER); { 
		--rv->waiting; 
	
		if ( rv->waiting == 0 ) { 
			turnstile_lock( &( rv->enter ) ); 
			os_sem_send(sem_on_ready);
			turnstile_unlock( &( rv->exit ) ); 
		} 
		} os_sem_send( &( rv->mutex ) ); 

	turnstile_pass( &( rv->exit ) ); 
} 
