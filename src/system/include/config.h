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

/* Arch settings */
#define LINUX
//#define FREERTOS
//#define ESP32


/* System debug configurations */
#define LOG_LEVEL       LOG_LVL_VERBOSE   ///< Define debug levels
#define SCH_RUN_TESTS   0                 ///< Enable orfreertos disable tests

/* General system settings */
#define SCH_BUFF_MAX_LEN            (256) ///< General buffers max length (bytes)

/* Communications system settings */
#define SCH_COMM_ADDRESS            (10)                     ///< Node address
#define SCH_TRX_PORT_TM             (10)                    ///< Telemetry port (in the ground station)
#define SCH_TRX_PORT_TC             (10)                    ///< Telecommands post (in the flight software)
#define SCH_TRX_PORT_DEBUG          (11)                    ///< Debug port (just print packets)
#define SCH_TRX_PORT_CONSOLE        (12)                    ///< Console port (execute console commands)
#define SCH_COMM_ZMQ_OUT            "tcp://192.168.0.36:8001"  ///< Out socket URI
#define SCH_COMM_ZMQ_IN             "tcp://192.168.0.36:8002"  ///< In socket URI
#define SCH_COMM_ENABLE             1

/*Flight Plan settings*/
#define SCH_RUN_FP                  1

/* Data repository settings */
#define SCH_STORAGE_MODE   	0   ///< Status repository location. (0) RAM, (1) Single external.
#define SCH_STORAGE_FILE            "/tmp/suchai.db"

#endif	/* SUCHAI_CONFIG_H */
