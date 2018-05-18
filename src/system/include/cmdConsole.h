/**
 * @file  cmdConsole.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @date 2018
 * @copyright GNU Public License.
 *
 * This header contains commands related with serial console functions
 */

#ifndef CMD_CONSOLE_H
#define CMD_CONSOLE_H

#ifdef LINUX
    #include "csp/csp.h"
#endif

#include "repoCommand.h"

/**
 * Registers the console commands in the system
 */
void cmd_console_init(void);

/**
 * Display a debug message
 *
 * @param fmt Str. Parameters format "%s"
 * @param params Str. Parameters as string "test"
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int con_debug_msg(char *fmt, char *params, int nparams);

/**
 * Show the list of available commands, id and parameters format
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int con_help(char *fmt, char *params, int nparams);

#endif /* CMD_CONSOLE_H */
