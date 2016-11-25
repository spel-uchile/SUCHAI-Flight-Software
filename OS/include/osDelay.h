/**
 * @file  delay.h
 * @author Ignacio Ibañez Aliaga 
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * Creation of functions related to time for operating systems Linux and FreeRTOS
 * 
 */

#ifndef _OS_DELAY_H_
#define _OS_DELAY_H_

#include "../../SUCHAI_config.h"

#if __linux__
    #include <unistd.h> 
 	typedef int portTick;
#else
 	typedef portTickType portTick;
#endif

void osDelay(long milisegundos);
portTick osDefineTime(long delayms);
portTick osTaskGetTickCount();
void osTaskDelayUntil(portTick* lastTime, portTick delay_ticks);

#endif
