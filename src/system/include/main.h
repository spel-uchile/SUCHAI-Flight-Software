//
// Created by carlos on 19-10-17.
//

#ifndef SUCHAI_FLIGHT_SOFTWARE_MAIN_H
#define SUCHAI_FLIGHT_SOFTWARE_MAIN_H

#include <stdio.h>
#include <signal.h>
#include <csp/csp.h>
#include <csp/interfaces/csp_if_zmqhub.h>

/* OS includes */
#include "osThread.h"
#include "osScheduler.h"
#include "osQueue.h"
#include "osSemphr.h"

/* system includes */
#include "config.h"
#include "globals.h"
#include "utils.h"
#include "repoData.h"
#include "repoCommand.h"

/* Task includes */
#include "taskTest.h"
#include "taskDispatcher.h"
#include "taskExecuter.h"
#include "taskHousekeeping.h"
#include "taskConsole.h"
#include "taskCommunications.h"

#endif //SUCHAI_FLIGHT_SOFTWARE_MAIN_H
