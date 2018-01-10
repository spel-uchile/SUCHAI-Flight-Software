/**
 * @file  FreeRTOS/delay.c
 * @author Ignacio Ibanez Aliaga
 * @author Carlos Gonzalez Cortes
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * Creation of functions related to time for operating systems Linux and FreeRTOS
 * 
 */

#include "osDelay.h"

void osDelay(uint32_t mseconds){
    portTick ticks = mseconds/portTICK_RATE_MS;
    vTaskDelay(ticks);
}

portTick osDefineTime(uint32_t mseconds){
    return mseconds/portTICK_RATE_MS;
}

portTick osTaskGetTickCount(void){
	return xTaskGetTickCount();
}

void osTaskDelayUntil(portTick *lastTime, uint32_t mseconds){
    portTick ticks = osDefineTime(mseconds);
	vTaskDelayUntil(lastTime, ticks);
}