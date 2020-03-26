/**
 * @file  taskConsole.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This task implements a client that works as a serial console.
 */

#ifndef T_COSOLE_H
#define T_CONSOLE_H

#include <stdio.h>

#include "config.h"
#include "globals.h"
#include "utils.h"
#ifdef LINUX
#include "linenoise.h"
#endif

#include "osQueue.h"
#include "osDelay.h"

#include "repoCommand.h"

void taskConsole(void *param);
int console_init(void);
int console_read(char *buffer, int len);

#endif //T_CONSOLE_H
