/**
 * @file  scheduler.h
 * @author Ignacio Iba�ez Aliaga 
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * Creation of functions related with the scheduler for systems operating Linux as FreeRTOS
 * 
 */

#ifndef _OS_SCHEDULER_H_
#define _OS_SCHEDULER_H_

#include "../../SUCHAI_config.h"
#include "osThread.h"

void osScheduler(os_thread** thread_id, int n_thread);

#endif