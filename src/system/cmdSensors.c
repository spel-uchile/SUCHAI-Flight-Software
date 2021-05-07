/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2018, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
 *      Copyright 2018, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *      Copyright 2018, Matias Ramirez Martinez, nicoram.mt@gmail.com
 *      Copyright 2018, Camilo Rojas, camrojas@uchile.cl
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

#include "cmdSensors.h"

static const char* tag = "cmdSens";

void cmd_sensors_init(void)
{
    cmd_add("sen_set_machine", set_state, "%u %u %d", 3);
    cmd_add("sen_activate", activate_sensor, "%d %d", 2);
    cmd_add("sen_take_sample", take_sample, "%u", 1);
    cmd_add("sen_init_dummy", init_dummy_sensor, "", 0);
}

int set_state(char *fmt, char *params, int nparams)
{
    if(params == NULL)
        return CMD_ERROR;

    unsigned int action;
    unsigned int step;
    int nsamples;
    if(nparams == sscanf(params, fmt, &action, &step, &nsamples)){
        int rc = dat_set_stmachine_state(action, step, nsamples);
        return rc ? CMD_OK : CMD_FAIL;
    }
    return CMD_ERROR;
}

int activate_sensor(char *fmt, char *params, int nparams)
{
    if(params == NULL)
        return CMD_ERROR;

    int payload;
    int activate;
    if(nparams == sscanf(params, fmt, &payload, &activate))
    {
        if(payload < 0 ) {
            if (activate == 0) {
                osSemaphoreTake(&repo_machine_sem, portMAX_DELAY);
                status_machine.active_payloads = 0;
                osSemaphoreGiven(&repo_machine_sem);

            } else  {
                osSemaphoreTake(&repo_machine_sem, portMAX_DELAY);
                status_machine.active_payloads = 0;
                int i;
                for (i=0; i < status_machine.total_sensors; i++) {
                    status_machine.active_payloads +=  1 << i ;
                }
                osSemaphoreGiven(&repo_machine_sem);
            }
            return CMD_OK;
        }

        if (payload >= status_machine.total_sensors ) {
            return CMD_ERROR;
        }

        if( activate == 1  && !dat_stmachine_is_sensor_active(payload,
                                                              status_machine.active_payloads,
                                                              status_machine.total_sensors) ) {
            osSemaphoreTake(&repo_machine_sem, portMAX_DELAY);
            status_machine.active_payloads = ((unsigned  int)1 << payload) | status_machine.active_payloads;
            osSemaphoreGiven(&repo_machine_sem);
            return CMD_OK;
        } else if ( activate == 0 && dat_stmachine_is_sensor_active(payload,
                                                                    status_machine.active_payloads,
                                                                    status_machine.total_sensors) ) {
            osSemaphoreTake(&repo_machine_sem, portMAX_DELAY);
            status_machine.active_payloads = ((unsigned  int)1 << payload) ^ status_machine.active_payloads;
            osSemaphoreGiven(&repo_machine_sem);
            return CMD_OK;
        }
    }
    return CMD_ERROR;
}

int take_sample(char *fmt, char *params, int nparams)
{
    if(params == NULL)
        return CMD_ERROR;

    int payload;
    if(nparams == sscanf(params, fmt, &payload)) {

        int ret;

        if( payload >= last_sensor) {
            return CMD_ERROR;
        }

        int curr_time =  (int)time(NULL);
        if( payload == 0 ) // Temperature
        {
            int16_t sensor1=1, sensor2=2;
            float gyro_temp=1.0;
#ifdef NANOMIND
            /* Read board temperature sensors */
            gs_lm71_read_temp(GS_A3200_SPI_SLAVE_LM71_0, 100, &sensor1); //sensor1 = lm70_read_temp(1);
            gs_lm71_read_temp(GS_A3200_SPI_SLAVE_LM71_1, 100, &sensor2); //sensor2 = lm70_read_temp(2);
            gs_mpu3300_read_temp(&gyro_temp);
#endif
            /* Save temperature data */
            int index_temp = dat_get_system_var(data_map[temp_sensors].sys_index);
            struct temp_data data_temp = {index_temp, curr_time, (float)(sensor1/10.0), (float)(sensor2/10.0), gyro_temp};
            ret = dat_add_payload_sample(&data_temp, temp_sensors);

            if (ret != 0) {
                return CMD_FAIL;
            }
            return CMD_OK;
        }
        else if ( payload == 1) // ADS
        {
            float gyro_x=1.0, gyro_y=2.0, gyro_z=3.0, mag_x=4.0, mag_y=5.0, mag_z=6.0;
#ifdef NANOMIND

            gs_mpu3300_gyro_t gyro_reading;
            gs_hmc5843_data_t hmc_reading;
            gs_mpu3300_read_gyro(&gyro_reading);
            gs_hmc5843_read_single(&hmc_reading);

            gyro_x = gyro_reading.gyro_x;
            gyro_y = gyro_reading.gyro_y;
            gyro_z = gyro_reading.gyro_z;
            mag_x = hmc_reading.x;
            mag_y = hmc_reading.y;
            mag_z = hmc_reading.z;
#endif
            /* Save ADCS data */
            int index_ads = dat_get_system_var(data_map[ads_sensors].sys_index);
            struct ads_data data_ads = {index_ads, curr_time,
                                        gyro_x, gyro_y, gyro_z,
                                        mag_x, mag_y, mag_z};
            ret = dat_add_payload_sample(&data_ads, ads_sensors);
            if (ret != 0) {
                return CMD_FAIL;
            }
            return CMD_OK;
        }
        else if (payload == 2) // EPS
        {
            uint32_t cursun=1;
            uint32_t cursys=2;
            uint32_t vbatt=3;
            int32_t temp1=4;
            int32_t temp2=5;
            int32_t temp3=6;
            int32_t temp4=7;
            int32_t temp5=8;
            int32_t temp6=9;

#ifdef NANOMIND
            eps_hk_t hk = {};
            eps_hk_get(&hk);
            cursun = hk.cursun;
            cursys = hk.cursys;
            vbatt = hk.vbatt;
            temp1 =  hk.temp[0];
            temp2 = hk.temp[1];
            temp3 = hk.temp[2];
            temp4 = hk.temp[3];
            temp5 = hk.temp[4];
            temp6 = hk.temp[5];
#endif
            int index_eps = dat_get_system_var(data_map[eps_sensors].sys_index);
            struct eps_data data_eps = {index_eps, curr_time, cursun, cursys, vbatt,
                                        temp1, temp2, temp3, temp4, temp5, temp6};
            ret = dat_add_payload_sample(&data_eps, eps_sensors);
            if (ret != 0) {
                return CMD_FAIL;
            }
            return CMD_OK;
        } else if (payload == 3) {

            // Pack status variables to a structure
            int i;
            uint32_t status_buff[dat_status_last_var];
            for(i = 0; i<dat_status_last_var; i++)
            {
                status_buff[i] = dat_get_status_var(dat_status_list[i].address).u;
            }

            int index_sta = dat_get_system_var(data_map[sta_sensors].sys_index);
            struct sta_data data_sta = {index_sta, curr_time};
            memcpy(data_sta.sta_buff, status_buff, sizeof(status_buff));
            ret = dat_add_payload_sample(&data_sta, sta_sensors);
            if (ret != 0) {
                return CMD_FAIL;
            }
            return CMD_OK;
        }
    }
    return CMD_ERROR;
}

int init_dummy_sensor(char *fmt, char *params, int nparams)
{
    LOGI(tag, "Initializing dummy sensor");
    return CMD_OK;
}
