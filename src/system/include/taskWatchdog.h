/**
 * @file  taskWatchdog.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This task implements a client that controls watchdog timers.
 */

#ifndef T_WDT_H
#define T_WDT_H

#include <stdlib.h>
#include <stdint.h>

#include "config.h"
#include "globals.h"

#include "osQueue.h"
#include "osDelay.h"

#include "repoCommand.h"
#include "repoData.h"

void taskWatchdog(void *param);

#endif //T_WDT_H




