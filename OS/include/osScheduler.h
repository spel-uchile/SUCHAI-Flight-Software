/**
 * @file  scheduler.h
 * @author Ignacio Ibanez Aliaga
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * Creation of functions related with the scheduler for systems operating Linux as FreeRTOS
 * 
 */

#ifndef _OS_SCHEDULER_H_
#define _OS_SCHEDULER_H_

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

#if __linux__
    #include "osThread.h"
#else
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "osThread.h"
#endif

void osScheduler(os_thread** thread_id, int n_thread);

#endif
