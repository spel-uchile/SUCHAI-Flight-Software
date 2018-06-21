/**
 * @file  config.h
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
 * This header contains system wide settings to customize different submodules
 */

#ifndef SUCHAI_CONFIG_H
#define	SUCHAI_CONFIG_H

/* General includes */
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

/* Select one operating system */
#define LINUX                                      ///< LINUX | FREERTOS
/* Select the correct architecture */
#ifdef FREERTOS
    #define ESP32                                ///< ESP32 | AVR32 | NANOMIND
#endif

/* System debug configurations */
#define LOG_LEVEL               LOG_LVL_INFO        ///<  LOG_LVL_INFO |  LOG_LVL_DEBUG

/* General system settings */
#define SCH_BUFF_MAX_LEN        (256)              ///< General buffers max length (bytes)
#define SCH_COMM_ENABLE         1   ///< TaskCommunications enabled (0 | 1)
#define SCH_FP_ENABLED          1      ///< TaskFlightPlan enabled (0 | 1)
#define SCH_HK_ENABLED          1      ///< TaskHousekeeping enabled (0 | 1)
#define SCH_TEST_ENABLED        1    ///< Set to run tests (0 | 1)
#define SCH_MAX_WDT_TIMER       100000                 ///< Seconds to send wdt_reset command
#define SCH_MAX_GND_WDT_TIMER   (3600*48)          ///< Seconds to reset the OBC if the ground watchdog was not clear

/* Communications system settings */
#define SCH_COMM_ADDRESS        1  ///< Node address
#define SCH_TRX_PORT_TM         (9)                ///< Telemetry port
#define SCH_TRX_PORT_TC         (10)               ///< Telecommands port
#define SCH_TRX_PORT_RPT        (11)               ///< Digirepeater port (resend packets)
#define SCH_TRX_PORT_CMD        (12)               ///< Commads port (execute console commands)
#define SCH_COMM_ZMQ_OUT        "tcp://127.0.0.1:8001"  ///< Out socket URI
#define SCH_COMM_ZMQ_IN         "tcp://127.0.0.1:8002"   ///< In socket URI

/* Data repository settings */
#define SCH_STORAGE_MODE        1    ///< Status repository location. (0) RAM, (1) Single external.
#define SCH_STORAGE_FILE        "/tmp/suchai.db"   ///< File to store the database, only if @SCH_STORAGE_MODE is 1

#endif	/* SUCHAI_CONFIG_H */
