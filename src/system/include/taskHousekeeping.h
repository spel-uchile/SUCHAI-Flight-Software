/**
 * @file  taskHousekeeping.h
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2019
 * @copyright GNU GPL v3
 *
 * This task implements a listener, that sends commands at periodical times.
 */

#ifndef T_HOUSEKEEPING_H
#define T_HOUSEKEEPING_H

#include <stdlib.h>
#include <stdint.h>
#include "config.h"

//FIXME: Task should not depend on drivers
#if SCH_COMM_ENABLE
    #include "csp/csp.h"
#endif
#ifdef AVR32
    #include "asf.h"
    #include "init.h"
#endif
#ifdef NANOMIND
    #include "drivers.h"
//     #include "led.h"
//     #include "wdt.h"
#endif

#include "globals.h"

#include "osQueue.h"
#include "osDelay.h"

#include "repoCommand.h"

void taskHousekeeping(void *param);

#endif //T_HOUSEKEEPING_H




