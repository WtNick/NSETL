
#include <stdio.h>
//#include "system.h"
#include "tasklet.h"

static void nop(){}
#define CPU_IDLE nop
#define INTERRUPT 
void __disable_irq(){}
void __enable_irq(){}
void SYSTEM_INIT(){}
void gpio_pinlevel(int l, int a){}

void gpio_toggle_pin(int l)
{
	printf("%i ", l);
}

// local general purpose tasklet wait context
struct TLWaitContext waitonme;



INTERRUPT void isr_gpio(void)
{
	// release all task waiting for a button push
	tasklet_release(&waitonme);
}

TASKLET_DEFINE(blinkytask, void* arg)
TASKLET_BEGIN(blinkytask)
	while(1){
	    // wait until the button is pressed
		TASKLET_AWAIT(&waitonme);
		
		// button is pressed. flash the LED
		// the LED id is derived from the task argument
		gpio_pinlevel(self->arg, 1);
		TASKLET_SLEEP(1000);
		gpio_pinlevel(self->arg,0);
	}
TASKLET_END


// the heartbeat task create burst of LED pulses
// the amount of pulses is determined by the task argument at task startup
TASKLET_DEFINE(tl_heartbeat, void* arg)
	int period;
TASKLET_BEGIN(tl_heartbeat)
	while(1){
		self->period = (int)self->arg;
		while(self->period--){
			gpio_toggle_pin(self->arg);
			TASKLET_SLEEP(500);
		}
		TASKLET_SLEEP(3000);
}
TASKLET_END

TASKLET_INSTANCE(blinkytask) heartbeat1;
TASKLET_INSTANCE(blinkytask) heartbeat2;

int tasklet_main(void){
	SYSTEM_INIT(); // <- initalize board, system, etc... setup timers, irq handlers, etc...

	tasklet_init(&heartbeat1, &blinkytask);
	tasklet_init(&heartbeat2, &blinkytask);
	//tasklet_init(&buttontask1, &mytaskA);
	
	tasklet_start(&heartbeat1, (void*)1);
	tasklet_start(&heartbeat2, (void*)2);
	//tasklet_start(&buttontask1, NULL);
	
	while(1){
		if (tasklet_process()){ 
			CPU_IDLE();
		}
	}
	return 0;
}