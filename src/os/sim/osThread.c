/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2019, Ignacio Ibanez Aliaga, ignacio.ibanez@usach.cl
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
int osCreateTask(void (*functionTask)(void *), char* name, unsigned short size, void * parameters, unsigned int priority, os_thread* thread){

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, size);

    int created = pthread_create(thread , &attr , (void *)(*functionTask) , parameters);
    pthread_setname_np(*thread, name);

    // Set Real Time scheduling and thread priority
    // Only with proper permissions
    const struct sched_param _priority = {(int) priority};
    if(pthread_setschedparam(*thread, SCHED_FIFO, &_priority) != 0)
        printf("[WARN] (%s) Failed to assign task priority, try as root\n", name);

    pthread_attr_destroy(&attr);

    return created;
}

void osTaskDelete(void *task_handle)
{
    pthread_t thread;
    if(task_handle == NULL)
        thread = pthread_self();
    else
        thread = *(pthread_t *)task_handle;

    printf("[INFO] Canceling thread %lu\n", thread);
    int s = pthread_cancel(thread);
    if (s != 0) printf("[WARN] Failed to cancel thread %lu\n", thread);
}

