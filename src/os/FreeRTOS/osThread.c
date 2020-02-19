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

#include "osThread.h"

/**
 * create a task in FreeRTOS
 */
int osCreateTask(void (*functionTask)(void *), char* name, unsigned short size, void * parameters, unsigned int priority, os_thread *thread)
{
#ifdef AVR32
    // FreeRTOS 7.0.0
    portBASE_TYPE created = xTaskCreate((*functionTask), (signed char*)name, size, parameters, priority, NULL);
#else
    // FreeRTOS > 8.0.0
    BaseType_t created = xTaskCreate((*functionTask), name, size, parameters, priority, NULL);
#endif
    return created == pdPASS ? 0 : 1;
}

void osTaskDelete(void *task_handle)
{
    vTaskDelete(task_handle);
}
