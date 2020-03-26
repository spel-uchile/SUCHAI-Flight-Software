/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020 Matias Ramirez Martinez, nicoram.mt@gmail.com
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

#include "taskFlightPlan.h"

static const char *tag = "FlightPlan"; 

void taskFlightPlan(void *param)
{

    LOGI(tag, "Started");
    char command[SCH_CMD_MAX_STR_PARAMS];
    char args[SCH_CMD_MAX_STR_PARAMS];
    int executions;
    int period;
    int i;
    portTick delay_ms = 1000;          //Task period in [ms]
    portTick xLastWakeTime = osTaskGetTickCount();

    time_t elapsed_sec;   // Seconds counter

    while(1)
    {
        osTaskDelayUntil(&xLastWakeTime, delay_ms); //Suspend task
        // Get next command in the flight plan, if any
        elapsed_sec = dat_get_time();
        int rc = dat_get_fp((int)elapsed_sec, command, args, &executions, &period);
        if(rc == -1)
            continue;

        LOGI(tag, "Command: %s", command);
        LOGI(tag, "Arguments: %s", args);
        LOGI(tag, "Executions: %d", executions);
        LOGI(tag, "Period: %d", period);

        // Send the command for N execution
        dat_set_system_var(dat_fpl_last, (int) elapsed_sec);
        for(i=0; i < executions; i++)
        {
            cmd_t *new_cmd = cmd_get_str(command);
            cmd_add_params_str(new_cmd, args);
            cmd_send(new_cmd);
        }
    }
}