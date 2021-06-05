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

#include "log_utils.h"
#include "config.h"
#include "init.h"
#include "drivers.h"

#include "osThread.h"
#include "osQueue.h"
#include "osDelay.h"

/* Task includes */
#include "taskConsole.h"
#if SCH_HK_ENABLED
#include "taskHousekeeping.h"
#endif
#if SCH_COMM_ENABLE
#include "taskCommunications.h"
#endif
#if SCH_FP_ENABLED
#include "taskFlightPlan.h"
#endif
#if SCH_SEN_ENABLED
#include "taskSensors.h"
#endif
#if SCH_ADCS_ENABLED
#include "taskADCS.h"
#endif

void taskInit(void *param);

#endif //SUCHAI_FLIGHT_SOFTWARE_TASKINIT_H
