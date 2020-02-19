/**
 * @file  osThread.h
 * @author Carlos Gonzalez Cortes
 * @author Ignacio Ibanez Aliaga
 * @date 2020
 * @copyright GNU Public License.
 *
 * 
 * Creation of functions related with tasks for systems operating Linux as FreeRTOS.
 */

#ifndef _OS_THREAD_H_
#define _OS_THREAD_H_

#include "config.h"

#ifdef LINUX
    #include <pthread.h>
    #include <features.h>
    #include <stdio.h>
    typedef pthread_t os_thread;
#else
    #include "FreeRTOSConfig.h"
    #include "FreeRTOS.h"
    #include "task.h"
    typedef portBASE_TYPE os_thread;
#endif

/**
 * Create a new task.
 * In GNU/Linux a new thread is created and started inmediately. In FreeRTOS
 * a new Task is created but will start after a call to osScheduler().
 *
 * @param functionTask Pointer to the target function task
 * @param name Task name. In GNU/Linux max 16 chars.
 * @param size Task stack size.
 * For GNU/Linux @see http://man7.org/linux/man-pages/man3/pthread_attr_setstacksize.3.html.
 * For FreeRTOS @see https://www.freertos.org/a00125.htm)
 * @param parameters Pointer to stack parameters
 * @param priority Task priority.
 * For GNU/Linux @see http://man7.org/linux/man-pages/man3/pthread_getschedparam.3.html
 * For FreeRTOS @see https://www.freertos.org/a00125.html
 * @param thread Pointer to store the task handler
 *
 * @return Returns 0 on success, error code if the task can not be created.
 */
int osCreateTask(void (*functionTask)(void *), char* name, unsigned short size, void * parameters, unsigned int priority, os_thread* thread);

/**
 * Delete a task. Only in FreeRTOS, not implemented for GNU/Linux
 * @param task_handle Pinter to a task handler
 */
void osTaskDelete(void *task_handle);

#endif // _OS_THREAD_H_
