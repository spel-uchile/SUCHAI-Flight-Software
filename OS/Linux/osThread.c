/**
 * @file  Linux/thread.c
 * @author Ignacio Ibañez Aliaga 
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * Creation of functions related with thread for systems operating Linux.
 *
 */

#include "../include/osThread.h"

/**
 * create a task in Linux as thread
 */
void osCreateTask(void (*functionTask)(void *), char* name, unsigned short size, void * parameters, unsigned int priority, os_thread* thread){
    pthread_create(thread , NULL , (void *)(*functionTask) , parameters);

    // Set Real Time scheduling and thread priority
    // Only with proper permissions
    const struct sched_param _priority = {(int) priority};
    if(pthread_setschedparam(*thread, SCHED_FIFO, &_priority) != 0)
        printf("[Warning] (%s) Failed to assign task priority, try as root\n", name);
}
