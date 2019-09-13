/**
 * @file  cmdFP.h
 * @author Matias Ramirez M  - nicoram.mt@gmail.com
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2019
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to the flight plan
 */


#ifndef CMD_FLIGHTPLAN_H
#define CMD_FLIGHTPLAN_H

#include "config.h"

#include "repoCommand.h"
#include "repoData.h"

/**
 * This function registers the list of command in the system, initializing the
 * functions array. This function must be called at every system start up.
 */
void cmd_fp_init(void);

/**
 * Set a command to be added in the flight plan
 *
 * Each argument for the command must be separated by a comma ',' character
 * instead os a space ' ' character.
 *
 * @param fmt Str. Parameters format "%d %d %d %d %d %d %s %s %d %d"
 * @param params Str. Parameters as string
 *  "<day> <month> <year> <hour> <min> <sec> <command> <args> <executions> <period>"
 * @param nparams Int. Number of parameters 10
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int fp_set(char *fmt, char *params, int nparams);

/**
 * Set a command to be added in the flight plan
 *
 * Each argument for the command must be separated by a comma ',' character
 * instead os a space ' ' character.
 *
 * @param fmt Str. Parameters format "%d %s %s %d %d"
 * @param params Str. Parameters as string
 *  "<unixtime> <command> <args> <executions> <period>"
 * @param nparams Int. Number of parameters 5
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int fp_set_unix(char *fmt, char *params, int nparams);

/**
 * Delete a command in the flight plan by the execution time
 *
 * @param fmt Str. Parameters format "%d %d %d %d %d %d"
 * @param params Str. Parameters as string "<day> <month> <year> <hour> <min> <sec>"
 * @param nparams Int. Number of parameters 6
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int fp_delete(char* fmt, char* params, int nparams);

/**
 * Delete a command in the flight plan by the execution unix time
 *
 * @param fmt Str. Parameters format "%d"
 * @param params Str. Parameters as string "<unix_time>"
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int fp_delete_unix(char* fmt, char* params, int nparams);

/**
 * Show all the commands presents in the flight plan
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int fp_show(char* fmt, char* params, int nparams);

/**
 * Reset the current flight plan, in other words, create a new empty flight plan
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int fp_reset(char* fmt, char* params, int nparams);

/**
 * Test that the command parameters are well read
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int test_fp_params(char* fmt, char* params,int nparams);

#endif //CMD_FLIGHTPLAN_H
