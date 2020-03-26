/*                                 SUCHAI
 *                       NANOSATELLITE FLIGHT SOFTWARE
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

#include "cmdDRP.h"

static const char *tag = "cmdDRP";

/**
 * This function registers the list of command in the system, initializing the
 * functions array. This function must be called at every system start up.
 */
void cmd_drp_init(void)
{
    cmd_add("drp_ebf", drp_execute_before_flight, "%d", 1);
    cmd_add("drp_get_vars", drp_print_system_vars, "", 0);
    cmd_add("drp_set_var", drp_update_sys_var_idx, "%d %d", 2);
    cmd_add("drp_add_hrs_alive", drp_update_hours_alive, "%d", 1);
    cmd_add("drp_clear_gnd_wdt", drp_clear_gnd_wdt, "", 0);
    cmd_add("drp_test_system_vars", drp_test_system_vars, "", 0);
    cmd_add("drp_set_deployed", drp_set_deployed, "%d", 1);
}

int drp_execute_before_flight(char *fmt, char *params, int nparams)
{
    if(params == NULL)
        return CMD_ERROR;

    int magic;
    if(nparams == sscanf(params, fmt, &magic))
    {
        if(magic == SCH_DRP_MAGIC)
        {
            // Reset all status variables values to 0
            dat_system_t var;
            for(var = dat_obc_opmode; var < dat_system_last_var; var++)
            {
                dat_set_system_var(var, 0);
            }

            // Set all status variables default values
            dat_set_system_var(dat_rtc_date_time, (int)time(NULL));
            // dat_set_system_var(dat_custom, default_value);

            // Delete memory sections
            dat_delete_memory_sections();

            return CMD_OK;
        }
        else
        {
            LOGW(tag, "EBF executed with incorrect magic number!")
            return CMD_FAIL;
        }
    }
    else
    {
        LOGW(tag, "EBF executed with invalid parameter!")
        return CMD_FAIL;
    }
}

int drp_print_system_vars(char *fmt, char *params, int nparams)
{
    LOGD(tag, "Displaying system variables list");

    dat_status_t status;
    dat_status_to_struct(&status);
    dat_print_status(&status);

    return CMD_OK;
}

int drp_update_sys_var_idx(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Parameter null");
        return CMD_FAIL;
    }
    int index, value;
    if(sscanf(params, fmt, &index, &value) == nparams)
    {
        dat_system_t var_index = (dat_system_t)index;
        if(var_index < dat_system_last_var)
        {
            dat_set_system_var(var_index, value);
            return CMD_OK;
        }
        else
        {
            LOGW(tag, "drp_update_sys_idx used with invalid index: %d", var_index);
            return CMD_FAIL;
        }
    }
    else
    {
        LOGW(tag, "drp_update_sys_idx used with invalid params: %s", params);
        return CMD_FAIL;
    }

}

int drp_update_hours_alive(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Parameter null");
        return CMD_FAIL;
    }
    int value;  // Value to add
    int current;  // Current value to update

    if(sscanf(params, fmt, &value) == nparams)
    {
        // Adds <value> to current hours alive
        current = dat_get_system_var(dat_obc_hrs_alive);
        current += value;
        dat_set_system_var(dat_obc_hrs_alive, current);

        // Adds <value> to current hours without reset
        current = dat_get_system_var(dat_obc_hrs_wo_reset);
        current += value;
        dat_set_system_var(dat_obc_hrs_wo_reset, current);
        return CMD_OK;
    }
    else
    {
        LOGW(tag, "drp_update_hours_alive used with invalid params: %s", params);
        return CMD_FAIL;
    }
}

int drp_clear_gnd_wdt(char *fmt, char *params, int nparams)
{
    dat_set_system_var(dat_obc_sw_wdt, 0);
    return CMD_OK;
}

int drp_test_system_vars(char *fmt, char *params, int nparams)
{
    int var_index;
    int var;
    int init_value;
    int test_value = 85;
    int return_value = CMD_OK;

    for (var_index = 0; var_index < dat_system_last_var; var_index++)
    {
        init_value = dat_get_system_var((dat_system_t) var_index);
        dat_set_system_var((dat_system_t) var_index, test_value);
        var = dat_get_system_var((dat_system_t) var_index);
        dat_set_system_var((dat_system_t) var_index, init_value);
        LOGV(tag, "Variable:%d, value: %d, expected %d", var, var, test_value);
        if (var != test_value)
        {
            return_value = CMD_FAIL;
        }
    }

    return return_value;
}

int drp_set_deployed(char *fmt, char *params, int nparams)
{
    int deployed;
    if(sscanf(params, fmt, &deployed) == nparams)
    {
        dat_set_system_var(dat_dep_deployed, deployed);
        return CMD_OK;
    }
    else
    {
        LOGE(tag, "Error parsing params");
        return CMD_ERROR;
    }
}
