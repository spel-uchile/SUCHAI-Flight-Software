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

#include "cmdRW.h"

static const char* tag = "cmdRW";
static const char* tag2 = "cmdIstage";

void cmd_rw_init(void)
{
    /** RW COMMANDS **/
    cmd_add("rw_get_speed", rw_get_speed, "", 0);
    cmd_add("rw_get_current", rw_get_current, "", 0);
    cmd_add("rw_set_speed", rw_set_speed, "%d %d", 2);
    /** UPPER ISTAGE COMMANDS **/
#ifdef SCH_USE_ISTAGE2
    cmd_add("is2_get_temp", istage2_get_temp, "", 0);
    cmd_add("is2_get_state", istage2_get_state_panel, "%d", 1);
    cmd_add("is2_deploy", istage2_deploy_panel, "%d", 1);
    cmd_add("is2_set_deploy", istage2_set_deploy, "%d", 1);
    cmd_add("is2_get_status", istage2_get_sensors_status, "", 0);
#endif
}

int rw_get_speed(char *fmt, char *params, int nparams)
{
    LOGI(tag, "Getting all speeds");

    uint16_t speed1 = rwdrv10987_get_speed(MOTOR1_ID);
    uint16_t speed2 = rwdrv10987_get_speed(MOTOR2_ID);
    uint16_t speed3 = rwdrv10987_get_speed(MOTOR3_ID);

    LOGI(tag, "Sampled speed1: %d, speed2: %d, speed3: %d", speed1, speed2, speed3);

    return 1; //TO DO: check error code
}

int rw_get_current(char *fmt, char *params, int nparams)
{
    LOGI(tag, "Sampling all currents");

    float current1 = rwdrv10987_get_current(MOTOR1_ID); //[mA]
    float current2 = rwdrv10987_get_current(MOTOR2_ID); //[mA]
    float current3 = rwdrv10987_get_current(MOTOR3_ID); //[mA]

    LOGI(tag, "Sampled current1: %f, current2: %f, current3: %f", current1, current2, current3);

    return 1; //TO DO: check error code
}

int rw_set_speed(char *fmt, char *params, int nparams)
{
    LOGI(tag, "Speed command");

    int motor_id;
    int speed;

    if(sscanf(params, fmt, &motor_id, &speed) == nparams)
    {
        LOGI(tag, "Setting motor: %d speed: %d", motor_id, speed);

        rwdrv10987_set_speed(motor_id, speed);
    }
    return 1; //TO DO: check error code
}

/** UPPER ISTAGE COMMANDS **/
#ifdef SCH_USE_ISTAGE2
int istage2_get_temp(char *fmt, char *params, int nparams)
{
    int rc;
    char istage_cmd[2] = {0, 0};
    char istage_ans[1];

    istage_cmd[0] = IS2_START_SENSORS_TEMP;
    rc = gs_i2c_master_transaction(ISTAGE_GSSB_TWI_HANDLER, ISTAGE_UPPER_ADD, istage_cmd, 2, istage_ans, 1, 250);
    LOGI(tag2, "START_SENSORS_TEMP %d (%d)", istage_ans[0], rc);


    istage_cmd[0] = IS2_GET_TEMP;
    uint16_t temps[8];
    rc = gs_i2c_master_transaction(ISTAGE_GSSB_TWI_HANDLER, ISTAGE_UPPER_ADD, istage_cmd, 2, (char *)temps, 8*sizeof(uint16_t), 250);
    LOGI(tag2, "IS2_GET_TEMP %d, %d, %d, %d, %d, %d, %d, %d (%d)", temps[0], temps[1], temps[2], temps[3], temps[4], temps[5], temps[6], temps[7], rc);

    istage_cmd[0] = IS2_STOP_SENSORS_TEMP;
    rc = gs_i2c_master_transaction(ISTAGE_GSSB_TWI_HANDLER, ISTAGE_UPPER_ADD, istage_cmd, 2, NULL, 0, 250);
    LOGI(tag2, "STOP_SENSORS_TEMP (%d)", rc);

    return CMD_OK;

}

int istage2_get_state_panel(char *fmt, char *params, int nparams)
{
    int rc, panel;

    if(params == NULL || sscanf(params, fmt, &panel) != nparams)
    {
        LOGW(tag2, "get_state_panel used with invalid params!");
        return CMD_FAIL;
    }

    char istage_cmd[2] = {IS2_READ_SW_FACE, (char)panel};
    char istage_ans[1] = {-1};
    rc = gs_i2c_master_transaction(ISTAGE_GSSB_TWI_HANDLER, ISTAGE_UPPER_ADD, istage_cmd, 2, istage_ans, 1, 250);
    LOGI(tag2, "IS2_READ_SW_FACE %d=%d (%d)", panel, istage_ans[0], rc);

    return CMD_OK;
}

int istage2_deploy_panel(char *fmt, char *params, int nparams)
{
    int rc, panel;

    if(params == NULL || sscanf(params, fmt, &panel) != nparams)
    {
        LOGW(tag2, "deploy_panel used with invalid params!");
        return CMD_FAIL;
    }

    char istage_cmd[2] = {IS2_BURN_FACE, (char)panel};
    char istage_ans[1] = {-1};
    rc = gs_i2c_master_transaction(ISTAGE_GSSB_TWI_HANDLER, ISTAGE_UPPER_ADD, istage_cmd, 2, NULL, 0, 250);
    LOGI(tag2, "IS2_BURN_FACE %d (%d)", panel, rc);

    return CMD_OK;
}

int istage2_set_deploy(char *fmt, char *params, int nparams)
{
    int rc, config;

    if(params == NULL || sscanf(params, fmt, &config) != nparams)
    {
        LOGW(tag2, "deploy_panel used with invalid params!");
        return CMD_FAIL;
    }

    char istage_cmd[2] = {IS2_SET_BURN, (char)config};
    char istage_ans[1] = {-1};
    rc = gs_i2c_master_transaction(ISTAGE_GSSB_TWI_HANDLER, ISTAGE_UPPER_ADD, istage_cmd, 2, NULL, 0, 250);
    LOGI(tag2, "IS2_SET_BURN %d (%d)", config, rc);

    return CMD_OK;
}

int istage2_get_sensors_status(char *fmt, char *params, int nparams)
{
    int rc;
    char istage_cmd[2] = {IS2_SENSORS_TEMP_STATUS, 0};
    char istage_ans[1] = {-1};
    rc = gs_i2c_master_transaction(ISTAGE_GSSB_TWI_HANDLER, ISTAGE_UPPER_ADD, istage_cmd, 2, istage_ans, 1, 250);
    LOGI(tag2, "IS2_SENSORS_TEMP_STATUS %d (%d)", istage_ans[0], rc);

    return CMD_OK;
}
#endif