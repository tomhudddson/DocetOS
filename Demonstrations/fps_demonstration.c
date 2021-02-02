#include "demonstration_tasks.h"

#include "task.h"
#include "mutex.h"
#include "debugTools.h"
#include "os.h"
#include "sleep.h"
#include "os_internal.h"
#include "tcb_priority_queue.h"

#ifdef DEMO_FPS_BASIC
static OS_TCB_t _fpsTcb1, _fpsTcb2, _fpsTcb3, _fpsTcb4, _fpsTcb5, _fpsTcb6;
static OS_mutex_t _demoFpsMux;

static void DemoFPSTask1(void const* const args) 
{
    LOG_OUTPUT_RAW("---------------\n");
    for (uint32_t i = 0; i < 5; i++) 
    {
        OS_MutexAquire(&_demoFpsMux);
        LOG_OUTPUT("Task 1\n");
        OS_MutexRelease(&_demoFpsMux);
    }
}

static void DemoFPSTask2(void const* const args) 
{
    LOG_OUTPUT_RAW("---------------\n");
    for (uint32_t i = 0; i < 5; i++) 
    {
        OS_MutexAquire(&_demoFpsMux);
        LOG_OUTPUT("Task 2\n");
        OS_MutexRelease(&_demoFpsMux);
    }
}

static void DemoFPSTask3(void const* const args) 
{
    LOG_OUTPUT_RAW("---------------\n");
    for (uint32_t i = 0; i < 5; i++) 
    {
        OS_MutexAquire(&_demoFpsMux);
        LOG_OUTPUT("Task 3\n");
        OS_MutexRelease(&_demoFpsMux);
    }
}

static void DemoFPSTask4(void const* const args) 
{
    LOG_OUTPUT_RAW("---------------\n");
    for (uint32_t i = 0; i < 5; i++) 
    {
        OS_MutexAquire(&_demoFpsMux);
        LOG_OUTPUT("Task 4\n");
        OS_MutexRelease(&_demoFpsMux);
    }
}

static void DemoFPSTask5(void const* const args) 
{
    LOG_OUTPUT_RAW("---------------\n");
    for (uint32_t i = 0; i < 5; i++) 
    {
        OS_MutexAquire(&_demoFpsMux);
        LOG_OUTPUT("Task 5\n");
        OS_MutexRelease(&_demoFpsMux);
    }
}

static void DemoFPSTask6(void const* const args) 
{
    LOG_OUTPUT_RAW("---------------\n");
    for (uint32_t i = 0; i < 5; i++) 
    {
        OS_MutexAquire(&_demoFpsMux);
        LOG_OUTPUT("Task 6\n");
        OS_MutexRelease(&_demoFpsMux);
    }
}
#endif

#ifdef DEMO_FPS_SLEEP
static OS_TCB_t _fpsTcb1, _fpsTcb2, _fpsTcb3;
static OS_mutex_t _fpsMux1;

static void DemoFPSSleepTask1(void const* const args)
{
    LOG(LOG_LVL_MSG, "Task 1 going to sleep..\n");
    OS_Sleep(10000);
    LOG(LOG_LVL_MSG, "Task 1 woken!\n");
}

static void DemoFPSSleepTask2(void const* const args)
{
    LOG(LOG_LVL_MSG, "Task 2 going to sleep..\n");
    OS_Sleep(5000);
    LOG(LOG_LVL_MSG, "Task 2 woken!\n");
}

static void DemoFPSSleepTask3(void const* const args) 
{
    LOG(LOG_LVL_MSG, "Task 3 going to sleep..\n");
    OS_Sleep(1000);
    LOG(LOG_LVL_MSG, "Task 3 woken!\n");
}

#endif 

#ifdef DEMO_FPS_WAIT
static OS_TCB_t _fpsTcb1, _fpsTcb2, _fpsTcb3;
static OS_tcbPriorityQueue_t _fpsPriorityQueue1;
static OS_TCB_t* _fpsPriorityQueueTasks[3];

static void DemoFPSWaitTask1(void const* const args)
{
    LOG(LOG_LVL_MSG, "Task 1 going to wait..\n");
    OS_Wait(&_fpsPriorityQueue1, OS_GetCheckCode());
    LOG(LOG_LVL_MSG, "Task 1 notified!\n");
}

static void DemoFPSWaitTask2(void const* const args)
{
    LOG(LOG_LVL_MSG, "Task 2 going to wait..\n");
    OS_Wait(&_fpsPriorityQueue1, OS_GetCheckCode());
    LOG(LOG_LVL_MSG, "Task 2 notifed!\n");
}

static void DemoFPSWaitTask3(void const* const args)
{
    LOG(LOG_LVL_MSG, "Task 3 going to sleep..\n");
    OS_Sleep(5000);
    LOG(LOG_LVL_MSG, "Notifying Task 1..\n");
    OS_Notify(&_fpsPriorityQueue1);
    LOG(LOG_LVL_MSG, "Task 3 going to sleep again..\n");
    OS_Sleep(5000);
    LOG(LOG_LVL_MSG, "Task 3 notifying Task 2..\n");
    OS_Notify(&_fpsPriorityQueue1);
}
#endif

void DemoInitFPSTasks()
{    
#ifdef DEMO_FPS_BASIC   
    __align(8)
    static uint32_t stack1[64], stack2[64], stack3[64], stack4[64], stack5[64], stack6[64];
    
    OS_InitialiseTCB(&_fpsTcb1, stack1 + 64, DemoFPSTask1, NULL);
    OS_InitialiseTCB(&_fpsTcb2, stack2 + 64, DemoFPSTask2, NULL);
    OS_InitialiseTCB(&_fpsTcb3, stack3 + 64, DemoFPSTask3, NULL);
    OS_InitialiseTCB(&_fpsTcb4, stack4 + 64, DemoFPSTask4, NULL);
    OS_InitialiseTCB(&_fpsTcb5, stack5 + 64, DemoFPSTask5, NULL);
    OS_InitialiseTCB(&_fpsTcb6, stack6 + 64, DemoFPSTask6, NULL);
    OS_InitMutex(&_demoFpsMux);
    
    OS_AddTask(&_fpsTcb1, OS_SCHEDULER_PRIORITY_LVL_1);
    OS_AddTask(&_fpsTcb2, OS_SCHEDULER_PRIORITY_LVL_3);
    OS_AddTask(&_fpsTcb3, OS_SCHEDULER_PRIORITY_LVL_2);
    OS_AddTask(&_fpsTcb4, OS_SCHEDULER_PRIORITY_LVL_5);
    OS_AddTask(&_fpsTcb5, OS_SCHEDULER_PRIORITY_LVL_4);
    OS_AddTask(&_fpsTcb6, OS_SCHEDULER_PRIORITY_LVL_1);
#endif

#ifdef DEMO_FPS_SLEEP   
    __align(8)
    static uint32_t stack1[64], stack2[64], stack3[64];
    
    OS_InitialiseTCB(&_fpsTcb1, stack1 + 64, DemoFPSSleepTask1, NULL);
    OS_InitialiseTCB(&_fpsTcb2, stack2 + 64, DemoFPSSleepTask2, NULL);
    OS_InitialiseTCB(&_fpsTcb3, stack3 + 64, DemoFPSSleepTask3, NULL);
    OS_AddTask(&_fpsTcb1, OS_SCHEDULER_PRIORITY_LVL_1);
    OS_AddTask(&_fpsTcb2, OS_SCHEDULER_PRIORITY_LVL_1);
    OS_AddTask(&_fpsTcb3, OS_SCHEDULER_PRIORITY_LVL_1);
    OS_InitMutex(&_fpsMux1);
#endif

#ifdef DEMO_FPS_WAIT    
    __align(8)
    static uint32_t stack1[64], stack2[64], stack3[64];
    
    OS_InitialiseTCB(&_fpsTcb1, stack1 + 64, DemoFPSWaitTask1, NULL);
    OS_InitialiseTCB(&_fpsTcb2, stack2 + 64, DemoFPSWaitTask2, NULL);
    OS_InitialiseTCB(&_fpsTcb3, stack3 + 64, DemoFPSWaitTask3, NULL);
    OS_AddTask(&_fpsTcb1, OS_SCHEDULER_PRIORITY_LVL_1);
    OS_AddTask(&_fpsTcb2, OS_SCHEDULER_PRIORITY_LVL_1);
    OS_AddTask(&_fpsTcb3, OS_SCHEDULER_PRIORITY_LVL_1);
    OS_InitTCBPriorityQueue(&_fpsPriorityQueue1, _fpsPriorityQueueTasks, 3, TCBPQ_ORDER_BY_PRIORITY);
#endif
}
