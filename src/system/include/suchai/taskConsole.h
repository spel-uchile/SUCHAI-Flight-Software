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

#include "suchai/config.h"
#include "globals.h"
#include "suchai/log_utils.h"
#ifdef LINUX
#include "linenoise.h"
#endif

#include "suchai/osQueue.h"
#include "suchai/osDelay.h"

#include "suchai/repoCommand.h"

void taskConsole(void *param);
int console_init(void);
int console_read(char *buffer, int len);
void console_set_prompt(char *prompt);

#endif //T_CONSOLE_H
