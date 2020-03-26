/**
 * @file init.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Matias Ramirez M - nicoram.mt@gmail.com
 * @date 2020
 * @copyright GNU GPL v3
 *
 * Initialization functions
 */

#ifndef SUCHAI_FLIGHT_SOFTWARE_INITI_H
#define SUCHAI_FLIGHT_SOFTWARE_INITI_H

#include "config.h"
#include "utils.h"
#include <stdio.h>
#include <signal.h>

/* system includes */
#include "repoData.h"
#include "repoCommand.h"

#if SCH_COMM_ENABLE
#include <csp/csp.h>
#include <csp/interfaces/csp_if_zmqhub.h>
#endif


/**
 * Performs a clean exit
 *
 * @param signal Int. Signal number
 */
void on_close(int signal);

/**
 * Performs initialization actions
 */
void on_reset(void);

#endif //SUCHAI_FLIGHT_SOFTWARE_INITI_H

