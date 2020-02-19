/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2020, Ignacio Ibanez Aliaga, ignacio.ibanez@usach.cl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "osDelay.h"

portTick osDefineTime(uint32_t mseconds)
{
    //use time
    return (portTick)mseconds*1000;
}

portTick osTaskGetTickCount(void)
{
    //calculate time
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    //return time in microseconds
    return (portTick)(time.tv_sec*1000000+time.tv_nsec/1000);
}

void osDelay(uint32_t mseconds)
{
    //transform to microseconds
    usleep(mseconds*1000);
}

void osTaskDelayUntil(portTick *lastTime, uint32_t mseconds)
{
    //calculate time left
    portTick c_usec = osTaskGetTickCount();   // Current ticks
    portTick d_usec = c_usec - *lastTime;     // Delta ticks
    portTick s_usec = osDefineTime(mseconds); // Sleep ticks

    // Return if more than desired milli seconds have passed
    if(d_usec > s_usec)
    {
        // Tag last delay ticks
        *lastTime = osTaskGetTickCount()-(d_usec-s_usec);
        return;
    }

    // Delay left ticks
    uint32_t s_msec = (s_usec - d_usec)/1000;
    osDelay(s_msec);

    // Tag last delay ticks
    *lastTime = osTaskGetTickCount();
}