#include "demonstration_tasks.h"

#include "task.h"
#include "itc_queue.h"
#include "debugTools.h"
#include "os.h"
#include "os_internal.h"

#ifdef DEMO_QUEUE_READ_SINGLE_MSG
static OS_TCB_t _demoQueueSingleReadTcb1, _demoQueueSingleReadTcb2;
OS_itcQueue_t _demoQueue3;

static void DemoITCQueueSingleReadSenderTask(void const* const args) 
{
    char* msg = "msg";
    
    LOG(LOG_LVL_MSG, "Sending msg..\n");
    OS_ITCSendMsg(&_demoQueue3, msg, sizeof(msg), &_demoQueueSingleReadTcb2);
}

static void DemoITCQueueSingleReadReceiverTask(void const* const args)
{
    void* data;
    
    LOG(LOG_LVL_MSG, "Receiving message..\n");
    OS_ITCReadMsg(&_demoQueue3, &data);
    LOG(LOG_LVL_MSG, "Message recieved!\n");
    LOG(LOG_LVL_OUTPUT, "%s\n", (char* )data);
}

#endif

#ifdef DEMO_QUEUE_FULL
static OS_TCB_t _demoQueueFullTcb1, _demoQueueFullTcb2, _demoQueueFullTcb3;
static OS_itcQueue_t _demoQueueFullQueue;

static void* test;

static void DemoITCQueueFullTask(void const* const args) 
{
    LOG(LOG_LVL_MSG, "Sender task sending messages..\n");
            
    test = (void* )5;
    
    for (uint32_t i = 0; i < 5; i++)
    {
        uint32_t value = i + 10;
        OS_ITCSendMsg(&_demoQueueFullQueue, (void* )(i + 10), sizeof(uint32_t), &_demoQueueFullTcb2);
    }
    
    for (uint32_t j = 0; j < 15; j++)
    {       
        OS_ITCSendMsg(&_demoQueueFullQueue, (void* )j, sizeof(uint32_t), &_demoQueueFullTcb3);
    }
    
    LOG(LOG_LVL_MSG, "All messages sent!\n");
}

static void DemoITCQueueFullReceiverTask1(void const* const args) 
{
    LOG(LOG_LVL_TRACE, "Reciever Task 1 Start\n");
    void* data;
    
    while (OS_ITCHasMsg(&_demoQueueFullQueue))
    {
        //LOG(LOG_LVL_MSG, "Receiver Task 1 reading..\n");
        
        OS_ITCReadMsg(&_demoQueueFullQueue, &data);
        LOG_OUTPUT("Receiver Task 1: %d\n", (uint32_t)data);
    }
}

static void DemoITCQueueFullReceiverTask2(void const* const args) 
{
    LOG(LOG_LVL_TRACE, "Reciever Task 2 Start\n");
    void* data;
    
    while (OS_ITCHasMsg(&_demoQueueFullQueue))
    {
        OS_ITCReadMsg(&_demoQueueFullQueue, &data);
        LOG_OUTPUT("Receiver Task 2: %d\n", (uint32_t)data);
    }
}
#endif

#ifdef DEMO_QUEUE_EMPTY
static OS_TCB_t _itcTcb1, _itcTcb2;
static OS_itcQueue_t _demoQueue2;

static void DemoITCQueueEmptySenderTask(void const* const args)
{
    char* data = "test_msg";
    
    OS_Sleep(3000);
    LOG(LOG_LVL_MSG, "Sending msg..\n");
    OS_ITCSendMsg(&_demoQueue2, data, sizeof(data), &_itcTcb2);
}

static void DemoITCQueueEmptyReceiverTask(void const* const args) 
{
    void* data;
    LOG(LOG_LVL_MSG, "Reading message..\n");
    OS_ITCReadMsg(&_demoQueue2, &data);
    LOG(LOG_LVL_OUTPUT, "%s\n", (char* )data);
}
#endif



/* Queue demonstration initialisation. */
void DemoInitITCQueueTasks(void)
{
#ifdef DEMO_QUEUE_FULL
    // Queue full demo. This demo should output the letter A at the start of 
    // the task then successfully send two messages to the queue and output the 
    // letter B. Then it attempts to send another message but the queue will be 
    // full by then so the task will be put into the 'wait' state and hang. 
    // Therefore, "C" should not be outputted.
    __align(8)
    static uint32_t queueFullStack1[64], queueFullStack2[64], queueFullStack3[64];

    OS_InitITCQueue(&_demoQueueFullQueue);
    OS_InitialiseTCB(&_demoQueueFullTcb1, queueFullStack1 + 64, DemoITCQueueFullTask, NULL);
    OS_InitialiseTCB(&_demoQueueFullTcb2, queueFullStack2 + 64, DemoITCQueueFullReceiverTask1, NULL);
    OS_InitialiseTCB(&_demoQueueFullTcb3, queueFullStack3 + 64, DemoITCQueueFullReceiverTask2, NULL);
    OS_AddTask(&_demoQueueFullTcb1, OS_SCHEDULER_PRIORITY_LVL_1);
    OS_AddTask(&_demoQueueFullTcb2, OS_SCHEDULER_PRIORITY_LVL_2);
    OS_AddTask(&_demoQueueFullTcb3, OS_SCHEDULER_PRIORITY_LVL_3);
#endif
    
#ifdef DEMO_QUEUE_EMPTY
    // Queue empty demo. This demo attempts to read a message from an empty 
    // queue. The letter "B" should not be outputted as reading from the empty 
    // queue should put the task into the 'wait' state and stop its execution.
    __align(8)
    static uint32_t stack1[64], stack2[64];
    
    OS_InitITCQueue(&_demoQueue2);
    OS_InitialiseTCB(&_itcTcb1, stack1 + 64, DemoITCQueueEmptySenderTask, NULL);
    OS_InitialiseTCB(&_itcTcb2, stack2 + 64, DemoITCQueueEmptyReceiverTask, NULL);
    OS_AddTask(&_itcTcb1, OS_SCHEDULER_PRIORITY_LVL_2);
    OS_AddTask(&_itcTcb2, OS_SCHEDULER_PRIORITY_LVL_1);
#endif

#ifdef DEMO_QUEUE_READ_SINGLE_MSG
    // Single send and read between tasks demo. This demo will first send a 
    // message from the sender task as it is priority level 1. The second task
    // will then recieve the message and output it. Note, the fixed priority
    // scheduler must be used for this demo.   
    __align(8)
    static uint32_t queueSingleReadStack1[64], queueSingleReadStack2[64];
    
    OS_InitITCQueue(&_demoQueue3);
    OS_InitialiseTCB(&_demoQueueSingleReadTcb1, queueSingleReadStack1 + 64, DemoITCQueueSingleReadSenderTask, NULL);
    OS_InitialiseTCB(&_demoQueueSingleReadTcb2, queueSingleReadStack2 + 64, DemoITCQueueSingleReadReceiverTask, NULL);
    OS_AddTask(&_demoQueueSingleReadTcb1, OS_SCHEDULER_PRIORITY_LVL_1);
    OS_AddTask(&_demoQueueSingleReadTcb2, OS_SCHEDULER_PRIORITY_LVL_2);
#endif
}
