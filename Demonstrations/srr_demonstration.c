#include "demonstration_tasks.h"

#include "task.h"
#include "mutex.h"
#include "debugTools.h"
#include "os.h"
#include "os_internal.h"

#ifdef DEMO_SRR
static OS_TCB_t _srrTcb1, _srrTcb2;
static OS_mutex_t _demoSrrMux;

static void DemoSRRTask1(void const* const args) 
{
    for (uint32_t i = 0; i < 100; i++)
    {
        OS_MutexAquire(&_demoSrrMux);
        LOG_OUTPUT_RAW("A");
        OS_MutexRelease(&_demoSrrMux);
    }
}

static void DemoSRRTask2(void const* const args) 
{
    for (uint32_t i = 0; i < 200; i++)
    {
        OS_MutexAquire(&_demoSrrMux);
        LOG_OUTPUT_RAW("B");
        OS_MutexRelease(&_demoSrrMux);
    }
}
#endif

void DemoInitSRRTasks()
{
#ifdef DEMO_SRR
    ASSERT(OS_GetSchedulerType() == OS_SCHEDULER_TYPE_SRR);
    
    __align(8)
    static uint32_t stack1[64], stack2[64];
    
    OS_InitialiseTCB(&_srrTcb1, stack1 + 64, DemoSRRTask1, NULL);
    OS_InitialiseTCB(&_srrTcb2, stack2 + 64, DemoSRRTask2, NULL);
    OS_InitMutex(&_demoSrrMux);
    
    OS_AddTask(&_srrTcb1, NULL);
    OS_AddTask(&_srrTcb2, NULL);
#endif
}