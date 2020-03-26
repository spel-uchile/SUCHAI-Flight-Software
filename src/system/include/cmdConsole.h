/**
 * @file  cmdConsole.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @date 2020
 * @copyright GNU Public License.
 *
 * This header contains commands related with serial console functions
 */

#ifndef CMD_CONSOLE_H
#define CMD_CONSOLE_H


#include "utils.h"
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

/**
 * Set the log verbosity level and current node to send logs
 *  - level can be 0 to 5 @see log_level_t
 *  - node can be -1 to use stdout, or > 0 to to send log using CSP to <node>
 *
 * @param fmt Str. Parameters format "%d %d"
 * @param params Str. Parameters as string "<level> <node>"
 * @param nparams Int. Number of parameters 2
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 *
 * Example
 * @code
 * //Set log level to DEBUG using STDOUT
 * log_set 4 -1
 * con_set_logger("%d %d", "4 -1", 2);
 *
 * #Set log level to INFO and send log to CSP node 10
 * log_set 3 10
 * con_set_logger("%d %d", "3 10", 2);
 * @endcode
 */
int con_set_logger(char *fmt, char *params, int nparams);


#endif /* CMD_CONSOLE_H */
