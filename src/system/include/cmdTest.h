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


int test_mult_exe(char* fmt, char* params, int nparams);


#endif //SUCHAI_FLIGHT_SOFTWARE_CMDTEST_H
