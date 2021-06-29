/**
 * @file  taskInit.h
 * @author Matias Ramirez  - nicoram.mt@gmail.com
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This task implements a listener, that sends commands at periodical times.
 */

#ifndef SUCHAI_FLIGHT_SOFTWARE_TASKINIT_H
#define SUCHAI_FLIGHT_SOFTWARE_TASKINIT_H

#include "suchai/log_utils.h"
#include "suchai/config.h"
#include "init.h"
#include "drivers.h"

#include "suchai/osThread.h"
#include "suchai/osQueue.h"
#include "suchai/osDelay.h"

/* Task includes */
#include "taskConsole.h"
#if SCH_COMM_ENABLE
#include "taskCommunications.h"
#endif
#if SCH_FP_ENABLED
#include "taskFlightPlan.h"
#endif

void taskInit(void *param);

/**
 * This function is called at the end of taskInit to execute
 * specific app initialization routines
 *
 * @note APP MUST PROVIDE AN IMPLEMENTATION OF THIS FUNCTION
 *
 * @param params pointer to taskInit params
 */
extern void initAppHook(void *params);

#endif //SUCHAI_FLIGHT_SOFTWARE_TASKINIT_H
