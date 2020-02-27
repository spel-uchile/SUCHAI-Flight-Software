/**
 * @file  config.h
 *
 * @author Carlos Gonzalez C
 * @author Camilo Rojas M
 * @author Tomas Opazo T
 * @author Tamara Gutierrez R
 * @author Matias Ramirez M
 * @author Ignacio Ibanez A
 * @author Diego Ortego P
 *
 * @date 2020
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
#define LINUX        ///< LINUX | FREERTOS
/* Select the correct architecture */
#define X86      ///< X86 | RPI | GROUNDSTATION | ESP32 | AVR32 | NANOMIND

/* Platform specific settings */
#ifdef GROUNDSTATION
    #define SCH_RESEND_TM_NODE  11  ///< If defined, resend TM packets to CosmosRB node
    #define SCH_USE_NANOPOWER
    #define SCH_USE_NANOCOM
    //#define SCH_USE_GSSB
#endif

#ifdef NANOMIND
    #define SCH_USE_NANOPOWER
    #define SCH_USE_NANOCOM
    #define SCH_USE_GSSB
#endif

/* System debug configurations */
#define LOG_LEVEL               LOG_LVL_INFO      ///< LOG_LVL_INFO |  LOG_LVL_DEBUG
#define SCH_NAME                "SUCHAI-DEV"      ///< Project code name
#define SCH_DEVICE_ID           0                 ///< Device unique ID
#define SCH_SW_VERSION          "2.1.5"           ///< Software version

/* General system settings */
#define SCH_CON_ENABLED         1     ///< TaskConsole enabled (0 | 1)
#define SCH_COMM_ENABLE         1    ///< TaskCommunications enabled (0 | 1)
#define SCH_FP_ENABLED          1      ///< TaskFlightPlan enabled (0 | 1)
#define SCH_HK_ENABLED          1      ///< TaskHousekeeping enabled (0 | 1)
#define SCH_SEN_ENABLED         0     ///< TaskSensors enabled (0 | 1)
#define SCH_TEST_ENABLED        0    ///< Set to run tests (0 | 1)
#define SCH_WDT_PERIOD          120                ///< CPU watchdog timer period in seconds
#define SCH_MAX_WDT_TIMER       60                 ///< Seconds to send wdt_reset command
#define SCH_MAX_GND_WDT_TIMER   (3600*48)          ///< Seconds to reset the OBC if the ground watchdog was not clear
#define SCH_UART_BAUDRATE       (500000)           ///< UART baud rate for serial console
#define SCH_KISS_UART_BAUDRATE  (500000)           ///< UART baud rate for kiss communication
#define SCH_KISS_DEVICE         "/dev/ttyUSB0"     ///< Kiss device path

/* Communications system settings */
#define SCH_COMM_ADDRESS        1  ///< Node address
#define SCH_TNC_ADDRESS         9                  ///< TNC node address
#define SCH_TRX_ADDRESS         5                  ///< TRX node address
#define SCH_EPS_ADDRESS         2                  ///< EPS node address
#define SCH_TRX_PORT_TM         (9)                ///< Telemetry port
#define SCH_TRX_PORT_TC         (10)               ///< Telecommands port
#define SCH_TRX_PORT_RPT        (11)               ///< Digirepeater port (resend packets)
#define SCH_TRX_PORT_CMD        (12)               ///< Commands port (execute console commands)
#define SCH_TRX_PORT_DBG        (13)               ///< Debug port, logs output
#define SCH_COMM_ZMQ_OUT        "tcp://127.0.0.1:8002"  ///< Out socket URI
#define SCH_COMM_ZMQ_IN         "tcp://127.0.0.1:8001"   ///< In socket URI
#define SCH_TX_INHIBIT          10                 /// Default silent time in seconds [0, 1800 (30min)]
#define SCH_TX_PWR              0                  /// Default TX power [0|1|2|3]
#define SCH_TX_BCN_PERIOD       60                 /// Default beacon period in seconds
#define SCH_TX_FREQ             437250000          /// Default TRX freq in Hz
#define SCH_TX_BAUD             4800               /// Default TRX baudrate [4800|9600|19200

/* Data repository settings */
#define SCH_STORAGE_MODE        1    ///< Status repository location. (0) RAM, (1) Single external.
#define SCH_STORAGE_TRIPLE_WR   1   ///< Tripled writing enabled (0 | 1)
#define SCH_STORAGE_FILE        "/tmp/suchai.db"   ///< File to store the database, only if @SCH_STORAGE_MODE is 1
#define SCH_STORAGE_PGUSER      "spel"

#define SCH_SECTIONS_PER_PAYLOAD 2                 ///< Memory blocks for storing each payload type TODO: Make configurable per payload
#define SCH_SIZE_PER_SECTION 256*1024              ///< Size of each memory block in flash storage
#define SCH_FLASH_INIT_MEMORY 0                    ///< Initial address in flash storage

/**
 * Memory settings.
 *
 * Control the memory used by task stacks, static allocated buffers, etc.
 * Note that in FreeRTOS the stack size is measured in words not bytes, so the
 * final stack size depends on the architecture stack wide
 * (@see https://www.freertos.org/a00125.html)
 */
#define SCH_TASK_DEF_STACK        (2*256)   ///< Default task stack size in words
#define SCH_TASK_DIS_STACK        (5*256)   ///< Dispatcher task stack size in words
#define SCH_TASK_EXE_STACK        (5*256)   ///< Executer task stack size in words
#define SCH_TASK_WDT_STACK        (5*256)   ///< Watchdog task stack size in words
#define SCH_TASK_INI_STACK        (5*256)   ///< Init task stack size in words
#define SCH_TASK_COM_STACK        (5*256)   ///< Communications task stack size in words
#define SCH_TASK_FPL_STACK        (5*256)   ///< Flight plan task stack size in words
#define SCH_TASK_CON_STACK        (5*256)   ///< Console task stack size in words
#define SCH_TASK_HKP_STACK        (5*256)   ///< Housekeeping task stack size in words
#define SCH_TASK_CSP_STACK        (5*256)     ///< CSP route task stack size in words
#define SCH_TASK_SEN_STACK        (5*256)   ///< Sensor task stack size in words

#define SCH_BUFF_MAX_LEN          (256)     ///< General buffers max length in bytes
#define SCH_BUFFERS_CSP           (10)       ///< Number of available CSP buffers
#define SCH_FP_MAX_ENTRIES        (25)      ///< Max number of flight plan entries
#define SCH_CMD_MAX_ENTRIES       (255)      ///< Max number of commands in the repository
#define SCH_CMD_MAX_STR_PARAMS    (256)      ///< Limit for the parameters length
#define SCH_CMD_MAX_STR_NAME      (256)      ///< Limit for the length of the name of a command
#define SCH_CMD_MAX_STR_FORMAT    (128)      ///< Limit for the length of the format field of a command

#endif //SUCHAI_CONFIG_H
