/**
 * @file  main.h
 *
 * @author Carlos Gonzalez C
 * @author Camilo Rojas M
 * @author Tomas Opazo T
 * @author Tamara Gutierrez R
 * @author Matias Ramirez M
 * @author Ignacio Ibanez A
 *
 * @date 2018
 * @copyright GNU GPL v3
 *
 * This header contains the main's includes
 */

#ifndef SUCHAI_FLIGHT_SOFTWARE_MAIN_H
#define SUCHAI_FLIGHT_SOFTWARE_MAIN_H


#include "config.h"
#include <stdio.h>
#include <signal.h>

#ifdef AVR32
    #include "asf.h"
    #include <time.h>
    #include <avr32/io.h>
    #include "intc.h"
    #include "board.h"
    #include "compiler.h"
    #include "rtc.h"
    #include "usart.h"
    #include "pm.h"

#endif

#ifdef NANOMIND
    #include <conf_a3200.h>

    #include <sysclk.h>
    #include <wdt.h>
    #include <gpio.h>
    #include <sdramc.h>
    #include <reset_cause.h>

    #include <dev/usart.h>
    #include <dev/i2c.h>
    #include <dev/cpu.h>

    #include <fm33256b.h>

    #include <led.h>
    #include <pwr_switch.h>
#endif


/* OS includes */
#include "osThread.h"
#include "osScheduler.h"
#include "osQueue.h"
#include "osSemphr.h"
#include "os.h"

/* system includes */
#include "globals.h"
#include "utils.h"
#include "repoData.h"
#include "repoCommand.h"
#include "init.h"

#ifdef LINUX
    #include <csp/csp.h>
    #include <csp/interfaces/csp_if_zmqhub.h>
#endif

/* Task includes */
#if SCH_TEST_ENABLED
    #include "taskTest.h"
#endif

#include "taskDispatcher.h"
#include "taskExecuter.h"
#include "taskWatchdog.h"
#include "taskConsole.h"
#include "taskInit.h"

#if SCH_HK_ENABLED
    #include "taskHousekeeping.h"
#endif
#if SCH_COMM_ENABLE
    #include "taskCommunications.h"
#endif
#if SCH_FP_ENABLED
    #include "taskFlightPlan.h"
#endif

#ifdef ESP32
    void app_main();
#else
    int main(void);
#endif

#endif //SUCHAI_FLIGHT_SOFTWARE_MAIN_H
