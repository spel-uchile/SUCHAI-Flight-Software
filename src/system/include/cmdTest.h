/**
 * @file  taskFlightPlan.h
 * @author Matias Ramirez  - nicoram.mt@gmail.com
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2018
 * @copyright GNU GPL v3
 *
 * This task implements a listener that send commands scheduled in the flight
 * plan
 */

#ifndef SUCHAI_FLIGHT_SOFTWARE_CMDTEST_H
#define SUCHAI_FLIGHT_SOFTWARE_CMDTEST_H

#include "config.h"
#include "repoCommand.h"
#include "osDelay.h"

/**
 * This function registers the list of command in the system, initializing the
 * functions array. This function must be called at every system start up.
 */
void cmd_test_init(void);

/**
 * This function makes a sleep for the time given as a parameter
 *
 * @param fmt Str. Parameters format "%d %ul"
 * @param params Str. Parameters as string "<sleep_time> <init_time> "
 * @param nparams Int. Number of parameters 2
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int test_mult_exe(char* fmt, char* params, int nparams);


/**
 * This function makes a test of the arguments given to flight plan, to check a good parse of them
 *
 * @param fmt Str. Parameters format "%d %s %d"
 * @param params Str. Parameters as string "<int> <str> <int> "
 * @param nparams Int. Number of parameters 3
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int test_fp_params(char* fmt, char* params,int nparams);


#endif //SUCHAI_FLIGHT_SOFTWARE_CMDTEST_H
