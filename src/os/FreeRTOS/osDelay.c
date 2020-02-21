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