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
    cmd_add("ebf", drp_execute_before_flight, "%d", 1);
    cmd_add("print_vars", drp_print_system_vars, "", 0);
    cmd_add("update_sys_var", drp_update_sys_var_idx, "%d %d", 2);
    cmd_add("update_hours_alive", drp_update_hours_alive, "%d", 1);
    cmd_add("clear_gnd_wdt", drp_clear_gnd_wdt, "", 0);
    cmd_add("sample_obc_sensors", drp_sample_obc_sensors, "", 0);
    cmd_add("test_system_vars", drp_test_system_vars, "", 0);
}

int drp_execute_before_flight(char *fmt, char *params, int nparams)
{
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

    /* Set sensors status variables */
    // TODO: Fix type of ADS variables. Currently saving floats as ints.
    dat_set_system_var(dat_obc_temp_1, sensor1/10.);
    dat_set_system_var(dat_obc_temp_2, sensor2/10.);
    dat_set_system_var(dat_obc_temp_3, gyro_temp);
    dat_set_system_var(dat_ads_acc_x, gyro_reading.gyro_x);
    dat_set_system_var(dat_ads_acc_y, gyro_reading.gyro_y);
    dat_set_system_var(dat_ads_acc_z, gyro_reading.gyro_z);
    dat_set_system_var(dat_ads_mag_x, hmc_reading.x);
    dat_set_system_var(dat_ads_mag_y, hmc_reading.y);
    dat_set_system_var(dat_ads_mag_z, hmc_reading.z);

    /* Print readings */
#if LOG_LEVEL >= LOG_LVL_INFO
    printf("\r\nTemp1: %.1f, Temp2 %.1f, Gyro temp: %.2f\r\n", sensor1/10., sensor2/10., gyro_temp);
    printf("Gyro x, y, z: %f, %f, %f\r\n", gyro_reading.gyro_x, gyro_reading.gyro_y, gyro_reading.gyro_z);
    printf("Mag x, y, z: %f, %f, %f\r\n\r\n",hmc_reading.x, hmc_reading.y, hmc_reading.z);
#endif
#endif

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