/**
 * @file  taskFlightPlan.h
 * @author Matias Ramirez  - nicoram.mt@gmail.com
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2017
 * @copyright GNU GPL v3
 *
 * This task implements a listener that send commands scheduled in the flight
 * plan
 */

#ifndef T_FLIGHTPLAN_H
#define T_FLIGHTPLAN_H

#include <utils.h>
#include <stdlib.h>
#include "config.h"
#include "osDelay.h"
#include "repoCommand.h"
#include "repoData.h"

#ifdef AVR32
    #include <avr32/io.h>
    #if __GNUC__
    #  include "intc.h"
    #endif
    #include "board.h"
    #include "compiler.h"
    #include "rtc.h"
    #include "usart.h"
    #include "pm.h"
    #include <time.h>
#endif

void taskFlightPlan(void *param);

/* TODO: Documentation required */
/**
 *
 * @param day
 * @param month
 * @param year
 * @param hour
 * @param min
 * @param sec
 * @return
 */
int date_to_unixtime(int day, int month, int year, int hour, int min, int sec);

#endif //T_FLIGHTPLAN_H
