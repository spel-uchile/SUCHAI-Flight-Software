/**
 * @file  taskInit.h
 * @author Matias Ramirez  - nicoram.mt@gmail.com
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2018
 * @copyright GNU GPL v3
 *
 * This task implements a listener, that sends commands at periodical times.
 */

#ifndef SUCHAI_FLIGHT_SOFTWARE_TASKINIT_H
#define SUCHAI_FLIGHT_SOFTWARE_TASKINIT_H

#include "utils.h"
#include "config.h"
#include "init.h"
#include "osThread.h"

/* Task includes */
#include "taskConsole.h"


#if SCH_HK_ENABLED
#include "taskHousekeeping.h"
#endif
#if SCH_COMM_ENABLE
#include <csp/csp.h>
#include <csp/csp_rtable.h>
#include <csp/interfaces/csp_if_kiss.h>
#include <csp/drivers/usart.h>
#ifdef NANOMIND
#include <csp/interfaces/csp_if_i2c.h>
#endif
#include "taskCommunications.h"
#endif
#if SCH_FP_ENABLED
#include "taskFlightPlan.h"
#endif

void taskInit(void *param);
void init_communications(void);

#endif //SUCHAI_FLIGHT_SOFTWARE_TASKINIT_H
