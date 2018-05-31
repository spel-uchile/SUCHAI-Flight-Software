/**
 * @file  taskConsole.h
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @author Carlos Gonzalez C - carlgonz@ug.uchile.cl
 * @date 2018
 * @copyright GNU GPL v3
 *
 * This task implements a client that works as a serial console.
 */

#ifndef T_COSOLE_H
#define T_CONSOLE_H

#include "config.h"

#include <ctype.h>

#include "osQueue.h"
#include "osDelay.h"

#include "globals.h"
#include "utils.h"

#include "repoCommand.h"

void taskConsole(void *param);
int console_init(void);
int console_read(char *buffer, int len);

#endif //T_CONSOLE_H
