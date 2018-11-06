/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2018, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2018, Tomas Opazo Toro, tomas.opazo.t@gmail.com
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

#include "taskHousekeeping.h"

static const char *tag = "Housekeeping";

void taskHousekeeping(void *param)
{
    LOGI(tag, "Started");

    portTick delay_ms    = 1000;            //Task period in [ms]

    unsigned int elapsed_sec = 0;           // Seconds counter
    unsigned int _10sec_check = 10;     //10[s] condition
    unsigned int _10min_check = 10*60;  //10[m] condition
    unsigned int _1hour_check = 60*60;  //01[h] condition

    portTick xLastWakeTime = osTaskGetTickCount();

    while(1)
    {

        osTaskDelayUntil(&xLastWakeTime, delay_ms); //Suspend task
        elapsed_sec += delay_ms/1000; //Update seconds counts

        // Debug command
        cmd_t *cmd_dbg = cmd_get_str("debug_obc");
        cmd_add_params_var(cmd_dbg, 0);
        cmd_send(cmd_dbg);

        dat_set_system_var(dat_rtc_date_time, (int) time(NULL));

        if((elapsed_sec % 2) == 0)
        {
            cmd_t *cmd_2s = cmd_get_str("sample_obc_sensors");
            cmd_send(cmd_2s);
        }

        /* 10 seconds actions */
        if((elapsed_sec % _10sec_check) == 0)
        {
            LOGD(tag, "10 sec tasks");
            //cmd_t *cmd_10s = cmd_get_str("get_mem");
            cmd_t *cmd_10s = cmd_get_str("test");
            cmd_add_params_var(cmd_10s, "Task housekeeping running");
            cmd_send(cmd_10s);
        }

        /* 10 minutes actions */
        if((elapsed_sec % _10min_check) == 0)
        {
            LOGD(tag, "10 min tasks");
            cmd_t *cmd_10m = cmd_get_str("get_mem");
            cmd_add_params_var(cmd_10m, 0);
            cmd_send(cmd_10m);
        }

        /* 1 hours actions */
        if((elapsed_sec % _1hour_check) == 0)
        {
            LOGD(tag, "1 hour check");
            cmd_t *cmd_1h = cmd_get_str("update_hours_alive");
            cmd_add_params_var(cmd_1h, 1); // Add 1hr
            cmd_send(cmd_1h);
        }
    }
}
