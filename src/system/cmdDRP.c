/*                                 SUCHAI
 *                       NANOSATELLITE FLIGHT SOFTWARE
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
    cmd_add("clear_gnd_wdt", drp_clear_gnd_wdt, "", 0);
    cmd_add("sample_obc_sensors", drp_sample_obc_sensors, "", 0);
}

int drp_print_system_vars(char *fmt, char *params, int nparams)
{
    LOGD(tag, "Displaying system variables list");

    printf("system variables repository\n");
    printf("index\t value\n");

    int var_index;
    for (var_index = 0; var_index < dat_system_last_var; var_index++)
    {
        int var = dat_get_system_var((dat_system_t)var_index);
        printf("%d\t %d\n", var_index, var);
    }

    return CMD_OK;
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

int drp_sample_obc_sensors(char *fmt, char *params, int nparams)
{
#ifdef NANOMIND
    int16_t sensor1, sensor2;
    float gyro_temp;

    mpu3300_gyro_t gyro_reading;
    hmc5843_data_t hmc_reading;

    /* Read board temperature sensors */
    sensor1 = lm70_read_temp(1);
    sensor2 = lm70_read_temp(2);

    /* Read gyroscope temperature and rate */
    mpu3300_read_temp(&gyro_temp);
    mpu3300_read_gyro(&gyro_reading);

    /* Read magnetometer */
    hmc5843_read_single(&hmc_reading);

    /* Print readings */
#if LOG_LEVEL >= LOG_LVL_INFO
    printf("\r\nTemp1: %.1f, Temp2 %.1f, Gyro temp: %.2f\r\n", sensor1/10., sensor2/10., gyro_temp);
    printf("Gyro x, y, z: %f, %f, %f\r\n", gyro_reading.gyro_x, gyro_reading.gyro_y, gyro_reading.gyro_z);
    printf("Mag x, y, z: %f, %f, %f\r\n\r\n",hmc_reading.x, hmc_reading.y, hmc_reading.z);
#endif
#endif

    return CMD_OK;
}