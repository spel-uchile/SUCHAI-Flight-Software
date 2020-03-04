/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2019, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2019, Tomas Opazo Toro, tomas.opazo.t@gmail.com
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

    unsigned int elapsed_sec = 1;           // Seconds counter
    unsigned int _10sec_check = 1;         //10[s] condition
    unsigned int _01min_check = 1*60;       //05[m] condition
    unsigned int _05min_check = 5*60;       //05[m] condition
    unsigned int _1hour_check = 60*60;      //01[h] condition

    portTick xLastWakeTime = osTaskGetTickCount();

    cmd_t *tle1 = cmd_get_str("obc_set_tle");
    cmd_add_params_str(tle1, "1 42788U 17036Z   20027.14771603  .00000881  00000-0  39896-4 0  9992");
    cmd_send(tle1);

    cmd_t *tle2 = cmd_get_str("obc_set_tle");
    cmd_add_params_str(tle2, "2 42788  97.3234  85.2817 0012095 159.3521 200.8207 15.23399088144212");
    cmd_send(tle2);

    cmd_t *tle_u = cmd_get_str("obc_update_tle");
    cmd_send(tle_u);
    tle_u = cmd_get_str("obc_get_tle");
    cmd_send(tle_u);

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
        // Update position
        if ((elapsed_sec % _10sec_check) == 0)
        {
            // Check if the TLE epoch is valid
            int tle_epoch = dat_get_system_var(dat_ads_tle_epoch);
            if(tle_epoch != 0)
            {
                cmd_t *cmd_tle_prop = cmd_get_str("obc_prop_tle");
                cmd_add_params_str(cmd_tle_prop, "0");
                cmd_send(cmd_tle_prop);

                cmd_t *cmd_stt = cmd_get_str("sim_adcs_quat");
                cmd_send(cmd_stt);

                cmd_t *cmd_point = cmd_get_str("sim_adcs_to_nadir");
                cmd_send(cmd_point);

                cmd_t *cmd_ctrl = cmd_get_str("sim_adcs_control");
                cmd_send(cmd_ctrl);

                cmd_t *cmd_att = cmd_get_str("set_adcs_attitude");
                cmd_send(cmd_att);
            }
        }

        /* 1 minute actions */
        // Update status vars
        if ((elapsed_sec % _01min_check) == 0)
        {
            cmd_t *cmd_update;
            cmd_update = cmd_get_str("eps_update_status");
            cmd_send(cmd_update);
            cmd_update = cmd_get_str("obc_update_status");
            cmd_send(cmd_update);
        }

        /* 5 minutes actions */
        //  Sample OBC payloads
        if ((elapsed_sec % _05min_check) == 0)
        {
            if ((elapsed_sec % _05min_check*2) == 0)
            {
                cmd_t *cmd_get_eps = cmd_get_str("eps_get_hk");
                cmd_send(cmd_get_eps);
            }
            else
            {
                cmd_t *cmd_get_obc = cmd_get_str("obc_get_sensors");
                cmd_send(cmd_get_obc)
            }
        }

        /* 1 hours actions */
        if((elapsed_sec % _1hour_check) == 0)
        {
            LOGD(tag, "1 hour check");
            cmd_t *cmd_1h = cmd_get_str("drp_add_hrs_alive");
            cmd_add_params_var(cmd_1h, 1); // Add 1hr
            cmd_send(cmd_1h);
        }
    }
}
