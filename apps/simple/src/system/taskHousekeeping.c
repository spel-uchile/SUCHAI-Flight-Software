/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2020, Tomas Opazo Toro, tomas.opazo.t@gmail.com
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

#include "app/system/taskHousekeeping.h"

static const char *tag = "Housekeeping";

void taskHousekeeping(void *param)
{
    LOGI(tag, "Started");

    portTick delay_ms    = 1000;            //Task period in [ms]
    unsigned int elapsed_sec = 1;           // Seconds counter
    unsigned int _1min_check = 60;         //10[s] condition
    unsigned int _1hour_check = 60*60;      //01[h] condition

    portTick xLastWakeTime = osTaskGetTickCount();

    while(1)
    {
        osTaskDelayUntil(&xLastWakeTime, delay_ms); //Suspend task
        elapsed_sec += delay_ms / 1000; //Update seconds counts

        /* 1 second actions */
        dat_set_system_var(dat_rtc_date_time, (int) time(NULL));

        //  Debug command
        if(log_lvl > LOG_LVL_DEBUG)
        {
            cmd_t *cmd_dbg = cmd_get_str("obc_debug");
            cmd_add_params_var(cmd_dbg, 0);
            cmd_send(cmd_dbg);
        }

        /* 10 sec actions */
        // Update status vars
        if ((elapsed_sec % _1min_check) == 0)
        {
            cmd_t *cmd_update;
            cmd_update = cmd_get_str("obc_get_sensors");
            cmd_send(cmd_update);
            cmd_update = cmd_get_str("obc_update_status");
            cmd_send(cmd_update);
        }

        /* 1 hours actions */
        if((elapsed_sec % _1hour_check) == 0)
        {
            cmd_t *cmd_1h = cmd_build_from_str("drp_add_hrs_alive 1");
            cmd_send(cmd_1h);
        }
    }
}
