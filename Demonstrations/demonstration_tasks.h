#ifndef DEMONSTRATION_TASKS_H
#define DEMONSTRATION_TASKS_H

/****************/
/* Introduction */
/****************/

/*
This file allows you to use the demonstration code. To view a demonstration,
uncomment one of the pre-processor definitions. Note, for the output of the 
demonstrations to be clear, only one demonstration should be uncommented at a
time. Once, uncommented, rebuild the project and the output should be on the 
terminal.

The code for the:
    - mutex demo                            mux_demonstration.c.
    - fixed-priority scheduler demo         fps_demonstration.c
    - inter-task communication demo         itc_demonstration.c
    - memory pool demo                      mp_demonstration.c
*/

/**************/
/* Mutex Demo */
/**************/

/* 
This demonstration shows how a mutex can be used. In this demo, there are three
tasks that all try to acquire the mutex. Note, Task 1 and Task 2 are both 
priority level 1, whereas Task 3 is priority level 2. 

When the demo is first run, Task 2 and Task 3 are put to sleep to ensure Task 1
is executed first and gains control of the mutex. Task 1 then sleeps to give 
time for Task 2 and 3 to wake. When they do wake, they attempt to acquire the 
mutex and are subsequently put into the wait state until Task 1 has released the 
mutex.

After Task 1 releases the mutex. Task 2 should gain control of the mutex before
Task 3, even though it wakes from sleep after Task 3. This is because it has
a priority level higher than Task 3. 

After Task 2 releases the mutex, Task 3 should then gain control.
*/
//#define DEMO_MUX

/*********************************/
/* Fixed-Priority Scheduler Demo */
/*********************************/

/*
This demonstration shows how the fixed-priority scheduler works. It contains 
six tasks, Task 1 to Task 6 and schedules them based on their priority. 

Task 1 and Task 6 have the highest priority so should be will first - however 
the order in which they are executed is not guarenteeed to be the order in which 
they are added to the scheduler.

Task 3 has the next highest priority show will be executed next, followed by
Task Task 2, then Task 5, then Task 4.
*/
//#define DEMO_FPS_BASIC

/*
This demonstration shows how OS_Sleep() can be used. It contains three tasks, 
Task 1, Task 2 and Task 3. They all have the same priority so their execution
order cannot be guarenteed.

All three tasks go to sleep as the tirst thing they do. Task 3 sleeps for less 
time than Task 2, which also sleeps for less time than Task 1. This means that
Task 3 gets woken before Task 2, which gets woken before Task 1.
*/
//#define DEMO_FPS_SLEEP

/*
This demonstration shows how the wait mechanism can be used. It contains three
tasks, Task 1, Task 2 and Task 3. All three tasks have the same priority so 
there is no guarentee in which order they will be executed. 

Task 3 is put to sleep to ensure Task 1 and Task 2 are made to wait. Once Task 3
has woken from sleep, it notifies Task 1 and subsequently goes back to sleep.
Task 1 then completes its execution.

After Task 3 has, once again, woken, it notifies Task 2 which subsequently
finishes its execution.
*/
//#define DEMO_FPS_WAIT

/***************************************/
/* Inter-Task Communication Queue Demo */
/***************************************/
 
/* 
This demonstration shows how a single message can be sent and received across
two tasks. There are two tasks, the sender task and the receiver task. Note, the
receiver task has a lower priority than the sender task. This is to ensure the 
sender task sends the message first.

When the demo is run, the sender task sends a message with the destination as 
the receiver. The receiver then reads this message and outputs it.
*/
//#define DEMO_QUEUE_READ_SINGLE_MSG  
 
/*
This demonstration shows how a task sending more messages than the message 
queue can handle is made to wait until there are free spaces in the queue. There
are three tasks, Sender Task, Receiver Task 1 and Receiver Task 2. Note, Sender
Task has the highest priority to ensure that it is run first, and Receiver Task 
1 has a higher priority that Receiver Task 2 to ensure that Receiver Task 1
is executed before Receiver Task 2.

When the demo is run, Sender Task sends 5 messages to the message queue with the 
destination as Receiver Task 1, and then attempts to send another 15 messages to 
the message queue with the destination as Receiver Task 2. However, during this
the message queue becomes full and Sender Task gets put to sleep.

At this stage, the message queue is half full with the destination as Receiver
Task 1, and half full with the destination as Receiver Task 2. 

Receiver Task 1 then reads a message which notifies the Sender Task that there
is a space in the message queue available. Receiver Task 1 then gets made to 
wait until the Sender Task has sent another message, after which it reads 
another message. This repeats for both the Receiver Task 1 and Receiver Task 2
until all messages have been sent. 

The output will not be in the same order as the messages were sent. This is 
because the OS_itcRead() function simply finds the first message meant for a 
task in the message queue. When Receiver Task 1 reads a message, a space at the 
start of the queue becomes available, therefore the next message added to the 
message queue will be at the front - of which the destination will be for 
Receiver Task 2. Hence, any messages for Receiver Task sent before the message
queue first gets full, will be read and outputted after all messages sent
after it was first put to sleep.

This is an area of improvement for the ITC system. Ideally, it should use a 
priority queue with the messages having different priorities, but I had to be 
realistic about implementing this as I was running out of time in the 
assignment.
*/
//#define DEMO_QUEUE_FULL

/*
This demo shows how a task waits if there are no messages in the queue, until 
a message is added. There are two tasks, the Sender Task and the Receiver Task. 
Note, the Receiver Task has a higher priority than the Sender Task to ensure it 
reads from an empty queue.

When run, the Receiver Task attempts to read from the empty message queue and is
put into the wait state. The Sender Task then sends a message which notifies the 
Receiver Task and it then reads the message.
*/
//#define DEMO_QUEUE_EMPTY

/********************/
/* Memory Pool Demo */
/********************/

/* 
This demonstration shows how the memory pool can be used. In this demo, there 
two tasks, Task 1 and Task 2. Task 1 gets executed before Task 2 because it 
has a higher priority.

During the demo's initialisation, the memory pool is given 3 memory locations. 
This value was chosen to easily fill up the memory pool to show what happens if 
a task attempts to allocate memory when the pools is full.

When the demo is run, Task 1 allocates memory for the pointers p1, p2 and p3. 
After this the memory pool is full. When Task 1 subsequently attempts to 
allocate memory for the fourth pointer, p4, it is put into the wait state. 

Following that, Task 2 is executed. It first sleeps for a while to demonstrate
that Task 1 has gone into the wait state, and then deallocates p1. During this 
deallocation, Task 1 gets notified so it can be once again executed. Task 1 then 
allocates memory for the p4 pointer (in the same address that was allocated)
for p1.

This process repeats for another pointer, p5. Instead, however, Task 2 
deallocated p3.
*/
//#define DEMO_MP

void DemoInitMuxTasks(void);
void DemoInitFPSTasks(void);
void DemoInitITCQueueTasks(void);
void DemoInitMPTasks(void);

#endif  // DEMONSTRATION_TASKS_H
