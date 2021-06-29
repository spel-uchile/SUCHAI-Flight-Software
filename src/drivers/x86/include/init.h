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

#include <stdio.h>
#include <signal.h>

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

