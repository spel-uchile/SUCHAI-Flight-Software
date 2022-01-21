/**
 * @file  cmdTestTLE.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2021
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to testing the sgp4
 * features.
 */

// Common includes for apps
#include "suchai/config.h"
#include "suchai/repoData.h"
#include "suchai/repoCommand.h"

#include "suchai/osDelay.h"
#include "suchai/cmdOBC.h"
#include <math.h>

void cmd_tle_test_init(void);
int _test_tle_prop(char *fmt, char *params, int nparams); // For testing
int _test_tle_cmp(char *fmt, char *params, int nparams); // For testing

