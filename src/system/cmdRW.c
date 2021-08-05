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
    cmd_add("rw_get_speed", rw_get_speed, "%d", 1);
    cmd_add("rw_get_current", rw_get_current, "%d", 1);
    cmd_add("rw_set_speed", rw_set_speed, "%d %d", 2);
    /** UPPER ISTAGE COMMANDS **/
#ifdef SCH_USE_ISTAGE2
    cmd_add("is2_get_temp", istage2_get_temp, "", 0);
    cmd_add("is2_get_state", istage2_get_state_panel, "%d", 1);
    cmd_add("is2_deploy", istage2_deploy_panel, "%d", 1);
    cmd_add("is2_set_deploy", istage2_set_deploy, "%d", 1);
#endif
}

int rw_get_speed(char *fmt, char *params, int nparams)
{
    int motorid;
    if(params == NULL || sscanf(params, fmt, &motorid) != nparams) {
        motorid = -1;
    }

    if(motorid > 0 && motorid < 4)
    {
        LOGI(tag, "Getting speed %d", motorid);
        uint16_t speed = rwdrv10987_get_speed(motorid);
        LOGR(tag, "Sampled speed%d: %d", motorid, speed);
    }
    else
    {
        LOGI(tag, "Getting all speeds");
        uint16_t speed1 = rwdrv10987_get_speed(MOTOR1_ID);
        uint16_t speed2 = rwdrv10987_get_speed(MOTOR2_ID);
        uint16_t speed3 = rwdrv10987_get_speed(MOTOR3_ID);
        LOGR(tag, "Sampled speed1: %d, speed2: %d, speed3: %d", speed1, speed2, speed3);
    }

    return CMD_OK; //TODO: check error code
}

int rw_get_current(char *fmt, char *params, int nparams)
{
    int motorid;
    if(params == NULL || sscanf(params, fmt, &motorid) != nparams) {
        motorid = -1;
    }

    if(motorid > 0 && motorid < 4)
    {
        LOGI(tag, "Sampling current %d", motorid)
        float current = rwdrv10987_get_current(motorid); //[mA]
        LOGR(tag, "Sampled current%d: %f", motorid, current);
    }
    else
    {
        LOGI(tag, "Sampling all currents");
        float current1 = rwdrv10987_get_current(MOTOR1_ID); //[mA]
        float current2 = rwdrv10987_get_current(MOTOR2_ID); //[mA]
        float current3 = rwdrv10987_get_current(MOTOR3_ID); //[mA]
        LOGR(tag, "Sampled current1: %f, current2: %f, current3: %f", current1, current2, current3);
    }

    return CMD_OK; //TODO: check error code
}

int rw_set_speed(char *fmt, char *params, int nparams)
{
    LOGI(tag, "Speed command");
    int motor_id;
    int speed;

    if(params == NULL || sscanf(params, fmt, &motor_id, &speed) != nparams)
        return CMD_SYNTAX_ERROR;

    int rc = rwdrv10987_set_speed(motor_id, speed);
    LOGR(tag, "Setting motor: %d speed: %d (%d)", motor_id, speed, rc);
    return rc == GS_OK ? CMD_OK : CMD_ERROR;
}

/** UPPER ISTAGE COMMANDS **/
#ifdef SCH_USE_ISTAGE2
int istage2_get_temp(char *fmt, char *params, int nparams)
{
    int rc;
    char istage_cmd[2] = {0, 0};
    char istage_ans[1];

    istage_cmd[0] = IS2_START_SENSORS_TEMP;
    rc = gs_i2c_master_transaction(ISTAGE_GSSB_TWI_HANDLER, ISTAGE_UPPER_ADD, istage_cmd, 2, istage_ans, 1, 500);
    LOGI(tag2, "START_SENSORS_TEMP %d (%d)", istage_ans[0], rc);
    if(rc != GS_OK)
        return CMD_ERROR;

    osDelay(100);

    istage_cmd[0] = I2S_SAMPLE_TEMP;
    rc = gs_i2c_master_transaction(ISTAGE_GSSB_TWI_HANDLER, ISTAGE_UPPER_ADD, istage_cmd, 2, istage_ans, 1, 500);
    LOGI(tag2, "SAMPLE_TEMP %d (%d)", istage_ans[0], rc);
    if(rc != GS_OK)
        return CMD_ERROR;

    osDelay(500);

    istage_cmd[0] = IS2_GET_TEMP;
    uint32_t temps[8];
    rc = gs_i2c_master_transaction(ISTAGE_GSSB_TWI_HANDLER, ISTAGE_UPPER_ADD, istage_cmd, 2, (char *)temps, sizeof(temps), 500);
    LOGR(tag2, "IS2_GET_TEMP %f, %f, %f, %f, %f, %f, %f, %f (%d)", temps[0], temps[1], temps[2], temps[3], temps[4], temps[5], temps[6], temps[7], rc);
    if(rc != GS_OK)
        return CMD_ERROR;

    istage_cmd[0] = IS2_STOP_SENSORS_TEMP;
    rc = gs_i2c_master_transaction(ISTAGE_GSSB_TWI_HANDLER, ISTAGE_UPPER_ADD, istage_cmd, 2, istage_ans, 1, 500);
    LOGI(tag2, "STOP_SENSORS_TEMP (%d)", rc);
    if(rc != GS_OK)
        return CMD_ERROR;

    return CMD_OK;

}

int istage2_get_state_panel(char *fmt, char *params, int nparams)
{
    int rc, panel;

    if(params == NULL || sscanf(params, fmt, &panel) != nparams)
    {
        LOGW(tag2, "get_state_panel used with invalid params!");
        return CMD_SYNTAX_ERROR;
    }

    char istage_cmd[2] = {IS2_READ_SW_FACE, (char)panel};
    char istage_ans[2] = {-1, -1};
    rc = gs_i2c_master_transaction(ISTAGE_GSSB_TWI_HANDLER, ISTAGE_UPPER_ADD, istage_cmd, 2, istage_ans, 2, 500);
    LOGR(tag2, "IS2_READ_SW_FACE %d=%d (%d)", panel, istage_ans[1], rc);

    if(rc != GS_OK)
        return CMD_ERROR;
    else
        return CMD_OK;
}

int istage2_deploy_panel(char *fmt, char *params, int nparams)
{
    int rc, panel;

    if(params == NULL || sscanf(params, fmt, &panel) != nparams)
    {
        LOGW(tag2, "deploy_panel used with invalid params!");
        return CMD_SYNTAX_ERROR;
    }

    char istage_cmd[2] = {IS2_BURN_FACE, (char)panel};
    rc = gs_i2c_master_transaction(ISTAGE_GSSB_TWI_HANDLER, ISTAGE_UPPER_ADD, istage_cmd, 2, NULL, 0, 500);
    LOGR(tag2, "IS2_BURN_FACE %d (%d)", panel, rc);

    if(rc != GS_OK)
        return CMD_ERROR;
    else
        return CMD_OK;
}

int istage2_set_deploy(char *fmt, char *params, int nparams)
{
    int rc, config;

    if(params == NULL || sscanf(params, fmt, &config) != nparams)
    {
        LOGW(tag2, "deploy_panel used with invalid params!");
        return CMD_SYNTAX_ERROR;
    }

    char istage_cmd[2] = {IS2_SET_BURN, (char)config};
    rc = gs_i2c_master_transaction(ISTAGE_GSSB_TWI_HANDLER, ISTAGE_UPPER_ADD, istage_cmd, 2, NULL, 0, 500);
    LOGR(tag2, "IS2_SET_BURN %d (%d)", config, rc);

    if(rc != GS_OK)
        return CMD_ERROR;
    else
        return CMD_OK;
}
#endif