//
// Created by matias on 05-06-18.
//

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
 * Test that the command parameters are well read
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int test_fp_params(char* fmt, char* params,int nparams);

int test_mult_exe(char* fmt, char* params, int nparams);

int test_print_char(char* fmt, char* params, int nparams);

int test_print_int(char* fmt, char* params, int nparams);

int test_print_char2(char* fmt, char* params, int nparams);

#endif //SUCHAI_FLIGHT_SOFTWARE_CMDTEST_H
