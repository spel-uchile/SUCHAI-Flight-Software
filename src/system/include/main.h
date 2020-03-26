/**
 * @file  main.h
 *
 * @author Carlos Gonzalez C
 * @author Camilo Rojas M
 * @author Tomas Opazo T
 * @author Tamara Gutierrez R
 * @author Matias Ramirez M
 * @author Ignacio Ibanez A
 * @author Diego Ortego P
 * @author Elias Obreque S
 *
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This header contains the main's includes
 */

#ifndef SUCHAI_FLIGHT_SOFTWARE_MAIN_H
#define SUCHAI_FLIGHT_SOFTWARE_MAIN_H

/* std includes */
#include <stdio.h>
#include <signal.h>

/* Global includes */
#include "drivers.h"
#include "init.h"
#include "config.h"
#include "globals.h"
#include "utils.h"

/* OS includes */
#include "osThread.h"
#include "osScheduler.h"
#include "osQueue.h"
#include "osSemphr.h"
#include "os/os.h"

/* system includes */
#include "repoData.h"
#include "repoCommand.h"

/* Task includes */
#include "taskInit.h"
#include "taskDispatcher.h"
#include "taskExecuter.h"
#include "taskWatchdog.h"
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
#if SCH_TEST_ENABLED
#include "taskTest.h"
#endif

/**
 * ESP32 already define main in the esp-id framework, the entry point
 * is app_main() instead
 */
#ifdef ESP32
    void app_main();
#else
    int main(void);
#endif

#endif //SUCHAI_FLIGHT_SOFTWARE_MAIN_H
