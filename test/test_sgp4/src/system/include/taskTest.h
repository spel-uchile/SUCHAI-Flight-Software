//
// Created by carlgonz 2020-02-24
//

#ifndef SUCHAI_FLIGHT_SOFTWARE_TASKTEST_H
#define SUCHAI_FLIGHT_SOFTWARE_TASKTEST_H

#include <math.h>

#include "config.h"
#include "globals.h"

#include "osDelay.h"
#include "repoCommand.h"
#include "cmdOBC.h"

int _obc_prop_tle_test(char *fmt, char *params, int nparams); // For testing
int _obc_prop_tle_cmp(char *fmt, char *params, int nparams); // For testing
void taskTest(void* param);

#endif //SUCHAI_FLIGHT_SOFTWARE_TASKTEST_H
