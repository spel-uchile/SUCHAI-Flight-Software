/**
 * @file  commandRepoitory.h
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @author Carlos Gonzalez C - carlgonz@ug.uchile.cl
 * @date 2012
 * @copyright GNU GPL v3
 *
 * @id 0x1106
 *
 * This task implements the executer module. Waits a message from dispatcher to
 * obtain the function and parameter to execute. When the function ends, send a
 * message to dispatcher with the result of the execution
 */

#include "SUCHAI_config.h"

void taskExecuter(void *param);
