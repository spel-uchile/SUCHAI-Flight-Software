//
// Created by matias on 05-04-18.
//

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
#include "taskCommunications.h"
#endif
#if SCH_FP_ENABLED
#include "taskFlightPlan.h"
#endif

void taskInit(void *param);
void init_communications(void);

#endif //SUCHAI_FLIGHT_SOFTWARE_TASKINIT_H
