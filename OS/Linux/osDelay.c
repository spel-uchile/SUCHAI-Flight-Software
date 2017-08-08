/**
 * @file  Linux/delay.c
 * @author Ignacio Ibañez Aliaga
 * @author Carlos Gonzalez Cortes
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * Functions related with time for systems operating Linux.
 * 
 */

#include "osDelay.h"

portTick osDefineTime(uint32_t mseconds){
    //use time
    return (portTick)mseconds*1000;
}

portTick osTaskGetTickCount(void){
    //calculate time
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    //return time in microseconds
    return (portTick)(time.tv_sec*1e+6 + time.tv_nsec*1e-3);
}

void osDelay(uint32_t mseconds){
    //transform to microseconds
    usleep(mseconds*1000);
}

void osTaskDelayUntil(portTick *lastTime, uint32_t mseconds){
    //calculate time left
    portTick c_usec = osTaskGetTickCount();   // Current ticks
    portTick d_usec = c_usec - *lastTime;     // Delta ticks
    portTick s_usec = osDefineTime(mseconds); // Sleep ticks

    // Return if more than desired milli seconds have passed
    if(d_usec > s_usec){
        return;
    }

    // Delay left ticks
    uint32_t s_msec = (s_usec - d_usec)/1000;
    osDelay(s_msec);

    // Tag last delay ticks
    *lastTime = osTaskGetTickCount();
}