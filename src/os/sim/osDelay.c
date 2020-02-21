/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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

static portTick ticks_us;
pthread_cond_t delay_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t tick_mutex = PTHREAD_MUTEX_INITIALIZER;

portTick osDefineTime(uint32_t mseconds)
{
    // Translate user time (ms) to system ticks (us)
    return (portTick)mseconds*1000;
}

portTick osTaskGetTickCount(void)
{
    // Return current system ticks (us)
    return (portTick)(ticks_us);
}

void osTaskSetTickCount(portTick new_tick_us)
{
    // Set current tick (us) and signal the change to waiting threads
    pthread_mutex_lock(&tick_mutex);
    ticks_us = new_tick_us;
    pthread_cond_broadcast(&delay_cond);
    pthread_mutex_unlock(&tick_mutex);
}

void osDelay(uint32_t mseconds)
{
    // Use the conditional variable to block the thread for the necessary ticks.
    pthread_mutex_lock(&tick_mutex);
    portTick max_ticks_us = mseconds*1000 + ticks_us;
    while(ticks_us < max_ticks_us)
        pthread_cond_wait(&delay_cond, &tick_mutex);
    pthread_mutex_unlock(&tick_mutex);
}

void osTaskDelayUntil(portTick *lastTime, uint32_t mseconds)
{
    // Calculate time left
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
