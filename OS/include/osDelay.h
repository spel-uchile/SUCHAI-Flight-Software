/**
 * @file  delay.h
 * @author Ignacio Ibanez Aliaga
 * @author Carlos Gonzalez Cortes
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * Functions related to time for operating systems Linux and FreeRTOS
 * 
 */

#ifndef _OS_DELAY_H_
#define _OS_DELAY_H_

#include "os.h"
#include "time.h"
#include "stdint.h"

#if __linux__
    #include <unistd.h>
    #include <sys/time.h>
 	typedef  __useconds_t portTick; // Always useconds (uint32_t)
#else
	#include "freertos/FreeRTOS.h"
	#include "freertos/task.h"
	typedef portTickType portTick;  // Always ticks counts (int)
#endif

/**
 * Define time in milliseconds to tick type.
 * @param mseconds uint32_t. Time in milliseconds
 * @return portTick
 */
portTick osDefineTime(uint32_t mseconds);

/**
 * Get current tick count
 * @return portTick
 */
portTick osTaskGetTickCount(void);

/**
 * Delay task execution
 * @param milliseconds uint32_t. Milliseconds to sleep
 */
void osDelay(uint32_t mseconds);

/**
 * Delay task execution in given millisecond since last delay. Use this function
 * to fine control periodic tasks.
 * @param lastTime portTick. Tick count of the last delay
 * @param milliseconds Long. Milliseconds to sleep.
 */
void osTaskDelayUntil(portTick *lastTime, uint32_t mseconds);

#endif
