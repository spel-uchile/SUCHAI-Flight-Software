//
// Created by matias on 22-03-18.
//

#ifndef SUCHAI_FLIGHT_SOFTWARE_INITI_H
#define SUCHAI_FLIGHT_SOFTWARE_INITI_H

#include "config.h"
#include "utils.h"
#include <stdio.h>
#include <signal.h>

/* system includes */
#include "repoData.h"
#include "repoCommand.h"

#include <csp/csp.h>
#include <csp/interfaces/csp_if_zmqhub.h>



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

