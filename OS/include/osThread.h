/**
 * @file  thread.h
 * @author Ignacio Iba�ez Aliaga 
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * 
 * Creation of functions related with tasks for systems operating Linux as FreeRTOS.
 */

#ifndef _OS_THREAD_H_
#define _OS_THREAD_H_

#if __linux__
    #include <pthread.h>
    #include <stdio.h>
    typedef pthread_t os_thread;
#else
    #include "freertos/FreeRTOSConfig.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    typedef portBASE_TYPE os_thread;
#endif

void osCreateTask(void (*functionTask)(void *), char* name, unsigned short size, void * parameters, unsigned int priority, os_thread* thread);

#endif
