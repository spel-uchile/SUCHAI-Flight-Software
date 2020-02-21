/**
 * @file  osScheduler.h
 * @author Carlos Gonzalez Cortes
 * @author Ignacio Ibanez Aliaga
 * @date 2020
 * @copyright GNU Public License.
 *
 * Creation of functions related with the scheduler for systems operating Linux as FreeRTOS
 * 
 */

#ifndef _OS_SCHEDULER_H_
#define _OS_SCHEDULER_H_

#include <stdio.h>
#include <stdlib.h>
#include "config.h"

#ifdef LINUX
    #include "osThread.h"
    #include "utils.h"
#else
    #include "FreeRTOS.h"
    #include "task.h"
    #include "osThread.h"
#endif

void osScheduler(os_thread* threads_id, int n_threads);

#endif
