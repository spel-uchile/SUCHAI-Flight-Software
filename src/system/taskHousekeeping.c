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

    while(1) {

        osTaskDelayUntil(&xLastWakeTime, delay_ms); //Suspend task
        elapsed_sec += delay_ms / 1000; //Update seconds counts

        dat_set_system_var(dat_rtc_date_time, (int) time(NULL));

        if ((elapsed_sec % 60) == 0) {
            if ((elapsed_sec % 120) == 0) {
                cmd_t *cmd_get_eps = cmd_get_str("eps_get_hk");
                cmd_send(cmd_get_eps);
            } else {
                cmd_t *cmd_get_obc = cmd_get_str("obc_get_sensors");
                cmd_send(cmd_get_obc)
        }

        }

    }
}
