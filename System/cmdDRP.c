/*                                 SUCHAI
 *                       NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2017, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2017, Tomas Opazo Toro, tomas.opazo.t@gmail.com
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
    cmd_add("print_vars", drp_print_system_vars, "", 0);
    cmd_add("update_sys_var", drp_update_sys_var_idx, "%d %d", 2);
    cmd_add("update_hours_alive", drp_update_hours_alive, "%d", 1);
}

int drp_print_system_vars(char *fmt, char *params, int nparams)
{
    LOGD(tag, "Displaying system variables list");

    //Take log_mutex to take control of the console
    int rc = osSemaphoreTake(&log_mutex, portMAX_DELAY);
    if(rc == CSP_SEMAPHORE_OK)
    {
        printf("System variables repository\n");
        printf("index\t value\n");

        int var_index;
        for (var_index = 0; var_index < dat_system_last_var; var_index++)
        {
            int var = dat_get_system_var((dat_system_t)var_index);
            printf("%d\t %d\n", var_index, var);
        }

        //Exit critical zone
        osSemaphoreGiven(&log_mutex);

        return CMD_OK;
    }
    else
    {
        LOGE(tag, "Unable to acquire console mutex");
        return CMD_FAIL;
    }
}

int drp_update_sys_var_idx(char *fmt, char *params, int nparams)
{
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
    int value;  // Value to add
    int current;  // Current value to update

    if(sscanf(params, fmt, &value) == nparams)
    {
        // Adds <value> to current hours alive
        current = dat_get_system_var(dat_obc_hours_alive);
        current += value;
        dat_set_system_var(dat_obc_hours_alive, current);

        // Adds <value> to current hours without reset
        current = dat_get_system_var(dat_obc_hours_without_reset);
        current += value;
        dat_set_system_var(dat_obc_hours_without_reset, current);
    }
    else
    {
        LOGW(tag, "drp_update_hours_alive used with invalid params: %s", params);
        return CMD_FAIL;
    }
}
