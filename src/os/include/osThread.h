/**
 * @file  osThread.h
 * @author Carlos Gonzalez Cortes
 * @author Ignacio Ibanez Aliaga
 * @date 2017
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
    #include <stdio.h>
    typedef pthread_t os_thread;
#else
    #include "FreeRTOSConfig.h"
    #include "FreeRTOS.h"
    #include "task.h"
    typedef portBASE_TYPE os_thread;
#endif

void osCreateTask(void (*functionTask)(void *), char* name, unsigned short size, void * parameters, unsigned int priority, os_thread* thread);
void osTaskDelete(void *task_handle);

#endif // _OS_THREAD_H_
