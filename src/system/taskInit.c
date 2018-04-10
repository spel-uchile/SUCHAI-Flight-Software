/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2017, Matias Ramirez Martinez, nicoram.mt@gmail.com
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

static const char *tag = "taskInit";

void taskInit(void *param)
{
    os_thread threads_id = (os_thread)param;

    unsigned short task_memory = 15*256;

    LOGD(tag, "Initializing Tasks ...");

    /* Creating clients tasks */
    osCreateTask(taskConsole, "console", task_memory, NULL, 2, &threads_id[3]);
#if SCH_HK_ENABLED
    // FIXME: This memory values seems not work on nanomind (tested with 10)
    osCreateTask(taskHousekeeping, "housekeeping", task_memory, NULL, 2, &threads_id[4]);
#endif
#if SCH_COMM_ENABLE
    osCreateTask(taskCommunications, "comm", task_memory, NULL,2, &threads_id[5]);
#endif
#if SCH_FP_ENABLED
    osCreateTask(taskFlightPlan,"flightplan", task_memory,NULL,2,&threads_id[6]);
#endif

    //TODO: Task has to kill himself
}