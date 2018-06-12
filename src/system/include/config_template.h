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
#define {{OS}}                                      ///< LINUX | FREERTOS
/* Select the correct architecture */
#ifdef FREERTOS
    #define {{ARCH}}                                ///< ESP32 | AVR32 | NANOMIND
#endif

/* System debug configurations */
#define LOG_LEVEL               {{LOG_LVL}}        ///<  LOG_LVL_INFO |  LOG_LVL_DEBUG

/* General system settings */
#define SCH_BUFF_MAX_LEN        (256)              ///< General buffers max length (bytes)
#define SCH_COMM_ENABLE         {{SCH_EN_COMM}}    ///< TaskCommunications enabled (0 | 1)
#define SCH_FP_ENABLED          {{SCH_EN_FP}}      ///< TaskFlightPlan enabled (0 | 1)
#define SCH_HK_ENABLED          {{SCH_EN_HK}}      ///< TaskHousekeeping enabled (0 | 1)
#define SCH_TEST_ENABLED        {{SCH_EN_TEST}}    ///< Set to run tests (0 | 1)
#define SCH_MAX_WDT_TIMER       10                 ///< Seconds to send wdt_reset command
#define SCH_MAX_GND_WDT_TIMER   (3600*48)          ///< Seconds to reset the OBC if the ground watchdog was not clear

/* Communications system settings */
#define SCH_COMM_ADDRESS        {{SCH_COMM_NODE}}  ///< Node address
#define SCH_TRX_PORT_TM         (9)                ///< Telemetry port
#define SCH_TRX_PORT_TC         (10)               ///< Telecommands port
#define SCH_TRX_PORT_RPT        (11)               ///< Digirepeater port (resend packets)
#define SCH_TRX_PORT_CMD        (12)               ///< Commads port (execute console commands)
#define SCH_COMM_ZMQ_OUT        "{{SCH_ZMQ_OUT}}"  ///< Out socket URI
#define SCH_COMM_ZMQ_IN         "{{SCH_ZMQ_IN}}"   ///< In socket URI

/* Data repository settings */
#define SCH_STORAGE_MODE        {{SCH_STORAGE}}    ///< Status repository location. (0) RAM, (1) Single external.
#define SCH_STORAGE_FILE        "/tmp/suchai.db"   ///< File to store the database, only if @SCH_STORAGE_MODE is 1

/**
 * Memory settings.
 *
 * Control the memory used by task stacks, static allocated buffers, etc.
 * Note that in FreeRTOS the stack size is measured in words not bytes, so the
 * final stack size depends on the architecture stack wide
 * (@see https://www.freertos.org/a00125.html)
 */
#define SCH_TASK_DEF_STACK        (1*256)   ///< Default task stack size in words
#define SCH_TASK_DIS_STACK        (5*256)   ///< Dispatcher task stack size in words
#define SCH_TASK_EXE_STACK        (5*256)   ///< Executer task stack size in words
#define SCH_TASK_WDT_STACK        (5*256)   ///< Watchdog task stack size in words
#define SCH_TASK_INI_STACK        (5*256)   ///< Init task stack size in words
#define SCH_TASK_COM_STACK        (4*256)   ///< Communications task stack size in words
#define SCH_TASK_FPL_STACK        (4*256)   ///< Flight plan task stack size in words
#define SCH_TASK_CON_STACK        (4*256)   ///< Console task stack size in words
#define SCH_TASK_HKP_STACK        (4*256)   ///< Housekeeping task stack size in words
#define SCH_TASK_CSP_STACK        (500)     ///< CSP route task stack size in words

#define SCH_BUFF_MAX_LEN          (256)     ///< General buffers max length in bytes
#define SCH_BUFFERS_CSP           (5)       ///< Number of available CSP buffers
#define SCH_FP_MAX_ENTRIES        (25)      ///< Max number of flight plan entries
#define SCH_CMD_MAX_ENTRIES       (50)      ///< Max number of commands in the repository
#define SCH_CMD_MAX_STR_PARAMS    (64)      ///< Limit for the parameters length

#endif //SUCHAI_CONFIG_H
