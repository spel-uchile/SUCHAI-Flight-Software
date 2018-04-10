/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2017, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2017, Ignacio Ibanez Aliaga, ignacio.ibanez@usach.cl
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

#include "osThread.h"

/**
 * create a task in Linux as thread
 */
void osCreateTask(void (*functionTask)(void *), char* name, unsigned short size, void * parameters, unsigned int priority, os_thread* thread){
    pthread_create(thread , NULL , (void *)(*functionTask) , parameters);

    // Set Real Time scheduling and thread priority
    // Only with proper permissions
    const struct sched_param _priority = {(int) priority};
    if(pthread_setschedparam(*thread, SCHED_FIFO, &_priority) != 0)
        printf("[Warning] (%s) Failed to assign task priority, try as root\n", name);
}

void osTaskDelete(void *task_handle){};

