/**
 * @file  SUCHAI_config.h
 * @author Carlos Gonzalez C
 * @author Tomas Opazo T
 * @autor Ignacio Ibanez A
 * @date 2017
 * @copyright GNU Public License.
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
#define {{OS}}                              ///< LINUX | FREERTOS
//#define FREERTOS                          ///< Use FreeRTOS (select arch below)
/* Select the correct architecture */
#ifdef FREERTOS
    #define {{ARCH}}                        ///< ESP32 | AVR32 | NANOMIND
#endif

/* System debug configurations */
#define LOG_LEVEL       {{LOG_LVL}};     ///<  LOG_LVL_INFO |  LOG_LVL_DEBUG

/* General system settings */
#define SCH_BUFF_MAX_LEN            (256)   ///< General buffers max length (bytes)
#define SCH_COMM_ENABLE             {{SCH_COMM}}       ///< TaskCommunications enabled (0 | 1)
#define SCH_FP_ENABLED              {{SCH_FP}};       ///< TaskFlightPlan enabled (0 | 1)
#define SCH_HK_ENABLED              {{SCH_HK}};       ///< TaskHousekeeping enabled (0 | 1)
#define SCH_TEST_ENABLED            {{SCH_TEST}};       ///< Set to run tests (0 | 1)
#define SCH_MAX_WDT_TIMER           10      ///< Seconds to send wdt_reset command
#define SCH_MAX_GND_WDT_TIMER       (3600*48)  ///< Seconds to reset the OBC if the ground watchdog was not clear

/* Communications system settings */
#define SCH_COMM_ADDRESS            (1)                     ///< Node address
#define SCH_TRX_PORT_TM             (10)                    ///< Telemetry port (in the ground station)
#define SCH_TRX_PORT_TC             (10)                    ///< Telecommands post (in the flight software)
#define SCH_TRX_PORT_DEBUG          (11)                    ///< Debug port (just print packets)
#define SCH_TRX_PORT_CONSOLE        (12)                    ///< Console port (execute console commands)
#define SCH_COMM_ZMQ_OUT            "tcp://192.168.0.36:8001"  ///< Out socket URI
#define SCH_COMM_ZMQ_IN             "tcp://192.168.0.36:8002"  ///< In socket URI

/* Data repository settings */
#define SCH_STORAGE_MODE   	{{SCH_ST_MODE}}   ///< Status repository location. (0) RAM, (1) Single external.
#define SCH_STORAGE_FILE    "/tmp/suchai.db"

#endif	/* SUCHAI_CONFIG_H */
