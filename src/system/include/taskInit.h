//
// Created by matias on 05-04-18.
//

#ifndef SUCHAI_FLIGHT_SOFTWARE_TASKINIT_H
#define SUCHAI_FLIGHT_SOFTWARE_TASKINIT_H

#include "utils.h"
#include "osThread.h"
#include "config.h"


/* Task includes */
#if SCH_TEST_ENABLED
#include "taskTest.h"
#endif
#include "taskDispatcher.h"
#include "taskExecuter.h"
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

void taskInit(void *param);

#endif //SUCHAI_FLIGHT_SOFTWARE_TASKINIT_H
