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

#include "osQueue.h"

/* System debug configurations */
#define LOG_LEVEL       LOG_LVL_VERBOSE   ///< Define debug levels
#define SCH_RUN_TESTS   0                 ///< Enable or disable tests

/* General system settings */
#define SCH_BUFF_MAX_LEN            (256) ///< General buffers max length (bytes)

/* Communications system settings */
#define SCH_COMM_ADDRESS            (1)                     ///< Node address
#define SCH_TRX_PORT_TM             (10)                    ///< Telemetry port (in the ground station)
#define SCH_TRX_PORT_TC             (10)                    ///< Telecommands post (in the flight software)
#define SCH_TRX_PORT_DEBUG          (11)                    ///< Debug port (just print packets)
#define SCH_TRX_PORT_CONSOLE        (12)                    ///< Console port (execute console commands)
#define SCH_COMM_ZMQ_OUT            "tcp://127.0.0.1:8001"  ///< Out socket URI
#define SCH_COMM_ZMQ_IN             "tcp://127.0.0.1:8002"  ///< In socket URI

/* Data repository settings */
#define SCH_STATUS_REPO_MODE    	0   ///< Status repository location. (0) Internal, (1) Single external.
#define SCH_STORAGE_FILE            "/tmp/suchai.db"

#endif	/* SUCHAI_CONFIG_H */
