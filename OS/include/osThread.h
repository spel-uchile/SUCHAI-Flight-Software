/**
 * @file  thread.h
 * @author Ignacio Ibañez Aliaga 
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * 
 * Creation of functions related with tasks for systems operating Linux as FreeRTOS.
 */

#ifndef _OS_THREAD_H_
#define _OS_THREAD_H_

#include "../../SUCHAI_config.h"

#if __linux__
    #include <pthread.h>
#else
    #include "../../FreeRTOSConfig.h"
#endif


void osCreateTask(void (*functionTask)(void *), char* name, unsigned short size, void * parameters, unsigned int priority);

#endif
