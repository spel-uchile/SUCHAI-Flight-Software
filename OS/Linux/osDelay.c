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

void osDelay(long milliseconds){
    //transform in microseconds
    usleep(milliseconds*1000);
}

portTick osDefineTime(long delayms){
    //use time
    return (portTick) delayms;
}

portTick osTaskGetTickCount(){
    //calculate time
    struct timeval time;
    gettimeofday(&time,0);
    //return time in microseconds
	return (int)(time.tv_sec*1000000 + time.tv_usec);
}

void osTaskDelayUntil(portTick* lastTime, portTick delay_ticks){

    //get time
    struct timeval current;
    gettimeofday(&current,0);

    //calculate time left
    portTick u_current = current.tv_sec*1000000+current.tv_usec;
    if((u_current-*lastTime)>=delay_ticks*1000){
        return;
    }
    //micro delay
	usleep(delay_ticks*1000-(u_current-*lastTime));

    gettimeofday(&current,0);
    *lastTime = current.tv_sec*1000000+current.tv_usec;

}