/**
 * @file  Linux/delay.c
 * @author Ignacio Ibañez Aliaga 
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * Creation of functions related with time for systems operating Linux.
 * 
 */

#include "../include/osDelay.h"

void osDelay(long milisegundos){
    usleep(milisegundos);
}

portTick osDefineTime(long delayms){
    /* FIXME: provide a proper implementation and doc*/
    return (portTick) delayms*1000;
}

portTick osTaskGetTickCount(){
	return 0;
}

void osTaskDelayUntil(portTick* lastTime, portTick delay_ticks){
	usleep(delay_ticks);
}