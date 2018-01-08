//
// Created by carlos on 19-10-17.
//

#ifndef SUCHAI_FLIGHT_SOFTWARE_MAIN_H
#define SUCHAI_FLIGHT_SOFTWARE_MAIN_H

#include <stdio.h>
#include <signal.h>


/* OS includes */
#include "osThread.h"
#include "osScheduler.h"
#include "osQueue.h"
#include "osSemphr.h"
#include "os.h"

/* system includes */
#include "config.h"
#include "globals.h"
#include "utils.h"
#include "repoData.h"
#include "repoCommand.h"

#ifdef LINUX
    #include <csp/csp.h>
    #include <csp/interfaces/csp_if_zmqhub.h>
#endif

/* Task includes */
//#include "taskTest.h"
#include "taskDispatcher.h"
#include "taskExecuter.h"
#include "taskHousekeeping.h"
#include "taskConsole.h"
#if SCH_COMM_ENABLE
    #include "taskCommunications.h"
#endif
#if SCH_STATUS_REPO_MODE
    #include "taskFlightPlan.h"
#endif

#ifdef FREERTOS
    void app_main();
#else
    int main(void);
#endif


#endif //SUCHAI_FLIGHT_SOFTWARE_MAIN_H
