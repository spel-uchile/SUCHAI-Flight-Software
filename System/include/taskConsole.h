/**
 * @file  taskConsole.h
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @author Carlos Gonzalez C - carlgonz@ug.uchile.cl
 * @date 2012
 * @copyright GNU GPL v3
 *
 * @id 0x1102
 *
 * This task implements a listener that works as a serial console.
 */

#ifndef T_COSOLE_H
#define T_CONSOLE_H


#include "FreeRTOS.h"
#include "queue.h"
//#include "task.h"

#include "console.h"
#include "cmdIncludes.h"

void taskConsole(void *param);

#endif //T_CONSOLE_H
