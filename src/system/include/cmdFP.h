/**
 * @file  cmdFP.h
 * @author Matias Ramirez M  - nicoram.mt@gmail.com
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2017
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to the flight plan
 */


#ifndef CMD_FLIGHTPLAN_H
#define CMD_FLIGHTPLAN_H

#include "data_storage.h"

#include "repoCommand.h"

/* TODO: Documentation required */  //Done
/**
 * This function registers the list of command in the system, initializing the
 * functions array. This function must be called at every system start up.
 */
void cmd_fp_init(void);

/**
 * Set a command to be added in the flight plan
 *
 * @param fmt Str. Parameters format "%d %d %d %d %d %d %s %s %d %d"
 * @param params Str. Parameters as string "<day> <month> <year> <hour> <min> <sec> <command> <args> <executions> <periodical>"
 * @param nparams Int. Number of parameters 11
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int fp_set(char *fmt, char *params, int nparams);

/**
 * Delete a command in the flight plan by the execution time
 *
 * @param fmt Str. Parameters format "%d %d %d %d %d %d"
 * @param params Str. Parameters as string "<day> <month> <year> <hour> <min> <sec>"
 * @param nparams Int. Number of parameters 7
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int fp_delete(char* fmt, char* params, int nparams);

/**
 * Show all the commands presents in the flight plan
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int fp_show(char* fmt, char* params, int nparams);

/**
 * Reset the current flight plan, in other words, create a new empty flight plan
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int fp_reset(char* fmt, char* params, int nparams);

#endif //CMD_FLIGHTPLAN_H
