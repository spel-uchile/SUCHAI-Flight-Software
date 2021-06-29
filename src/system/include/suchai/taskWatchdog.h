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

#include "suchai/config.h"
#include "globals.h"

#include "suchai/osQueue.h"
#include "suchai/osDelay.h"

#include "suchai/repoCommand.h"
#include "suchai/repoData.h"

void taskWatchdog(void *param);

#endif //T_WDT_H




