/**
 * @file  cmdFP.h
 * @author Matias Ramirez M  - nicoram.mt@gmail.com
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2020
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
 * Add a command to the flight plan by date and time
 *
 * @param fmt Str. Parameters format "%d %d %d %d %d %d %d %d %s %n"
 * @param params Str. Parameters as string
 *  "<day> <month> <year> <hour> <min> <sec> <executions> <period> <command> [args]"
 * @param nparams Int. Number of parameters 10
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int fp_set(char *fmt, char *params, int nparams);

/**
 * Add a command to the flight plan by unix time
 *
 * @param fmt Str. Parameters format "%d %d %d %s %n"
 * @param params Str. Parameters as string
 *  "<unixtime> <executions> <period> <command> [args]"
 * @param nparams Int. Number of parameters 5
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int fp_set_unix(char *fmt, char *params, int nparams);

/**
 * Add a command to the flight plan to be executed <seconds> seconds after
 * current unix time.
 *
 * @param fmt Str. Parameters format "%d %d %d %s %n"
 * @param params Str. Parameters as string
 *  "<seconds> <executions> <period>  <command> [args]"
 * @param nparams Int. Number of parameters 5
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int fp_set_dt(char *fmt, char *params, int nparams);

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
