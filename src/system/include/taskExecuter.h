/**
 * @file  taskExecuter.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This task implements the executer module. Waits a message from dispatcher to
 * obtain the function and parameter to execute. When the function ends, send a
 * message to dispatcher with the result of the execution
 */

#ifndef T_EXECUTER_H
#define T_EXECUTER_H

#include "globals.h"
#include "config.h"
#include "utils.h"

#include "osQueue.h"

#include "repoCommand.h"

void taskExecuter(void *param);

#endif
