//
// Created by matias on 22-03-18.
//

#ifndef SUCHAI_FLIGHT_SOFTWARE_INITI_H
#define SUCHAI_FLIGHT_SOFTWARE_INITI_H

#include "utils.h"
#include "config.h"
#include <stdio.h>
#include <signal.h>

/* system includes */
#include "repoData.h"
#include "repoCommand.h"

#ifdef LINUX
    #include <csp/csp.h>
    #include <csp/interfaces/csp_if_zmqhub.h>
#endif

#ifdef AVR32
    #include "asf.h"
    #include "util.h"
    #include <time.h>
    #include <avr32/io.h>
    #include "intc.h"
    #include "board.h"
    #include "compiler.h"
    #include "rtc.h"
    #include "usart.h"
    #include "pm.h"

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

