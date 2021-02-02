#include <stdio.h>

#include "os.h"
#include "utils/config.h"
#include "debugTools.h"
#include "simpleRoundRobin.h"
#include "fixedPriorityScheduler.h"
#include "sleep.h"
#include "mutex.h"
#include "itc_queue.h"
#include "semaphore.h"
#include "memory.h"
#include "demonstration_tasks.h"

#define DEMONSTRATION 

int main(void) 
{
	// Set up core clock and initialise serial port. 
	config_init();
    
    LOG_OUTPUT_RAW("\n\n--------------------\n");
    LOG(LOG_LVL_MSG, "DocetOS initialising..\n");
    
    OS_InitFPS();
	OS_Init(&fixedPriorityScheduler);    
    
#ifdef DEMONSTRATION
    DemoInitMuxTasks();
    DemoInitFPSTasks();
    DemoInitITCQueueTasks();
    DemoInitMPTasks();
#endif
    
    LOG(LOG_LVL_MSG, "DocetOS starting..\n");
    LOG_OUTPUT_RAW("--------------------\n\n");
	
	OS_Start();
}
