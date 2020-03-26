/**
 * @file  taskFlightPlan.h
 * @author Matias Ramirez  - nicoram.mt@gmail.com
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This task implements a listener that send commands scheduled in the flight
 * plan
 */

#ifndef T_FLIGHTPLAN_H
#define T_FLIGHTPLAN_H

#include <utils.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"

#include "osDelay.h"
#include "osQueue.h"

#include "repoCommand.h"
#include "repoData.h"

void taskFlightPlan(void *param);

#endif //T_FLIGHTPLAN_H
