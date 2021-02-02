#include "demonstration_tasks.h"

#include "os.h"
#include "mutex.h"
#include "sleep.h"
#include "debugTools.h"

#ifdef DEMO_MUX

static OS_TCB_t _tcb1, _tcb2, _tcb3;
static OS_mutex_t _mux;

static void DemoMuxTask1(void const* const args)
{
    OS_MutexAquire(&_mux);
    LOG(LOG_LVL_MSG, "Task 1 owns mutex!\n");
    LOG(LOG_LVL_MSG, "Task 1 going to sleep..\n");
    OS_Sleep(6000);
    LOG(LOG_LVL_MSG, "Task 1 woken from sleep!\n");
    OS_MutexRelease(&_mux);
}

static void DemoMuxTask2(void const* const args)
{
    OS_Sleep(4000);
    OS_MutexAquire(&_mux);
    LOG(LOG_LVL_MSG, "Task 2 owns mutex!\n");
    OS_MutexRelease(&_mux);
}

static void DemoMuxTask3(void const* const args)
{
    OS_Sleep(1000);
    OS_MutexAquire(&_mux);
    LOG(LOG_LVL_MSG, "Task 3 owns mutex!\n");
    OS_MutexRelease(&_mux);
}
#endif 

void DemoInitMuxTasks(void) {
#ifdef DEMO_MUX
    __align(8)
    static uint32_t stack1[64], stack2[64], stack3[64];
    
    OS_InitialiseTCB(&_tcb1, stack1 + 64, DemoMuxTask1, NULL);
    OS_InitialiseTCB(&_tcb2, stack2 + 64, DemoMuxTask2, NULL);
    OS_InitialiseTCB(&_tcb3, stack3 + 64, DemoMuxTask3, NULL);
    OS_AddTask(&_tcb1, OS_SCHEDULER_PRIORITY_LVL_1);
    OS_AddTask(&_tcb2, OS_SCHEDULER_PRIORITY_LVL_1);
    OS_AddTask(&_tcb3, OS_SCHEDULER_PRIORITY_LVL_2);
    OS_InitMutex(&_mux);
#endif 
}