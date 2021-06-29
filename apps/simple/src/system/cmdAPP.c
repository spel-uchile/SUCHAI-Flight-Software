/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2021, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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

#include "app/system/cmdAPP.h"

static const char* tag = "cmdAPP";

void cmd_app_init(void)
{
    cmd_add("obc_get_sensors", obc_get_sensors, "", 0);
    cmd_add("obc_update_status", obc_update_status, "", 0);
}

int obc_get_sensors(char *fmt, char *params, int nparams)
{
#ifdef LINUX
    int curr_time =  (int)time(NULL);
    float systemp, millideg;
    FILE *thermal;
    int n;

    LOGD(tag, "Reading obc data in Linux \n timestamp: %d", curr_time);
    // Reading temp
    thermal = fopen("/sys/class/thermal/thermal_zone0/temp","r");
    n = fscanf(thermal,"%f",&millideg);
    fclose(thermal);
    if(n!= 1)
        return CMD_ERROR;
    // Save temp
    systemp = millideg / 1000;
    int index_temp = dat_get_system_var(data_map[temp_sensors].sys_index);
    struct temp_data data_temp = {index_temp, curr_time, systemp};
    LOGR(tag, "Temp1: %.1f", data_temp.obc_temp_1);
    dat_add_payload_sample(&data_temp, temp_sensors);
    return CMD_OK;
#else
    return CMD_ERROR;
#endif
}

int obc_update_status(char *fmt, char *params, int nparams)
{
#ifdef LINUX
    int curr_time =  (int)time(NULL);
    float systemp, millideg;
    FILE *thermal;
    int n;

    LOGD(tag, "Reading obc data in Linux \n timestamp: %d", curr_time);
    // Reading temp
    thermal = fopen("/sys/class/thermal/thermal_zone0/temp","r");
    n = fscanf(thermal,"%f",&millideg);
    fclose(thermal);
    if(n!= 1)
        return CMD_ERROR;

    // Save temp
    systemp = millideg / 1000;
    /* Set sensors status variables (fix type) */
    value32_t temp_1;
    temp_1.f = systemp;
    dat_set_status_var(dat_obc_temp_1, temp_1);

    LOGR(tag, "Temp1: %.1f", temp_1.f);
    return CMD_OK;
#else
    return CMD_ERROR;
#endif
}
