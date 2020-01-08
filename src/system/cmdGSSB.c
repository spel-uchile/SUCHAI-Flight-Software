/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2019, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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


#include "cmdGSSB.h"

#define CMD_ERROR_NONE CMD_OK
#define CMD_ERROR_SYNTAX  CMD_ERROR
#define CMD_ERROR_FAIL CMD_FAIL
#define GS_OK 0

static const char* tag = "cmdGSSB";
static uint8_t i2c_addr = 5;            ///< Selected I2C device
static uint16_t i2c_timeout_ms = 1000;  ///< I2C timeout

void cmd_gssb_init(void)
{
    cmd_add("gssb_pwr", gssb_pwr, "%d %d", 2);
    cmd_add("gssb_select", gssb_select_addr, "%i", 1);
    cmd_add("gssb_scan", gssb_bus_scan, "%i %i", 2);
    cmd_add("gssb_fss_get_sun", gssb_read_sunsensor, "", 0);
    cmd_add("gssb_fss_get_temp", gssb_get_temp, "", 0);
    cmd_add("gssb_fss_set_config", gssb_sunsensor_conf, "%d", 1);
    cmd_add("gssb_fss_commit_config", gssb_sunsensor_conf_save, "", 0);
    cmd_add("gssb_set_addr", gssb_set_i2c_addr, "%i", 1);
    cmd_add("gssb_commit_addr", gssb_commit_i2c_addr, "", 0);
    cmd_add("gssb_get_version", gssb_get_version, "", 0);
    cmd_add("gssb_get_uuid", gssb_get_uuid, "", 0);
    cmd_add("gssb_get_model", gssb_get_model, "", 0);
    cmd_add("gssb_get_temp", gssb_interstage_temp, "", 0);
    cmd_add("gssb_msp_get_temp", gssb_msp_outside_temp, "", 0);
    cmd_add("gssb_msp_cal_temp", gssb_msp_outside_temp_calibrate, "%d %d", 2);
    cmd_add("gssb_get_temp_int", gssb_internal_temp, "", 0);
    cmd_add("gssb_burn", gssb_interstage_burn, "", 0);
    cmd_add("gssb_get_sun", gssb_common_sun_voltage, "", 0);
    cmd_add("gssb_get_burn_config", gssb_interstage_get_burn_settings, "", 0);
    cmd_add("gssb_set_burn_config", gssb_interstage_set_burn_settings, "%d %d %d %d %d %d %d", 7);
    cmd_add("gssb_arm_auto", gssb_interstage_arm, "%d", 1);
    cmd_add("gssb_arm_manual", gssb_interstage_state, "%d", 1);
    cmd_add("gssb_unlock_config", gssb_interstage_settings_unlock, "%d", 1);
    cmd_add("gssb_reset", gssb_soft_reset, "", 0);
    cmd_add("gssb_get_status", gssb_interstage_get_status, "", 0);
    cmd_add("gssb_update_status", gssb_update_status, "", 0);
    cmd_add("gssb_antenna_release", gssb_antenna_release, "%d %d %d %d", 4);

}

int gssb_pwr(char *fmt, char *params, int nparams)
{
    if(params == NULL)
        return CMD_ERROR_SYNTAX;

    int vcc_on, vcc2_on;
    if(sscanf(params, fmt, &vcc_on, &vcc2_on) != nparams)
        return CMD_ERROR_SYNTAX;
    
    if (vcc_on > 0)
        gs_a3200_pwr_switch_enable(GS_A3200_PWR_GSSB);
    else
        gs_a3200_pwr_switch_disable(GS_A3200_PWR_GSSB);

    if (vcc2_on > 0)
        gs_a3200_pwr_switch_enable(GS_A3200_PWR_GSSB2);
    else
        gs_a3200_pwr_switch_disable(GS_A3200_PWR_GSSB2);

    osDelay(100);

    LOGI(tag, "Set GSSB switch state to: %d %d", vcc_on, vcc2_on);
    return CMD_OK;
}


int gssb_select_addr(char *fmt, char *params, int nparams)
{
    int addr;

    if (params == NULL)
        return CMD_ERROR_SYNTAX;

    if(sscanf(params, fmt, &addr) == nparams)
    {
        if(addr > 0)
            i2c_addr = addr;
        
        LOGI(tag, "Selected I2C addr %#X (%d)", i2c_addr, i2c_addr);
        return CMD_ERROR_NONE;
    }

    return CMD_ERROR_SYNTAX;
}

int gssb_bus_scan(char *fmt, char *params, int nparams)
{
    int8_t devices[128];
    uint8_t addr = 0;
    int start;
    int end;

    if(params == NULL || (sscanf(params, fmt, &start, &end) != nparams))
    {
        start = 1;
        end = 100;
    }

    printf("Scanning I2C bus from %d to %d...\n", start, end);
    if(start > 128 || end > 128)
        return CMD_ERROR_SYNTAX;

    gs_gssb_bus_scan((uint8_t)start, (uint8_t)end, 100, devices);

    for (addr = (uint8_t)start; addr < (uint8_t)end; addr++) {
        if (devices[addr] == GS_OK) {
            printf("\tFound device at: %#X (%d)\r\n", addr, addr);
        }
    }

    return CMD_ERROR_NONE;
}

int gssb_read_sunsensor(char *fmt, char *params, int nparams)
{
    uint16_t sun[4];
    int i;

    /* Start sampling */
    if (gs_gssb_sun_sample_sensor(i2c_addr, i2c_timeout_ms) != GS_OK)
        return CMD_ERROR_FAIL;

    /* Wait for result to be ready */
    osDelay(30);
    if (gs_gssb_sun_read_sensor_samples(i2c_addr, i2c_timeout_ms, sun) != GS_OK)
        return CMD_ERROR_FAIL;

    printf("\r\n ---- Values ----\r\n");
    for (i = 0; i < 4; i++) {
        printf("Data %d: %d\r\n", i, sun[i]);
    }

    return CMD_ERROR_NONE;
}

int gssb_get_temp(char *fmt, char *params, int nparams)
{
    float temp;

    /* Command ADC to sample temp */
    if (gs_gssb_sun_sample_temp(i2c_addr, i2c_timeout_ms) != GS_OK)
        return CMD_ERROR_FAIL;

    /* Wait for conversion to finish and then read result */
    osDelay(20);
    if (gs_gssb_sun_get_temp(i2c_addr, i2c_timeout_ms, &temp) != GS_OK)
        return CMD_ERROR_FAIL;

    printf("Temp: %f\r\n", temp);

    return CMD_ERROR_NONE;
}

int gssb_sunsensor_conf(char *fmt, char *params, int nparams)
{
    int conf;

    if (params == NULL)
        return CMD_ERROR_SYNTAX;

    //conf = atoi(ctx->argv[1]);
    if(sscanf(params, fmt, &conf) == nparams) {
        if (gs_gssb_sun_sensor_conf(i2c_addr, i2c_timeout_ms, (uint16_t)conf) != GS_OK)
            return CMD_ERROR_FAIL;
        return CMD_ERROR_NONE;
    }
    return  CMD_ERROR_SYNTAX;
}

int gssb_sunsensor_conf_save(char *fmt, char *params, int nparams)
{
    if (gs_gssb_sun_sensor_conf_save(i2c_addr, i2c_timeout_ms) != GS_OK)
        return CMD_ERROR_FAIL;

    return CMD_ERROR_NONE;
}

int gssb_set_i2c_addr(char *fmt, char *params, int nparams)
{
    int new_i2c_addr;
    uint32_t uuid;

    if (params == NULL)
        return CMD_ERROR_SYNTAX;

    //new_i2c_addr = atoi(ctx->argv[1]);
    if(sscanf(params, fmt, &new_i2c_addr) != nparams)
        return CMD_ERROR_SYNTAX;

    // Set new i2c address
    printf("Setting address to %#X...", (uint8_t)new_i2c_addr);
    gs_gssb_set_i2c_addr(i2c_addr, i2c_timeout_ms, (uint8_t)new_i2c_addr);

    // Check if the new i2c address works
    if (gs_gssb_get_uuid(new_i2c_addr, i2c_timeout_ms, &uuid) != GS_OK) {
        return CMD_ERROR_FAIL;
    }
    printf("Changed address from %hhu to %hhu\n\ron dev: %" PRIu32 "\n\n\rWorking address is %hhu\n\r", i2c_addr,
            new_i2c_addr, uuid, new_i2c_addr);
    i2c_addr = new_i2c_addr;

    return CMD_ERROR_NONE;
}

int gssb_commit_i2c_addr(char *fmt, char *params, int nparams)
{

    if (gs_gssb_commit_i2c_addr(i2c_addr, i2c_timeout_ms) != GS_OK)
        return CMD_ERROR_FAIL;

    return CMD_ERROR_NONE;
}

int gssb_get_version(char *fmt, char *params, int nparams)
{

    uint8_t rx_buff[20];

    if (gs_gssb_get_version(i2c_addr, i2c_timeout_ms, rx_buff, 20) != GS_OK)
        return CMD_ERROR_FAIL;

    rx_buff[19] = '\0';
    printf("Git version: %s\r\n", (char *) rx_buff);

    return CMD_ERROR_NONE;
}

int gssb_get_uuid(char *fmt, char *params, int nparams)
{
    uint32_t uuid;

    if (gs_gssb_get_uuid(i2c_addr, i2c_timeout_ms, &uuid) != GS_OK)
        return CMD_ERROR_FAIL;
    printf("Device UUID: %"PRIu32"\r\n", uuid);

    return CMD_ERROR_NONE;
}

int gssb_get_model(char *fmt, char *params, int nparams)
{
    gs_gssb_model_t model;

    if (gs_gssb_get_model(i2c_addr, i2c_timeout_ms, &model) != GS_OK)
        return CMD_ERROR_FAIL;
    printf("Model: ");
    switch(model){
        case GS_GSSB_MODEL_MSP:
            printf("MSP\n\r");
            break;
        case GS_GSSB_MODEL_AR6:
            printf("AR6\n\r");
            break;
        case GS_GSSB_MODEL_ISTAGE:
            printf("Interstage\n\r");
            break;
        case GS_GSSB_MODEL_ANT6:
            printf("ANT6\n\r");
            break;
        case GS_GSSB_MODEL_I4:
            printf("I4\n\r");
            break;
        default:
            printf("Unknown\r\n");
    }
    return CMD_ERROR_NONE;
}

int gssb_interstage_temp(char *fmt, char *params, int nparams)
{
    float temp;

    if (gs_gssb_istage_get_temp(i2c_addr, i2c_timeout_ms, &temp) != GS_OK)
        return CMD_ERROR_FAIL;

    printf("Temp: %f\r\n", temp);

    return CMD_ERROR_NONE;
}

int gssb_msp_outside_temp(char *fmt, char *params, int nparams)
{
    int16_t temp;

    if (gs_gssb_msp_get_outside_temp(i2c_addr, i2c_timeout_ms, &temp) != GS_OK)
        return CMD_ERROR_FAIL;

    printf("Outside temp: %i\r\n", temp);

    return CMD_ERROR_NONE;
}

//FIXME: Remove?
int gssb_msp_outside_temp_calibrate(char *fmt, char *params, int nparams)
{
    uint16_t current, resistor;
    int curr, res;

    if (params == NULL)
        return CMD_ERROR_SYNTAX;

    if(sscanf(params, fmt, &curr, &res) != nparams)
        return CMD_ERROR_SYNTAX;

    if ((curr > 400000) || (curr < 100000)) {
        printf("Current of %d out of range [100000 - 400000]\r\n", curr);
        return CMD_ERROR_SYNTAX;
    } else {
        /* Current is typed in nano Ampere but send in 10⁻⁸ Ampere to fit in 16 bit*/
        current = (uint16_t) (curr / 10);
    }

    if ((res > 200000) || (res < 50000)) {
        printf("Resistance of %d out of range [50000 - 200000]\r\n", res);
        return CMD_ERROR_SYNTAX;
    } else {
        /* Resistance is typed in mili Ohm but send in centi Ohm to fit in 16 bit */
        resistor = (uint16_t) (res / 10);
    }

    if (gs_gssb_msp_calibrate_outside_temp(i2c_addr, i2c_timeout_ms, current, resistor) != GS_OK)
        return CMD_ERROR_FAIL;

    return CMD_ERROR_NONE;
}


int gssb_internal_temp(char *fmt, char *params, int nparams)
{
    int16_t temp;

    if (gs_gssb_istage_get_internal_temp(i2c_addr, i2c_timeout_ms, &temp) != GS_OK)
        return CMD_ERROR_FAIL;

    printf("Internal temp: %i\r\n", temp);

    return CMD_ERROR_NONE;
}


int gssb_interstage_burn(char *fmt, char *params, int nparams)
{
    if (gs_gssb_istage_burn(i2c_addr, i2c_timeout_ms) != GS_OK)
        return CMD_ERROR_FAIL;

    printf("Istage burning...\n");
    return CMD_ERROR_NONE;
}


int gssb_common_sun_voltage(char *fmt, char *params, int nparams)
{
    uint16_t voltage;

    if (gs_gssb_istage_get_sun_voltage(i2c_addr, i2c_timeout_ms, &voltage) != GS_OK)
        return CMD_ERROR_FAIL;

    printf("Sun voltage: %i\r\n", voltage);

    return CMD_ERROR_NONE;
}

int gssb_interstage_get_burn_settings(char *fmt, char *params, int nparams)
{
    gs_gssb_istage_burn_settings_t settings;

    /* When the command is called with out arguments then print the settings */
    if (params == NULL || nparams == 0) {

        if (gs_gssb_istage_get_burn_settings(i2c_addr, i2c_timeout_ms, &settings) != GS_OK)
            return CMD_ERROR_FAIL;
        if (settings.status == 2)
            printf("\r\nDeploy mode:\t\t ARMED AUTO\r\n");
        else
            printf("\r\nDeploy mode:\t\t NOT ARMED\r\n");
        printf("Deploy delay from boot:\t %"PRIu16" s\r\n", settings.short_cnt_down);

        printf("\r\n-------- Deploy algorithm config ----------\r\n");
        printf("Knife on time:\t\t %i ms\r\n", settings.std_time_ms);
        printf("Increment:\t\t %i ms\r\n", settings.increment_ms);
        printf("Max repeats:\t\t %i\r\n", settings.max_repeat);
        printf("Repeat time:\t\t %i s\r\n", settings.rep_time_s);
        printf("Switch polarity:\t %"PRIu8"\r\n", settings.switch_polarity);
        printf("Settings locked:\t %"PRIu8"\r\n", settings.locked);

    /* Else set settings */
    }else {
        return CMD_ERROR_SYNTAX;
    }

    return CMD_ERROR_NONE;
}


int gssb_interstage_set_burn_settings(char *fmt, char *params, int nparams)
{
    gs_gssb_istage_burn_settings_t settings;
    int std_time, increment_ms, short_cnt_down, max_repeat, rep_time_s;
    int switch_polarity, reboot_deploy_cnt;

    if (sscanf(params, fmt, &std_time, &increment_ms, &short_cnt_down,
                      &max_repeat, &rep_time_s, &switch_polarity, &reboot_deploy_cnt) == nparams) {
        /* First fetch settings and check that the interstage is unlocked and if it
         * is not then print warning about the settings cannot be changed */
        if (gs_gssb_istage_get_burn_settings(i2c_addr, i2c_timeout_ms, &settings) != GS_OK)
            return CMD_ERROR_FAIL;

        /* Check settings range */
        //int tmp;
        //tmp = atoi(ctx->argv[1]);
        if ((std_time > 65535) || (std_time < 0)) {
            printf("Knife on time of %d out of range [0 - 65535]\r\n", std_time);
            return CMD_ERROR_SYNTAX;
        } else {
            settings.std_time_ms = (uint16_t) std_time;
        }

        //tmp = atoi(ctx->argv[2]);
        if ((increment_ms > 65535) || (increment_ms < 0)) {
            printf("Increment of %d out of range [0 - 65535]\r\n", increment_ms);
            return CMD_ERROR_SYNTAX;
        } else {
            settings.increment_ms = (uint16_t) increment_ms;
        }

        //tmp = atoi(ctx->argv[3]);
        if ((short_cnt_down > 65535) || (short_cnt_down < 0)) {
            printf("Auto deploy delay of %d out of range [0 - 65535]\r\n", short_cnt_down);
            return CMD_ERROR_SYNTAX;
        } else {
            settings.short_cnt_down = (uint16_t) short_cnt_down;
        }

        //tmp = atoi(ctx->argv[4]);
        if ((max_repeat > 255) || (max_repeat < 0)) {
            printf("Max repeats of %d out of range [0 - 255]\r\n", max_repeat);
            return CMD_ERROR_SYNTAX;
        } else {
            settings.max_repeat = (uint8_t) max_repeat;
        }

        //tmp = atoi(ctx->argv[5]);
        if ((rep_time_s > 255) || (rep_time_s < 0)) {
            printf("Time between repeats of %d out of range [0 - 255]\r\n", rep_time_s);
            return CMD_ERROR_SYNTAX;
        } else {
            settings.rep_time_s = (uint8_t) rep_time_s;
        }

        //tmp = atoi(ctx->argv[6]);
        if ((switch_polarity > 1) || (switch_polarity < 0)) {
            printf("Polarity can only be selected to 0 or 1 not %d\r\n", switch_polarity);
            return CMD_ERROR_SYNTAX;
        } else {
            settings.switch_polarity = (uint8_t) switch_polarity;
        }

        //tmp = atoi(ctx->argv[7]);
        if ((reboot_deploy_cnt > 255) || (reboot_deploy_cnt < 0)) {
            printf("Reboot deploy number of %d out of range [0 - 255]\r\n", reboot_deploy_cnt);
            return CMD_ERROR_SYNTAX;
        } else {
            settings.reboot_deploy_cnt = (uint8_t) reboot_deploy_cnt;
        }

        if (gs_gssb_istage_settings_unlock(i2c_addr, i2c_timeout_ms) != GS_OK)
            return CMD_ERROR_FAIL;

        printf("Setting burn settings...\n");
        if (gs_gssb_istage_set_burn_settings(i2c_addr, i2c_timeout_ms, &settings) != GS_OK)
            return CMD_ERROR_FAIL;

        /* We need to have a delay as we write the settings to EEPROM which takes some time */
        osDelay(50);

        printf("Setting burn cnt...\n");
        if (gs_gssb_istage_set_burn_settings_cnt(i2c_addr, i2c_timeout_ms, &settings) != GS_OK)
            return CMD_ERROR_FAIL;

        /* We need to have a delay as we write the settings to EEPROM which takes some time */
        osDelay(20);

        if (gs_gssb_istage_settings_lock(i2c_addr, i2c_timeout_ms) != GS_OK) {
            printf("\r\n Warning could not lock settings after they was unlocked\r\n");
            return CMD_ERROR_FAIL;
        }

        printf("Resetting interstage...\n");
        gs_gssb_soft_reset(i2c_addr, i2c_timeout_ms);
    } else {
        return CMD_ERROR_SYNTAX;
    }

    return CMD_ERROR_NONE;
}

int gssb_interstage_arm(char *fmt, char *params, int nparams)
{
    uint8_t data = 0;
    int arm_auto = 0;

    if (params == NULL)
        return CMD_ERROR_SYNTAX;

    if(sscanf(params, fmt, &arm_auto) != nparams)
        return CMD_ERROR_SYNTAX;

    if (arm_auto)
        data = 0x04;
    else
        data = 0x08;

    if (gs_gssb_istage_settings_unlock(i2c_addr, i2c_timeout_ms) != GS_OK)
        return CMD_ERROR_FAIL;

    if (gs_gssb_istage_set_arm(i2c_addr, i2c_timeout_ms, data) != GS_OK)
        return CMD_ERROR_FAIL;

    if (gs_gssb_istage_settings_lock(i2c_addr, i2c_timeout_ms) != GS_OK) {
        printf("\r\n Warning could not lock settings after they was unlocked\r\n");
        return CMD_ERROR_FAIL;
    }

    return CMD_ERROR_NONE;
}


int gssb_interstage_state(char *fmt, char *params, int nparams)
{
    int armed_manual = 0;

    if (params == NULL)
        return CMD_ERROR_SYNTAX;

    //armed_manual = atoi(ctx->argv[1]);
    if(sscanf(params, fmt, &armed_manual) != nparams)
        return CMD_ERROR_SYNTAX;

    if (gs_gssb_istage_settings_unlock(i2c_addr, i2c_timeout_ms) != GS_OK) {
        LOGE(tag,"Error unlocking istage settings\n");
        return CMD_ERROR_FAIL;
    }

    if (gs_gssb_istage_set_state(i2c_addr, i2c_timeout_ms, (uint8_t)armed_manual) != GS_OK) {
        LOGE(tag,"Error setting istage state\n");
        return CMD_ERROR_FAIL;
    }

    if (gs_gssb_istage_settings_lock(i2c_addr, i2c_timeout_ms) != GS_OK) {
        LOGE(tag, "Warning could not lock settings after they was unlocked\n");
        return CMD_ERROR_FAIL;
    }

    return CMD_ERROR_NONE;
}


int gssb_interstage_settings_unlock(char *fmt, char *params, int nparams)
{
    int unlock;
    if (sscanf(params, fmt, &unlock) != nparams)
        return CMD_ERROR_SYNTAX;

    if (unlock) {
        if (gs_gssb_istage_settings_unlock(i2c_addr, i2c_timeout_ms) != GS_OK)
            return CMD_ERROR_FAIL;
    } else {
        if (gs_gssb_istage_settings_lock(i2c_addr, i2c_timeout_ms) != GS_OK)
            return CMD_ERROR_FAIL;
    }
    printf("Parameters unlock: %d", unlock);
    return CMD_ERROR_NONE;
}


int gssb_soft_reset(char *fmt, char *params, int nparams)
{
    printf("Resetting device %#X...", i2c_addr);
    if (gs_gssb_soft_reset(i2c_addr, i2c_timeout_ms) != GS_OK)
        return CMD_ERROR_FAIL;

    return CMD_ERROR_NONE;
}


int gssb_interstage_get_status(char *fmt, char *params, int nparams)
{
    gs_gssb_istage_status_t status;
    const char *state_str;
    
    if (gs_gssb_istage_status(i2c_addr, i2c_timeout_ms, &status) != GS_OK)
        return CMD_ERROR_FAIL;

    switch (status.state) {
        case 0:
            state_str = "Not armed";
            break;
        case 1:
            state_str = "Armed for manual deploy";
            break;
        case 2:
            state_str = "Armed for automatical deploy";
            break;
        case 3:
        case 4:
            state_str = "Deploying";
            break;
        default:
            state_str = "Unknown state";
            break;
    };

    printf("Current state:\t\t\t\t %s\r\n", state_str);
    if (status.release_status == 1)
        printf("Antenna state:\t\t\t\t %s\r\n", "Released");
    else
        printf("Antenna state:\t\t\t\t %s\r\n", "Not released");
    printf("\r\n");

    printf("Delay till deploy:\t\t\t %"PRIu16" s\r\n", status.deploy_in_s);
    printf("Number of attempts since boot:\t\t %"PRIu8"\r\n", status.number_of_deploys);
    printf("Knife that will be used in next deploy:\t %"PRIu8"\r\n", status.active_knife);
    printf("Total deploy attempts:\t\t\t %"PRIu16"\r\n", status.total_number_of_deploys);
    printf("Reboot deploy cnt:\t\t\t %"PRIu8"\r\n", status.reboot_deploy_cnt);

    return CMD_ERROR_NONE;
}


int gssb_ar6_burn(char *fmt, char *params, int nparams)
{
    int duration;
    if (params == NULL)
        return CMD_ERROR_SYNTAX;
    if (sscanf(params, fmt, &duration) != nparams)
        return CMD_ERROR_SYNTAX;

    if ((duration > 20) || (duration < 0)) {
        printf("Duration out of range [0 - 20]\r\n");
        return CMD_ERROR_SYNTAX;
    }

    if (gs_gssb_ar6_burn(i2c_addr, i2c_timeout_ms, (uint8_t)duration) != GS_OK)
        return CMD_ERROR_FAIL;
    else
        return CMD_ERROR_NONE;
}

// FIXME: Remove?
int gssb_ar6_get_status(char *fmt, char *params, int nparams)
{
    gs_gssb_ar6_status_t status;

    if (gs_gssb_ar6_get_release_status(i2c_addr, i2c_timeout_ms, &status.release) != GS_OK)
        return CMD_ERROR_FAIL;

    printf("Antenna release\n\r");
    if (status.release.state == 1)
        printf("\tBurn state:\t\t\t\t %s\r\n", "Burning");
    else
        printf("\tBurn state:\t\t\t\t %s\r\n", "Idle");

    printf("\tBurn time left:\t\t\t\t %hhu\r\n", status.release.burn_time_left);

    if (status.release.status == 1)
        printf("\tAntenna state:\t\t\t\t %s\r\n", "Released");
    else
        printf("\tAntenna state:\t\t\t\t %s\r\n", "Not released");

    printf("\tBurn attempts:\t\t\t\t %hhu\r\n", status.release.burn_tries);

    if (gs_gssb_ant6_get_backup_status(i2c_addr, i2c_timeout_ms, &status.backup) != GS_OK)
        return CMD_ERROR_FAIL;

    printf("Backup\n\r");
    printf("\tState:\t\t\t\t\t ");
    if (status.backup.state == 0) {
        printf("Finished successful\r\n");
    } else if (status.backup.state == 1) {
        printf("Waiting to deploy\r\n");
        printf("\tSeconds to backup deploy:\t\t %"PRIu32"\r\n", status.backup.seconds_to_deploy);
    } else if (status.backup.state == 2) {
        printf("Deploying\r\n");
    } else if (status.backup.state == 3) {
        printf("Finished unsuccessful\r\n");
    } else if (status.backup.state == 4) {
        printf("Not active\r\n");
    }

    if (gs_gssb_ar6_get_board_status(i2c_addr, i2c_timeout_ms, &status.board) != GS_OK)
        return CMD_ERROR_FAIL;

    printf("Board\n\r");
    printf("\tSeconds since boot:\t\t\t %"PRIu32"\r\n", status.board.seconds_since_boot);
    printf("\tNumber of reboots:\t\t\t %hhu\r\n", status.board.reboot_count);

    return CMD_ERROR_NONE;
}


int gssb_common_burn_channel(char *fmt, char *params, int nparams)
{
    int channel, duration;
    if (params == NULL)
        return CMD_ERROR_SYNTAX;

    if(sscanf(params, fmt, &channel, &duration) != nparams)
        return CMD_ERROR_SYNTAX;

    if ((channel > 10) || (channel < 0)) {
        printf("Channels out of range [0 - 10]\r\n");
        return CMD_ERROR_SYNTAX;
    }

    if ((duration > 60) || (duration < 0)) {
        printf("Duration out of range [0 - 60]\r\n");
        return CMD_ERROR_SYNTAX;
    }

    //Can be ant6 or i4... but we do not want to use the internal API, implement the
    //specific device commands instead
    if (gs_gssb_ant6_burn_channel(i2c_addr, i2c_timeout_ms, (uint8_t)channel, (uint8_t)duration) != GS_OK)
    //if (gs_gssb_i4_burn_channel(i2c_addr, i2c_timeout_ms, channel, duration) != GS_OK)
        return CMD_ERROR_FAIL;
    else
        return CMD_ERROR_NONE;
}


int gssb_common_stop_burn(char *fmt, char *params, int nparams)
{
    if (params == NULL)
        return CMD_ERROR_SYNTAX;

    //Can be ant6 or i4... but we do not want to use the internal API, implement the
    //specific device commands instead
    if (gs_gssb_ant6_stop_burn_all_channels(i2c_addr, i2c_timeout_ms) != GS_OK)
    //if (gs_gssb_i4_stop_burn_all_channels(i2c_addr, i2c_timeout_ms) != GS_OK)
        return CMD_ERROR_FAIL;
    else
        return CMD_ERROR_NONE;
}


int gssb_ant6_get_status_all_channels(char *fmt, char *params, int nparams)
{
    gs_gssb_ant6_status_t status;

    if (gs_gssb_ant6_get_release_status(i2c_addr, i2c_timeout_ms, &status.release) != GS_OK)
        return CMD_ERROR_FAIL;

    printf("Channel 0\n\r");
    if (status.release.channel_0_state == 1)
        printf("\tBurn state:\t\t\t\t %s\r\n", "Burning");
    else
        printf("\tBurn state:\t\t\t\t %s\r\n", "Idle");

    printf("\tBurn time left:\t\t\t\t %hhu\r\n", status.release.channel_0_burn_time_left);

    if (status.release.channel_0_status == 1)
        printf("\tAntenna state:\t\t\t\t %s\r\n", "Released");
    else
        printf("\tAntenna state:\t\t\t\t %s\r\n", "Not released");

    printf("\tBurn attempts:\t\t\t\t %hhu\r\n", status.release.channel_0_burn_tries);

    printf("Channel 1\n\r");
    if (status.release.channel_1_state == 1)
        printf("\tBurn state:\t\t\t\t %s\r\n", "Burning");
    else
        printf("\tBurn state:\t\t\t\t %s\r\n", "Idle");

    printf("\tBurn time left:\t\t\t\t %hhu\r\n", status.release.channel_1_burn_time_left);

    if (status.release.channel_1_status == 1)
        printf("\tAntenna state:\t\t\t\t %s\r\n", "Released");
    else
        printf("\tAntenna state:\t\t\t\t %s\r\n", "Not released");

    printf("\tBurn attempts:\t\t\t\t %hhu\r\n", status.release.channel_1_burn_tries);

    if (gs_gssb_ant6_get_backup_status(i2c_addr, i2c_timeout_ms, &status.backup) != GS_OK)
        return CMD_ERROR_FAIL;

    printf("Backup\n\r");
    printf("\tState:\t\t\t\t\t ");
    if (status.backup.state == 0) {
        printf("Finished successful\r\n");
    } else if (status.backup.state == 1) {
        printf("Waiting to deploy\r\n");
        printf("\tSeconds to backup deploy:\t\t %"PRIu32"\r\n", status.backup.seconds_to_deploy);
    } else if (status.backup.state == 2) {
        printf("Deploying\r\n");
    } else if (status.backup.state == 3) {
        printf("Finished unsuccessful\r\n");
    } else if (status.backup.state == 4) {
        printf("Not active\r\n");
    }

    if (gs_gssb_ant6_get_board_status(i2c_addr, i2c_timeout_ms, &status.board) != GS_OK)
        return CMD_ERROR_FAIL;

    printf("Board\n\r");
    printf("\tSeconds since boot:\t\t\t %"PRIu32"\r\n", status.board.seconds_since_boot);
    printf("\tNumber of reboots:\t\t\t %hhu\r\n", status.board.reboot_count);

    return CMD_ERROR_NONE;
}


int gssb_i4_get_status_all_channels(char *fmt, char *params, int nparams)
{
    gs_gssb_i4_status_t status;

    if (gs_gssb_i4_get_release_status(i2c_addr, i2c_timeout_ms, &status.release) != GS_OK)
        return CMD_ERROR_FAIL;

    printf("Channel 0\n\r");
    if (status.release.channel_0_state == 1)
        printf("\tBurn state:\t\t\t\t %s\r\n", "Burning");
    else
        printf("\tBurn state:\t\t\t\t %s\r\n", "Idle");

    printf("\tBurn time left:\t\t\t\t %hhu\r\n", status.release.channel_0_burn_time_left);

    if (status.release.channel_0_status_0 == 1)
        printf("\tAntenna state:\t\t\t\t %s\r\n", "Released");
    else
        printf("\tAntenna state:\t\t\t\t %s\r\n", "Not released");

    if (status.release.channel_0_status_1 == 2)
        printf("\tAntenna state:\t\t\t\t %s\r\n", "Released");
    else
        printf("\tAntenna state:\t\t\t\t %s\r\n", "Not released");

    printf("\tBurn attempts:\t\t\t\t %hhu\r\n", status.release.channel_0_burn_tries);

    printf("Channel 1\n\r");
    if (status.release.channel_1_state == 1)
        printf("\tBurn state:\t\t\t\t %s\r\n", "Burning");
    else
        printf("\tBurn state:\t\t\t\t %s\r\n", "Idle");

    printf("\tBurn time left:\t\t\t\t %hhu\r\n", status.release.channel_1_burn_time_left);

    if (status.release.channel_1_status_0 == 1)
        printf("\tAntenna state:\t\t\t\t %s\r\n", "Released");
    else
        printf("\tAntenna state:\t\t\t\t %s\r\n", "Not released");

    if (status.release.channel_1_status_1== 2)
        printf("\tAntenna state:\t\t\t\t %s\r\n", "Released");
    else
        printf("\tAntenna state:\t\t\t\t %s\r\n", "Not released");

    printf("\tBurn attempts:\t\t\t\t %hhu\r\n", status.release.channel_1_burn_tries);

    if (gs_gssb_i4_get_backup_status(i2c_addr, i2c_timeout_ms, &status.backup) != GS_OK)
        return CMD_ERROR_FAIL;

    printf("Backup\n\r");
    printf("\tState:\t\t\t\t\t ");
    if (status.backup.state == 0) {
        printf("Finished successful\r\n");
    } else if (status.backup.state == 1) {
        printf("Waiting to deploy\r\n");
        printf("\tSeconds to backup deploy:\t\t %"PRIu32"\r\n", status.backup.seconds_to_deploy);
    } else if (status.backup.state == 2) {
        printf("Deploying\r\n");
    } else if (status.backup.state == 3) {
        printf("Finished unsuccessful\r\n");
    } else if (status.backup.state == 4) {
        printf("Not active\r\n");
    }

    if (gs_gssb_i4_get_board_status(i2c_addr, i2c_timeout_ms, &status.board) != GS_OK)
        return CMD_ERROR_FAIL;

    printf("Board\n\r");
    printf("\tSeconds since boot:\t\t\t %"PRIu32"\r\n", status.board.seconds_since_boot);
    printf("\tNumber of reboots:\t\t\t %hhu\r\n", status.board.reboot_count);

    return CMD_ERROR_NONE;
}


int gssb_common_reset_count(char *fmt, char *params, int nparams)
{
    if (params == NULL)
        return CMD_ERROR_SYNTAX;
    //Can be ant6 or i4... but we do not want to use the internal API, implement the
    //specific device commands instead
    if (gs_gssb_ant6_reset_count(i2c_addr, i2c_timeout_ms) != GS_OK)
    //if (gs_gssb_ar6_reset_count(i2c_addr, i2c_timeout_ms) != GS_OK)
    //if (gs_gssb_i4_reset_count(i2c_addr, i2c_timeout_ms) != GS_OK)
        return CMD_ERROR_FAIL;
    else
        return CMD_ERROR_NONE;
}


int gssb_common_backup_settings(char *fmt, char *params, int nparams)
{
    gs_gssb_backup_settings_t settings;
    int minutes, backup_active, max_burn_duration;

    if (sscanf(params, fmt, &minutes, &backup_active, &max_burn_duration) == nparams) {
        if ((minutes > 5000) || (minutes < 0)) {
            printf("Minutes until deploy out of range [0 - 5000]\r\n");
            return CMD_ERROR_SYNTAX;
        } else {
            settings.minutes = (uint16_t)minutes;
        }

        if ((backup_active != 1) && (backup_active != 0)) {
            printf("Backup active out of range [0 - 1]\r\n");
            return CMD_ERROR_SYNTAX;
        } else {
            settings.backup_active = (uint8_t)backup_active;
        }

        if ((max_burn_duration > 127) || (max_burn_duration < 0)) {
            printf("Max burn duration out of range [0 - 127]\r\n");
            return CMD_ERROR_SYNTAX;
        } else {
            settings.max_burn_duration = (uint8_t)max_burn_duration;
        }

        if (gs_gssb_ant6_set_backup_settings(i2c_addr, i2c_timeout_ms, settings) != GS_OK)
        //if (gs_gssb_ar6_set_backup_settings(i2c_addr, i2c_timeout_ms, settings) != GS_OK)
        //if (gs_gssb_i4_set_backup_settings(i2c_addr, i2c_timeout_ms, settings) != GS_OK)
            return CMD_ERROR_FAIL;

        printf("\n\rReboot board for changes to take effect\n\r");
    } else {
        if (gs_gssb_ant6_get_backup_settings(i2c_addr, i2c_timeout_ms, &settings) != GS_OK)
        //if (gs_gssb_ar6_get_backup_settings(i2c_addr, i2c_timeout_ms, &settings) != GS_OK)
        //if (gs_gssb_i4_get_backup_settings(i2c_addr, i2c_timeout_ms, &settings) != GS_OK)
            return CMD_ERROR_FAIL;
    }

    printf("Backup deploy settings:\n\r\tMinutes until deploy:\t\t\t %hi\n\r\tBackup deployment:\t\t\t ", settings.minutes);
    if (settings.backup_active == 1) {
        printf("Active\n\r");
    } else {
        printf("Not active\n\r");
    }
    printf("\tMax burn duration:\t\t\t %hhu\n\r", settings.max_burn_duration);

    return CMD_ERROR_NONE;
}

int gssb_update_status(char *fmt, char *params, int nparams)
{
    char istage_addr[4] = {0x10, 0x11, 0x12, 0x13};
    gs_gssb_istage_status_t status;
    int deploy_status = 0;
    int rel_status = 0;
    int i = 0;

    for(i=0; i < 4; i++)
    {
        if(gs_gssb_istage_status(istage_addr[i], i2c_timeout_ms, &status) != GS_OK)
        {
            LOGE(tag, "Error reading status from %d", istage_addr[i])
            return CMD_ERROR_FAIL;
        }
        rel_status = status.release_status;
        deploy_status += rel_status;
        LOGI(tag, "Interstage selected: %#x. Released: %d", istage_addr[i], rel_status);
    }

    if(deploy_status >= 0)
    {
        LOGI(tag, "Antennas release status: %d", deploy_status);
        dat_set_system_var(dat_dep_ant_deployed, deploy_status);
        return CMD_OK;
    }
    else
        return CMD_FAIL;
}

int gssb_antenna_release(char *fmt, char *params, int nparams)
{
    int addr, knife_on, knife_off, repeats = 0;

    if(params == NULL)
    {
        LOGE(tag, "Null params");
        return CMD_ERROR;
    }

    int rc = 1;
    gs_gssb_istage_burn_settings_t settings;

    if(sscanf(params, fmt, &addr, &knife_on, &knife_off, &repeats) == nparams)
    {
        //Get current config
        if (gs_gssb_istage_get_burn_settings(addr, i2c_timeout_ms, &settings) != GS_OK)
            return CMD_ERROR_FAIL;

        // Fill parameters
        settings.std_time_ms = knife_on;
        settings.increment_ms = 0;
        settings.short_cnt_down = 0;
        settings.max_repeat = repeats;
        settings.rep_time_s = knife_off;

        // Unlock parameters
        if (gs_gssb_istage_settings_unlock(addr, i2c_timeout_ms) != GS_OK)
            return CMD_ERROR_FAIL;
        // Set parameters
        if (gs_gssb_istage_set_burn_settings(addr, i2c_timeout_ms, &settings) != GS_OK)
            return CMD_ERROR_FAIL;
        /* We need to have a delay as we write the settings to EEPROM which takes some time */
        osDelay(50);
        // "Setting burn cnt...
        if (gs_gssb_istage_set_burn_settings_cnt(addr, i2c_timeout_ms, &settings) != GS_OK)
            return CMD_ERROR_FAIL;
        /* We need to have a delay as we write the settings to EEPROM which takes some time */
        osDelay(20);
        if (gs_gssb_istage_settings_lock(addr, i2c_timeout_ms) != GS_OK)
            return CMD_ERROR_FAIL;
        printf("Resetting interstage...\n");
        gs_gssb_soft_reset(addr, i2c_timeout_ms);
        LOGD(tag, "istage on: %d. Off: %d. Rep: %d. (rc: %d)",
             knife_on, knife_off, repeats, rc);

        // Deploy manual
        if (gs_gssb_istage_settings_unlock(addr, i2c_timeout_ms) != GS_OK)
            return CMD_ERROR_FAIL;
        if (gs_gssb_istage_set_arm(addr, i2c_timeout_ms, 0x08) != GS_OK)
            return CMD_ERROR_FAIL;
        // Burn
        if (gs_gssb_istage_burn(addr, i2c_timeout_ms) != GS_OK)
            return CMD_ERROR_FAIL;
        LOGI(tag, "istage %#x deploying! (rc: %d)", addr, rc);

        return CMD_OK;
    }
    else
    {
        LOGE(tag, "Error parsing params");
        return CMD_ERROR;
    }
}