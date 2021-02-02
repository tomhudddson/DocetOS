#ifndef ITC_QUEUE_H
#define ITC_QUEUE_H

#define ITC_MAX_MSGS 10

#include <stdint.h>

#include "task.h"
#include "mutex.h"
#include "semaphore.h"

/**
* @brief This structure contains one message that can be sent between tasks.
*   There is no need to initialise this yourself as the OS_ITCSendMsg does it 
*   for you.
*/
typedef struct s_itcMsg
{
    // This field stores the data the sending task want the receiving task
    // to get.
    void* data;
    
    // The size of the data field in bytes.
    size_t dataSz;
    
    // Pointer to the receiving tcb, i.e. the destination.
    OS_TCB_t* dest;
} OS_itcMsg_t;

/**
* @brief This structure contains a single Inter-Task Communication (ITC) message
*   queue. When messages are sent to the queue, they are stored in the first 
*   available location in msgbuf. Therefore, the order in which messages are 
*   read cannot be guarenteed.
*/
typedef struct s_itcQueue
{
    // This is the actual message queue. It stores the messages.
    OS_itcMsg_t  msgbuf[ITC_MAX_MSGS];
    
    // Mutex lock to prevent simultaneous access which may corrupt the queue.
    OS_mutex_t   mux;
    
    // Counting semaphore to keep track of how many messages are in the queue.
    OS_sem_t     sem;
} OS_itcQueue_t;

/**
* @brief This function initialises a message queue. Note, this function must 
*   be called before using the queue.
* @param queue The queue to initialise.
*/
void OS_InitITCQueue(OS_itcQueue_t* const queue);

/**
* @brief This function sends a message to a message queue. If the message queue
*   is full at the time of sending, then the calling task will be made to wait
*   until a different message leaves the qeueue. At which point, the calling 
*   task will be woken and can attempt to send the message again.
* @param queue Pointer to the message queue to send a message to.
* @param data The item of data the sending task wishes the receiving task to 
*   recieve. Note, the data is copied into an OS_itcMsg_t structure so this 
*   parameter can be on a local stack.
* @param dataSz The size in bytes of the data.
* @param dest Pointer to the destination tcb, i.e. the task the message is 
*   intended for.
*/
void OS_ITCSendMsg(OS_itcQueue_t* const queue, 
                     const void* const data,
                     const size_t dataSz,
                     OS_TCB_t* const dest);
      
/**
* @brief This function allows the calling task to read a message from a 
*   message queue. It will find the first message in the queue meant for the 
*   calling task, and copy the message's data field into the data parameter. 
*   To check for multiple messages for one task, OS_ITCHasMsg should be used
*   in conjuction with a while loop. If the message queue is full at the time of
*   reading, then the calling tasks will be made to wait until a message enters 
*   the queue. At which point the calling task will be woken and read from the 
*   queue again.   
* @param queue Pointer to the message queue to read a message from.                 
* @param data Pointer to a variable which the data from the message will be 
*   copied to.                     
*/                     
void OS_ITCReadMsg(OS_itcQueue_t* const queue, void** data);      

/**
* @brief This function determines whether there is a message in a message queue
*   for the calling task.
* @param queue Pointer to the queue to check.
* @return 1 if there is a message for the calling task.
* @return 0 if there is not a message for the calling task.
*/                     
uint32_t OS_ITCHasMsg(OS_itcQueue_t* const queue);  

/**
* @brief Print the contents of the queue.
* @param queue Pointer to the queue to print.
*/
void OS_ITCPrintQueue(OS_itcQueue_t* const queue);                     

#endif  // ITC_QUEUE_H

                     