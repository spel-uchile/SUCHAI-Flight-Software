/**
 * @file  taskDispatcher.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This task implements the dispatcher. Reads commands from queue, determines
 * if the commands is executable, asks to command repository the function to
 * send to taskExecuter. It's an event driven task.
 */

#ifndef T_DISPATCHER_H
#define T_DISPATCHER_H

#include "config.h"
#include "globals.h"

#include "osQueue.h"

#include "repoCommand.h"
#include "repoData.h"

void taskDispatcher(void *param);
int check_if_executable(cmd_t *newCmd);

#endif
