/**
 *                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
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

#include "cmdEPS.h"

static const char *tag = "cmdEPS";

void cmd_eps_init(void)
{
#ifdef SCH_USE_NANOPOWER
    //EPS io driver requires some initialization
    eps_set_node(SCH_EPS_ADDRESS);
    eps_set_timeout(1000);

    // Register commands
    cmd_add("eps_hard_reset", eps_hard_reset, "", 0);
    cmd_add("eps_get_hk", eps_get_hk, "", 0);
    cmd_add("eps_get_config", eps_get_config, "", 0);
    cmd_add("eps_set_heater", eps_set_heater, "%d %d", 2);
    cmd_add("eps_update_status", eps_update_status_vars, "", 0);
#endif
}

#ifdef SCH_USE_NANOPOWER
int eps_hard_reset(char *fmt, char *params, int nparams)
{
    if(eps_hardreset() > 0)
        return CMD_OK;

    LOGE(tag, "Unable to reset the EPS!");
    return CMD_FAIL;
}

int eps_get_hk(char *fmt, char *params, int nparams)
{
    eps_hk_t hk = {};
    if(eps_hk_get(&hk) > 0)
    {
        eps_hk_print(&hk);

        int curr_time = (int)time(NULL);
        struct eps_data data_eps = {curr_time, hk.cursun, hk.cursys, hk.vbatt,
                hk.temp[0], hk.temp[1], hk.temp[2], hk.temp[3], hk.temp[4], hk.temp[5]};

        dat_add_payload_sample(&data_eps, eps_sensors);

        LOGI(tag, "WRITING EPS DATA: %u %u %u %d %d %d %d %d %d", hk.cursun, hk.cursys, hk.vbatt,
             hk.temp[0],  hk.temp[1], hk.temp[2], hk.temp[3], hk.temp[4], hk.temp[5]);

        struct eps_data data_eps_get;
        dat_get_recent_payload_sample(&data_eps_get, eps_sensors, 0);
        LOGI(tag, "READING EPS DATA: %u %u %u %d %d %d %d %d %d", data_eps_get.cursun, data_eps_get.cursys, data_eps_get.vbatt,
             data_eps_get.temp1,  data_eps_get.temp2, data_eps_get.temp3, data_eps_get.temp4, data_eps_get.temp5, data_eps_get.temp6);
    }
    else
    {
        return CMD_FAIL;
    }
    return CMD_OK;
}

int eps_get_config(char *fmt, char *params, int nparams)
{
    eps_config_t nanopower_config;

    if(eps_config_get(&nanopower_config) > 0)
    {
        eps_config_print(&nanopower_config);
    }
    else
    {
        return CMD_FAIL;
    }
    return CMD_OK;
}

int eps_set_heater(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "NULL params!");
        return CMD_FAIL;
    }

    int heater, on_off;
    uint8_t state[2];

    if(sscanf(params, fmt, &heater, &on_off) == nparams)
    {
        LOGI(tag, "Setting heater %d to state %d", heater, on_off);
        eps_heater((uint8_t) heater, (uint8_t) on_off, state);
        LOGI(tag, "Heater state is %u %u",(unsigned int) state[0],(unsigned int) state[1]);
        return CMD_OK;
    }
    else
    {
        LOGE(tag, "Invalid params!");
        return CMD_FAIL;
    }
}

int eps_update_status_vars(char *fmt, char *params, int nparams)
{
    eps_hk_t hk = {};
    if(eps_hk_get(&hk) > 0)
    {
        dat_set_system_var(dat_eps_vbatt, hk.vbatt);
        dat_set_system_var(dat_eps_cur_sun, hk.cursun);
        dat_set_system_var(dat_eps_cur_sys, hk.cursys);
        dat_set_system_var(dat_eps_temp_bat0, hk.temp[4]);
    }
    else
    {
        return CMD_FAIL;
    }
    return CMD_OK;
}

#endif //SCH_USE_NANOPOWER