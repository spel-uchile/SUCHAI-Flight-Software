/**
 * @file  FreeRTOS/delay.c
 * @author Ignacio Iba�ez Aliaga 
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * Creation of functions related to time for operating systems Linux and FreeRTOS
 * 
 */
#include "../include/osDelay.h"

void osDelay(long milliseconds){
    vTaskDelay(milliseconds);
}

portTick osDefineTime(long delayms){
    return delayms/portTICK_RATE_MS;
}

portTick osTaskGetTickCount(){
	return xTaskGetTickCount();
}

void osTaskDelayUntil(portTick* lastTime, portTick delay_ticks){
	vTaskDelayUntil(lastTime, delay_ticks);
}