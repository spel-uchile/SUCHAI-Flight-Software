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
    cmd_add("eps_set_output", eps_set_output, "%d %d", 2);
    cmd_add("eps_set_output_all", eps_set_output_all, "%d", 1);
    cmd_add("eps_set_vboost", eps_set_vboost, "%d", 1);
    cmd_add("eps_set_mppt", eps_set_pptmode, "%d", 1);
    cmd_add("eps_reset_wdt", eps_reset_wdt, "", 0);
    cmd_add("eps_update_status", eps_update_status_vars, "", 0);
#endif
}

#ifdef SCH_USE_NANOPOWER
int eps_hard_reset(char *fmt, char *params, int nparams)
{
    if(eps_hardreset() > 0)
        return CMD_OK;

    LOGE(tag, "Unable to reset the EPS!");
    return CMD_ERROR;
}

int eps_get_hk(char *fmt, char *params, int nparams)
{
    eps_hk_t hk = {};
    if(eps_hk_get(&hk) > 0)
    {
        eps_hk_print(&hk);

        int curr_time = (int)time(NULL);
        int index_eps = dat_get_system_var(data_map[eps_sensors].sys_index);
        struct eps_data data_eps = {index_eps, curr_time, hk.cursun, hk.cursys, hk.vbatt,
                hk.temp[0], hk.temp[1], hk.temp[2], hk.temp[3], hk.temp[4], hk.temp[5]};

        int ret;
        ret = dat_add_payload_sample(&data_eps, eps_sensors);

        LOGI(tag, "WRITING EPS DATA: %u %u %u %d %d %d %d %d %d", hk.cursun, hk.cursys, hk.vbatt,
             hk.temp[0],  hk.temp[1], hk.temp[2], hk.temp[3], hk.temp[4], hk.temp[5]);
        if (ret == -1) {
            return CMD_ERROR;
        }

        struct eps_data data_eps_get;
         ret = dat_get_recent_payload_sample(&data_eps_get, eps_sensors, 0);
        LOGI(tag, "READING EPS DATA: %u %u %u %d %d %d %d %d %d", data_eps_get.cursun, data_eps_get.cursys, data_eps_get.vbatt,
             data_eps_get.temp1,  data_eps_get.temp2, data_eps_get.temp3, data_eps_get.temp4, data_eps_get.temp5, data_eps_get.temp6);
        if (ret == -1) {
            return CMD_ERROR;
        }
    }
    else
    {
        return CMD_ERROR;
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
        return CMD_ERROR;
    }
    return CMD_OK;
}

int eps_set_heater(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "NULL params!");
        return CMD_ERROR;
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
        return CMD_ERROR;
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
        return CMD_ERROR;
    }
    return CMD_OK;
}

int eps_set_output(char *fmt, char *params, int nparams)
{
    unsigned int channel, mode;
    // "<channel> <mode>"
    if(params == NULL || sscanf(params, fmt, &channel, &mode) != nparams)
    {
        LOGE(tag, "Error parsing parameters!");
        return CMD_SYNTAX_ERROR;
    }
    if(channel > 7){
        LOGE(tag, "Error parsing parameters: channel > 7 (%d)!", channel);
        return CMD_SYNTAX_ERROR;
    }
    mode = mode == 0 ? 0 : 1; // Mode is 0 -> OFF or > 0 = 1 -> ON

    int rc = eps_output_set_single((uint8_t)channel, (uint8_t)mode, 0);
    if(rc > 0)
        return CMD_OK;
    else
        return CMD_ERROR;
}

int eps_set_output_all(char *fmt, char *params, int nparams)
{
    unsigned int mode;
    uint8_t mask;
    // "<on/off>"
    if(params == NULL || sscanf(params, fmt, &mode) != nparams)
    {
        LOGE(tag, "Error parsing parameters!");
        return CMD_SYNTAX_ERROR;
    }

    mode = mode == 0 ? 0 : 1; // Mode is 0 -> OFF or > 0 = 1 -> ON
    mask = mode ? 0xFF : 0x00; // Mode 1 -> All outputs on, else all outputs off

    int rc = eps_output_set(mask);
    if(rc > 0)
        return CMD_OK;
    else
        return CMD_ERROR;
}

int eps_set_vboost(char *fmt, char *params, int nparams)
{
    int vboost;
    if(params == NULL || sscanf(params, fmt, &vboost) != nparams)
    {
        LOGE(tag, "Error parsing parameters!");
        return CMD_SYNTAX_ERROR;
    }

    int rc = eps_vboost_set(vboost, vboost, vboost);
    return rc > 0 ? CMD_OK : CMD_ERROR;
}

int eps_set_pptmode(char *fmt, char *params, int nparams)
{
    int pptmode;
    if(params == NULL || sscanf(params, fmt, &pptmode) != nparams)
    {
        LOGE(tag, "Error parsing parameters!");
        return CMD_SYNTAX_ERROR;
    }

    int rc = eps_pptmode_set((char)pptmode);
    return rc > 0 ? CMD_OK : CMD_ERROR;
}

int eps_reset_wdt(char *fmt, char *params, int nparams)
{
    int rc = eps_wdt_gnd_reset();
    return rc > 0 ? CMD_OK : CMD_ERROR;
}

#endif //SCH_USE_NANOPOWER