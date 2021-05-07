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
    cmd_add("drp_print_vars", drp_print_system_vars, "", 0);
    cmd_add("drp_set_var", drp_update_sys_var_idx, "%d %f", 2);
    cmd_add("drp_set_var_name", drp_update_sys_var_name, "%s %f", 2);
    cmd_add("drp_get_var_name", drp_get_sys_var_name, "%s", 1);
    cmd_add("drp_add_hrs_alive", drp_update_hours_alive, "%d", 1);
    cmd_add("drp_clear_gnd_wdt", drp_clear_gnd_wdt, "", 0);
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
            int index;
            int rc = 0;
            // Reset all status variables values to default values
            for(index=0; index < dat_status_last_address; index++)
            {
                rc += dat_set_status_var(index, dat_get_status_var_def(index).value);
            }

            // Update relevant status variables
            rc += dat_set_system_var(dat_rtc_date_time, (int) time(NULL));

            // Delete memory sections
            rc += dat_delete_memory_sections();

            if(rc == 0)
                return CMD_OK;
            else
            {
                LOGE(tag, "%d errors in drp_ebf!", -rc);
                return CMD_FAIL;
            }
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
    printf("idx, %-20s, value, type\n", "name");
    int i;
    for(i=0; i<dat_status_last_var; i++)
    {
        dat_sys_var_t var = dat_status_list[i];
        value32_t value = dat_get_status_var(var.address);
        var.value = value;
        dat_print_system_var(&var);
    }
    return CMD_OK;
}

int drp_update_sys_var_idx(char *fmt, char *params, int nparams)
{
    int address;
    float value;
    if(params == NULL || sscanf(params, fmt, &address, &value) != nparams)
    {
        LOGE(tag, "Error parsing arguments!");
        return CMD_ERROR;
    }

    dat_status_address_t var_address = (dat_status_address_t)address;
    if(var_address < dat_status_last_address)
    {
        // Support int and float arguments
        value32_t variable;
        dat_sys_var_t variable_def = dat_get_status_var_def(var_address);
        switch (variable_def.type) {
            case 'f':
                variable.f = value;
                break;
            default:
                variable.i = (int32_t)value;
                break;
        }
        // Store the variable value
        dat_set_status_var(var_address, variable);
        return CMD_OK;
    }
    else
    {
        LOGE(tag, "drp_update_sys_idx used with invalid index: %d", var_address);
        return CMD_FAIL;
    }
}

int drp_update_sys_var_name(char *fmt, char *params, int nparams)
{
    char name[MAX_VAR_NAME];
    float value;
    if(params == NULL || sscanf(params, fmt, name, &value) != nparams)
    {
        LOGE(tag, "Error parsing arguments!");
        return CMD_ERROR;
    }

    // Get variable definition by name
    dat_sys_var_t variable_def = dat_get_status_var_def_name(name);
    if(variable_def.status == -1)
    {
        LOGE(tag, "drp_update_sys_var_name used with invalid name: %s", name);
        return CMD_FAIL;
    }

    // Support int and float arguments
    value32_t variable;
    switch (variable_def.type) {
        case 'f':
            variable.f = value;
            break;
        default:
            variable.i = (int32_t)value;
            break;
    }

    // Store the variable value
    int rc = dat_set_status_var(variable_def.address, variable);
    return rc >= 0 ? CMD_OK : CMD_FAIL;
}

int drp_get_sys_var_name(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Error parsing arguments!");
        return CMD_ERROR;
    }

    if(strlen(params) > MAX_VAR_NAME) {
        LOGE(tag, "drp_get_sys_var_name used with invalid name: %s", params);
        return CMD_FAIL;
    }

    char name[MAX_VAR_NAME];

    if(sscanf(params, fmt, name) != nparams)
    {
        LOGE(tag, "Error parsing arguments!");
        return CMD_ERROR;
    }

    // Get variable definition by name
    dat_sys_var_t variable_def = dat_get_status_var_def_name(name);
    if(variable_def.status == -1)
    {
        LOGE(tag, "drp_update_sys_var_name used with invalid name: %s", name);
        return CMD_FAIL;
    }

    // Support int and float arguments
    value32_t variable = dat_get_status_var(variable_def.address);
    variable_def.value = variable;
    dat_print_system_var(&variable_def);

    return CMD_OK;
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

int drp_set_deployed(char *fmt, char *params, int nparams)
{
    int deployed;
    if(params == NULL || sscanf(params, fmt, &deployed) != nparams)
    {
        return CMD_FAIL;
    }

    dat_set_system_var(dat_dep_deployed, deployed);
    return CMD_OK;
}
