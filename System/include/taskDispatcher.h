/**
 * @file  commandRepoitory.h
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @author Carlos Gonzalez C - carlgonz@ug.uchile.cl
 * @date 2012
 * @copyright GNU GPL v3
 *
 * @id 0x1105
 *
 * This task implements the dispatcher. Reads commands from queue, determines
 * if the commands is executable, asks to command repository the function to
 * send to taskExecuter. It's an event driven task.
 */

#ifndef T_DISPATCHER_H
#define T_DISPATCHER_H

#include "../../SUCHAI_config.h"
#include "repoCommand.h"
#include "repoData.h"

void taskDispatcher(void *param);
int check_if_executable(DispCmd *newCmd);

#endif
