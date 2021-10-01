/**
 * @file  cmdOBC.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to the on board computer
 * (OBC) features.
 */

#ifndef CMD_OBC_H
#define CMD_OBC_H

#include <signal.h>
#include "suchai/config.h"
#include "suchai/cpu.h"

#ifdef SCH_HAVE_MALLOC
#include <malloc.h>
#endif

#include "TLE.h"
#include "suchai/osDelay.h"
#include "suchai/repoCommand.h"
#include "suchai/repoData.h"


/**
 * Register on board computer related (OBC) commands
 */
void cmd_obc_init(void);

/**
 * Print ID information such as software version, device id, name and node.
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int obc_ident(char* fmt, char* params, int nparams);

/**
 * Check if the OBC is working, usually blinking a led or printing a debug
 * message. In AVR32, NANOMIND and ESP32 blink the led indicated in the
 * parameter in LINUX just print a debug message. In RPI blink GPIO 16,17,22-27.
 * Default to led_number=0
 *
 * @param fmt Str. Parameters format "d"
 * @param params Str. [led_number] Optional
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int obc_debug(char *fmt, char *params, int nparams);

/**
 * Reset the watchdog timer
 * Use the argument to indicate the WDT to reset (if supporteed)
 *  0 -> Internal/CPU (default)
 *  1 -> External (if supported)
 *
 * @param fmt Str. Parameters format "%d"
 * @param params Str. [watchdog type] Optional
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int obc_reset_wdt(char *fmt, char *params, int nparams);

/**
 * Do a clean exit, closing repositories and freeing memory.
 * Then, usually exit(0) (GNU/Linxu) but microcontrollers (FreeRTOS) may simply reset
 *
 * @param fmt Str. Parameters format: ""
 * @param params ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK
 */
int obc_exit(char *fmt, char *params, int nparams);

/**
 * Reset (reboot) the system. Use arg to select the reboot type
 *  0: Soft-reset   (linux: exit(), others: reset, default)
 *  1: Hard-reset   (linux: sudo reboot, others: reset)
 *
 * @warning: In GNU/Linux if params is 1 then "sudo reboot"
 * is executed to reboot the system, if params is null or any other value then
 * just exit the application
 *
 * @param fmt Str. Parameters format: "%d"
 * @param params Str. [reset type] Optional
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int obc_reset(char *fmt, char *params, int nparams);

/**
 * Debug system memory
 * @note: Call mallinfo() if supported
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int obc_get_os_memory(char *fmt, char *params, int nparams);

/**
 * Set the system time
 *
 * @param fmt Str. Parameters format "%d"
 * @param params Str. Parameters as string "<unix timestamp>"
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int obc_set_time(char* fmt, char* params,int nparams);

/**
 * Show the system time in the format given. Param equal 0 is ISO format,
 * param equal 1 is UNIX TIME format
 *
 * @param fmt Str. Parameters format "%d"
 * @param params Str. Parameters as string "<format>"
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int obc_get_time(char *fmt, char *params, int nparams);

/**
 * Execute a shell command by calling system(). No check are performed over the
 * params, so use this command carefully. @see man system
 * @warning only available in GNU/Linux systems
 *
 * @param fmt str. Parameters format: "%s"
 * @param params  str. Parameters, the system command to execute: eg: "echo hello world"
 * @param nparams int. Number of parameters: 1
 * @return CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int obc_system(char* fmt, char* params, int nparams);

/**
 * Set current working directory
 *
 * @param fmt str. Parameters format: "%s"
 * @param params  str. <path/to/new/cwd>
 * @param nparams int. Number of parameters: 1
 * @return CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int obc_set_cwd(char* fmt, char* params, int nparams);

/**
 * Print current working directory
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int obc_get_cwd(char* fmt, char* params, int nparams);

/**
 * List files in path or CWD if used without parameters
 * @param fmt "%s"
 * @param params "[path]" Optional, list CWD if empty
 * @param nparams 1
 * @return CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int obc_ls(char* fmt, char* params, int nparams);

/**
 * Create a directory if not exists
 * @param fmt "%s"
 * @param params "<path>" Must be a valid path
 * @param nparams 1
 * @return CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int obc_mkdir(char* fmt, char* params, int nparams);


#endif /* CMD_OBC_H */
