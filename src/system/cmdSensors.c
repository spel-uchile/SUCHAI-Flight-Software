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
        int rc = set_machine_state(action, step, nsamples);
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
                machine.active_payloads = 0;
                osSemaphoreGiven(&repo_machine_sem);

            } else  {
                osSemaphoreTake(&repo_machine_sem, portMAX_DELAY);
                machine.active_payloads = 0;
                int i;
                for (i=0; i < machine.total_sensors; i++) {
                    machine.active_payloads +=  1 << i ;
                }
                osSemaphoreGiven(&repo_machine_sem);
            }
            return CMD_OK;
        }

        if ( payload >= machine.total_sensors ) {
            return CMD_ERROR;
        }

        if( activate == 1  && !is_payload_active(payload, machine.active_payloads, machine.total_sensors) ) {
            osSemaphoreTake(&repo_machine_sem, portMAX_DELAY);
            machine.active_payloads = ((unsigned  int)1 << payload) | machine.active_payloads;
            osSemaphoreGiven(&repo_machine_sem);
            return CMD_OK;
        } else if ( activate == 0 && is_payload_active(payload, machine.active_payloads, machine.total_sensors) ) {
            osSemaphoreTake(&repo_machine_sem, portMAX_DELAY);
            machine.active_payloads = ((unsigned  int)1 << payload) ^ machine.active_payloads;
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
            struct temp_data data_temp = {curr_time, (float)(sensor1/10.0), (float)(sensor2/10.0), gyro_temp};
            dat_add_payload_sample(&data_temp, temp_sensors);
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
            struct ads_data data_ads = {curr_time,
                                        gyro_x, gyro_y, gyro_z,
                                        mag_x, mag_y, mag_z};
            dat_add_payload_sample(&data_ads, ads_sensors);
            return CMD_OK;
        }
        else if ( payload == 2) // EPS
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
            eps_hk_print(&hk);
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
            struct eps_data data_eps = {curr_time, cursun, cursys, vbatt,
                                        temp1, temp2, temp3, temp4, temp5, temp6};
            dat_add_payload_sample(&data_eps, eps_sensors);
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

int is_payload_active(int payload, int active_payloads, int n_payloads) {
//    printf("max number of active payload %d\n", (1 << n_payloads));
    if ( active_payloads >= (1 << n_payloads) ) {
        return 0;
    }
    return ( (active_payloads & (1 << payload)) != 0 );
}