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


#include "suchai/log_utils.h"
#include "suchai/repoCommand.h"

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
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int con_debug_msg(char *fmt, char *params, int nparams);

/**
 * Show the list of available commands, id and parameters format
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int con_help(char *fmt, char *params, int nparams);

/**
 * Set the log verbosity level, mode, and node. If mode is LOG_MODE_CSP then logs are send to <node>
 *  - level can be 0 to 5 @see log_level_t
 *  - mode can be 0 to 3 @see log_mode_t
 *  - node can be the destination to send log using CSP, or the file suffix when using file mode.
 *
 * @param fmt Str. Parameters format "%d %d %d"
 * @param params Str. Parameters as string "<level> <mode> <node>"
 * @param nparams Int. Number of parameters 3
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 *
 * Example
 * @code
 * //Set log level to DEBUG (1) using STDOUT (0)
 * log_set 5 0 0
 * con_set_logger("%d %d %d", "4 0 0", 2);
 *
 * #Set log level to INFO (4) and send log to a FILE (1) named suchai_3.log
 * log_set 4 1 3
 * con_set_logger("%d %d", "4 1 3", 3);
 *
 * #Set log level to RESULTS (1) and send log to CSP (2) node 10
 * log_set 1 2 10
 * con_set_logger("%d %d", "1 2 10", 3);
 * @endcode
 */
int con_set_logger(char *fmt, char *params, int nparams);


#endif /* CMD_CONSOLE_H */
