/**
 * @file  os.h
 * @author Carlos Gonzalez Cortes
 * @author Ignacio Ibanez Aliaga
 * @date 2017
 * @copyright GNU Public License.
 *
 */

#ifndef _OS_H
#define _OS_H

#if __linux__
    #define portMAX_DELAY (uint32_t) 0xffffffff
    #define pdPASS				     1
    #define configMINIMAL_STACK_SIZE 1
    #define portBASE_TYPE	short
    #define ClrWdt(){};
#else
    #include "FreeRTOS.h"
    #include "task.h"
 	#include "queue.h"
 	#include "semphr.h"
    #include "timers.h"
#endif

#endif //_OS_H
