/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2021, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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

#include "taskInit.h"
#include "osThread.h"
#include "log_utils.h"
#include "app/taskADCS.h"
#include "app/taskSensors.h"

/**
 * App specific initialization routines
 * This function is called by taskInit
 *
 * @param params taskInit params
 */
void initAppHook(void *params)
{
    int t_ok;
    t_ok = osCreateTask(taskSensors, "sensors", SCH_TASK_SEN_STACK, NULL, 2, NULL);
    if(t_ok != 0) LOGE("simple-app", "Task sensors not created!");

    t_ok = osCreateTask(taskADCS, "adcs", SCH_TASK_SEN_STACK, NULL, 2, NULL);
    if(t_ok != 0) LOGE("simple-app", "Task adcs not created!");
}