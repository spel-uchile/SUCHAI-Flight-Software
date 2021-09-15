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
#include "suchai/init.h"
#include "drivers.h"

#include "suchai/osThread.h"
#include "suchai/osQueue.h"
#include "suchai/osDelay.h"

/* Task includes */
#include "suchai/taskConsole.h"
#if SCH_COMM_ENABLE
#include "suchai/taskCommunications.h"
#endif
#if SCH_FP_ENABLED
#include "suchai/taskFlightPlan.h"
#endif

static csp_iface_t *csp_if_zmqhub;

void csp_add_zmq_iface(int node);
int init_setup_libcsp(int node);


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
