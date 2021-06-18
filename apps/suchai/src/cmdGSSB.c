/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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
#define CMD_ERROR_SYNTAX  CMD_SYNTAX_ERROR
#define CMD_ERROR_FAIL CMD_ERROR
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

    LOGR(tag, "Set GSSB switch state to: %d %d", vcc_on, vcc2_on);
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

        LOGR(tag, "Selected I2C addr %#X (%d)", i2c_addr, i2c_addr);
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

    LOGR(tag, "Scanning I2C bus from %d to %d...", start, end);
    if(start > 128 || end > 128)
        return CMD_ERROR_SYNTAX;

    gs_gssb_bus_scan((uint8_t)start, (uint8_t)end, 100, devices);

    for (addr = (uint8_t)start; addr < (uint8_t)end; addr++) {
        if (devices[addr] == GS_OK) {
            LOGR(tag, "\tFound device at: %#X (%d)", addr, addr);
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

    for (i = 0; i < 4; i++) {
        LOGR(tag, "GSSB %d sun sensor %d: %d", i2c_addr, i, sun[i]);
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

    LOGR(tag, "GSSB %d temp: %.4f °C", i2c_addr, temp);

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
    LOGR(tag, "Setting address to %#X...", (uint8_t)new_i2c_addr);
    gs_gssb_set_i2c_addr(i2c_addr, i2c_timeout_ms, (uint8_t)new_i2c_addr);

    // Check if the new i2c address works
    if (gs_gssb_get_uuid(new_i2c_addr, i2c_timeout_ms, &uuid) != GS_OK) {
        return CMD_ERROR_FAIL;
    }
    LOGD(tag, "Changed address from %hhu to %hhu\ron dev: %" PRIu32 "\rWorking address is %hhu\r", i2c_addr,
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
    LOGR(tag, "Git version: %s", (char *) rx_buff);

    return CMD_ERROR_NONE;
}

int gssb_get_uuid(char *fmt, char *params, int nparams)
{
    uint32_t uuid;

    if (gs_gssb_get_uuid(i2c_addr, i2c_timeout_ms, &uuid) != GS_OK)
        return CMD_ERROR_FAIL;
    LOGR(tag, "Device UUID: %"PRIu32"", uuid);

    return CMD_ERROR_NONE;
}

int gssb_get_model(char *fmt, char *params, int nparams)
{
    gs_gssb_model_t model;

    if (gs_gssb_get_model(i2c_addr, i2c_timeout_ms, &model) != GS_OK)
        return CMD_ERROR_FAIL;
    switch(model){
        case GS_GSSB_MODEL_MSP:
            LOGR(tag, "Model: MSP");
            break;
        case GS_GSSB_MODEL_AR6:
            LOGR(tag, "Model: AR6");
            break;
        case GS_GSSB_MODEL_ISTAGE:
            LOGR(tag, "Model: Interstage");
            break;
        case GS_GSSB_MODEL_ANT6:
            LOGR(tag, "Model: ANT6");
            break;
        case GS_GSSB_MODEL_I4:
            LOGR(tag, "Model: I4");
            break;
        default:
            LOGR(tag, "Model: Unknown");
    }
    return CMD_ERROR_NONE;
}

int gssb_interstage_temp(char *fmt, char *params, int nparams)
{
    float temp;

    if (gs_gssb_istage_get_temp(i2c_addr, i2c_timeout_ms, &temp) != GS_OK)
        return CMD_ERROR_FAIL;

    LOGR(tag, "Temp: %f", temp);

    return CMD_ERROR_NONE;
}

int gssb_msp_outside_temp(char *fmt, char *params, int nparams)
{
    int16_t temp;

    if (gs_gssb_msp_get_outside_temp(i2c_addr, i2c_timeout_ms, &temp) != GS_OK)
        return CMD_ERROR_FAIL;

    LOGR(tag, "Outside temp: %i", temp);

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
        LOGW(tag, "Current of %d out of range [100000 - 400000]", curr);
        return CMD_ERROR_SYNTAX;
    } else {
        /* Current is typed in nano Ampere but send in 10⁻⁸ Ampere to fit in 16 bit*/
        current = (uint16_t) (curr / 10);
    }

    if ((res > 200000) || (res < 50000)) {
        LOGW(tag, "Resistance of %d out of range [50000 - 200000]", res);
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

    LOGR(tag, "Internal temp: %i", temp);

    return CMD_ERROR_NONE;
}


int gssb_interstage_burn(char *fmt, char *params, int nparams)
{
    if (gs_gssb_istage_burn(i2c_addr, i2c_timeout_ms) != GS_OK)
        return CMD_ERROR_FAIL;

    LOGR(tag, "Istage burning...");
    return CMD_ERROR_NONE;
}


int gssb_common_sun_voltage(char *fmt, char *params, int nparams)
{
    uint16_t voltage;

    if (gs_gssb_istage_get_sun_voltage(i2c_addr, i2c_timeout_ms, &voltage) != GS_OK)
        return CMD_ERROR_FAIL;

    LOGR(tag, "Sun voltage: %i", voltage);

    return CMD_ERROR_NONE;
}

int gssb_interstage_get_burn_settings(char *fmt, char *params, int nparams)
{
    gs_gssb_istage_burn_settings_t settings;

    /* When the command is called with out arguments then print the settings */
    if (params == NULL || nparams == 0) {

        if (gs_gssb_istage_get_burn_settings(i2c_addr, i2c_timeout_ms, &settings) != GS_OK)
            return CMD_ERROR_FAIL;
        if (settings.status == 2){
            LOGR(tag, "Deploy mode:\t\t ARMED AUTO");
        }
        else {
            LOGR(tag, "Deploy mode:\t\t NOT ARMED");
        }
        LOGR(tag, "Deploy delay from boot:\t %"PRIu16" s", settings.short_cnt_down);

        LOGR(tag, "-------- Deploy algorithm config ----------");
        LOGR(tag, "Knife on time:\t\t %i ms", settings.std_time_ms);
        LOGR(tag, "Increment:\t\t %i ms", settings.increment_ms);
        LOGR(tag, "Max repeats:\t\t %i", settings.max_repeat);
        LOGR(tag, "Repeat time:\t\t %i s", settings.rep_time_s);
        LOGR(tag, "Switch polarity:\t %"PRIu8"", settings.switch_polarity);
        LOGR(tag, "Settings locked:\t %"PRIu8"", settings.locked);

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
            LOGE(tag, "Knife on time of %d out of range [0 - 65535]", std_time);
            return CMD_ERROR_SYNTAX;
        } else {
            settings.std_time_ms = (uint16_t) std_time;
        }

        //tmp = atoi(ctx->argv[2]);
        if ((increment_ms > 65535) || (increment_ms < 0)) {
            LOGE(tag, "Increment of %d out of range [0 - 65535]", increment_ms);
            return CMD_ERROR_SYNTAX;
        } else {
            settings.increment_ms = (uint16_t) increment_ms;
        }

        //tmp = atoi(ctx->argv[3]);
        if ((short_cnt_down > 65535) || (short_cnt_down < 0)) {
            LOGE(tag, "Auto deploy delay of %d out of range [0 - 65535]", short_cnt_down);
            return CMD_ERROR_SYNTAX;
        } else {
            settings.short_cnt_down = (uint16_t) short_cnt_down;
        }

        //tmp = atoi(ctx->argv[4]);
        if ((max_repeat > 255) || (max_repeat < 0)) {
            LOGE(tag, "Max repeats of %d out of range [0 - 255]", max_repeat);
            return CMD_ERROR_SYNTAX;
        } else {
            settings.max_repeat = (uint8_t) max_repeat;
        }

        //tmp = atoi(ctx->argv[5]);
        if ((rep_time_s > 255) || (rep_time_s < 0)) {
            LOGE(tag, "Time between repeats of %d out of range [0 - 255]", rep_time_s);
            return CMD_ERROR_SYNTAX;
        } else {
            settings.rep_time_s = (uint8_t) rep_time_s;
        }

        //tmp = atoi(ctx->argv[6]);
        if ((switch_polarity > 1) || (switch_polarity < 0)) {
            LOGE(tag,"Polarity can only be selected to 0 or 1 not %d", switch_polarity);
            return CMD_ERROR_SYNTAX;
        } else {
            settings.switch_polarity = (uint8_t) switch_polarity;
        }

        //tmp = atoi(ctx->argv[7]);
        if ((reboot_deploy_cnt > 255) || (reboot_deploy_cnt < 0)) {
            LOGE(tag,"Reboot deploy number of %d out of range [0 - 255]", reboot_deploy_cnt);
            return CMD_ERROR_SYNTAX;
        } else {
            settings.reboot_deploy_cnt = (uint8_t) reboot_deploy_cnt;
        }

        if (gs_gssb_istage_settings_unlock(i2c_addr, i2c_timeout_ms) != GS_OK)
            return CMD_ERROR_FAIL;

        LOGI(tag, "Setting burn settings...");
        if (gs_gssb_istage_set_burn_settings(i2c_addr, i2c_timeout_ms, &settings) != GS_OK)
            return CMD_ERROR_FAIL;

        /* We need to have a delay as we write the settings to EEPROM which takes some time */
        osDelay(50);

        LOGI(tag, "Setting burn cnt...");
        if (gs_gssb_istage_set_burn_settings_cnt(i2c_addr, i2c_timeout_ms, &settings) != GS_OK)
            return CMD_ERROR_FAIL;

        /* We need to have a delay as we write the settings to EEPROM which takes some time */
        osDelay(20);

        if (gs_gssb_istage_settings_lock(i2c_addr, i2c_timeout_ms) != GS_OK) {
            LOGW(tag, " Warning could not lock settings after they was unlocked");
            return CMD_ERROR_FAIL;
        }

        LOGI(tag,"Resetting interstage...");
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
        LOGW(tag, " Warning could not lock settings after they was unlocked");
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
        LOGE(tag,"Error unlocking istage settings");
        return CMD_ERROR_FAIL;
    }

    if (gs_gssb_istage_set_state(i2c_addr, i2c_timeout_ms, (uint8_t)armed_manual) != GS_OK) {
        LOGE(tag,"Error setting istage state");
        return CMD_ERROR_FAIL;
    }

    if (gs_gssb_istage_settings_lock(i2c_addr, i2c_timeout_ms) != GS_OK) {
        LOGE(tag, "Warning could not lock settings after they was unlocked");
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
    LOGR(tag, "Parameters unlock: %d", unlock);
    return CMD_ERROR_NONE;
}


int gssb_soft_reset(char *fmt, char *params, int nparams)
{
    LOGR(tag, "Resetting device %#X...", i2c_addr);
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

    LOGR(tag, "Current state:\t\t\t\t %s", state_str);
    if (status.release_status == 1) {
        LOGR(tag, "Antenna state:\t\t\t\t %s", "Released");
    }
    else {
        LOGR(tag, "Antenna state:\t\t\t\t %s", "Not released");
    }

    LOGR(tag, "Delay till deploy:\t\t\t %"PRIu16" s", status.deploy_in_s);
    LOGR(tag, "Number of attempts since boot:\t\t %"PRIu8"", status.number_of_deploys);
    LOGR(tag, "Knife that will be used in next deploy:\t %"PRIu8"", status.active_knife);
    LOGR(tag, "Total deploy attempts:\t\t\t %"PRIu16"", status.total_number_of_deploys);
    LOGR(tag, "Reboot deploy cnt:\t\t\t %"PRIu8"", status.reboot_deploy_cnt);

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
        LOGE(tag, "Duration out of range [0 - 20]");
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

    LOGR(tag, "Antenna release\r");
    if (status.release.state == 1) {
        LOGR(tag, "\tBurn state:\t\t\t\t %s", "Burning");
    }
    else {
        LOGR(tag, "\tBurn state:\t\t\t\t %s", "Idle");
    }

    LOGR(tag, "\tBurn time left:\t\t\t\t %hhu", status.release.burn_time_left);

    if (status.release.status == 1) {
        LOGR(tag, "\tAntenna state:\t\t\t\t %s", "Released");
    }
    else {
        LOGR(tag, "\tAntenna state:\t\t\t\t %s", "Not released");
    }

    LOGR(tag, "\tBurn attempts:\t\t\t\t %hhu", status.release.burn_tries);

    if (gs_gssb_ant6_get_backup_status(i2c_addr, i2c_timeout_ms, &status.backup) != GS_OK)
        return CMD_ERROR_FAIL;

    LOGR(tag, "Backup\r");
    LOGR(tag, "\tState:\t\t\t\t\t ");
    if (status.backup.state == 0) {
        LOGR(tag, "Finished successful");
    } else if (status.backup.state == 1) {
        LOGR(tag, "Waiting to deploy");
        LOGR(tag, "\tSeconds to backup deploy:\t\t %"PRIu32"", status.backup.seconds_to_deploy);
    } else if (status.backup.state == 2) {
        LOGR(tag, "Deploying");
    } else if (status.backup.state == 3) {
        LOGR(tag, "Finished unsuccessful");
    } else if (status.backup.state == 4) {
        LOGR(tag, "Not active");
    }

    if (gs_gssb_ar6_get_board_status(i2c_addr, i2c_timeout_ms, &status.board) != GS_OK)
        return CMD_ERROR_FAIL;

    LOGR(tag, "Board\r");
    LOGR(tag, "\tSeconds since boot:\t\t\t %"PRIu32"", status.board.seconds_since_boot);
    LOGR(tag, "\tNumber of reboots:\t\t\t %hhu", status.board.reboot_count);

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
        LOGW(tag, "Channels out of range [0 - 10]");
        return CMD_ERROR_SYNTAX;
    }

    if ((duration > 60) || (duration < 0)) {
        LOGW(tag, "Duration out of range [0 - 60]");
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

    LOGR(tag, "Channel 0\r");
    if (status.release.channel_0_state == 1) {
        LOGR(tag, "\tBurn state:\t\t\t\t %s", "Burning");
    }
    else {
        LOGR(tag, "\tBurn state:\t\t\t\t %s", "Idle");
    }

    LOGR(tag, "\tBurn time left:\t\t\t\t %hhu", status.release.channel_0_burn_time_left);

    if (status.release.channel_0_status == 1) {
        LOGR(tag, "\tAntenna state:\t\t\t\t %s", "Released");
    }
    else {
        LOGR(tag, "\tAntenna state:\t\t\t\t %s", "Not released");
    }

    LOGR(tag, "\tBurn attempts:\t\t\t\t %hhu", status.release.channel_0_burn_tries);

    LOGR(tag, "Channel 1\r");
    if (status.release.channel_1_state == 1) {
        LOGR(tag, "\tBurn state:\t\t\t\t %s", "Burning");
    }
    else {
        LOGR(tag, "\tBurn state:\t\t\t\t %s", "Idle");
    }

    LOGR(tag, "\tBurn time left:\t\t\t\t %hhu", status.release.channel_1_burn_time_left);

    if (status.release.channel_1_status == 1) {
        LOGR(tag, "\tAntenna state:\t\t\t\t %s", "Released");
    }
    else {
        LOGR(tag, "\tAntenna state:\t\t\t\t %s", "Not released");
    }

    LOGR(tag, "\tBurn attempts:\t\t\t\t %hhu", status.release.channel_1_burn_tries);

    if (gs_gssb_ant6_get_backup_status(i2c_addr, i2c_timeout_ms, &status.backup) != GS_OK)
        return CMD_ERROR_FAIL;

    LOGR(tag, "Backup\r");
    LOGR(tag, "\tState:\t\t\t\t\t ");
    if (status.backup.state == 0) {
        LOGR(tag, "Finished successful");
    } else if (status.backup.state == 1) {
        LOGR(tag, "Waiting to deploy");
        LOGR(tag, "\tSeconds to backup deploy:\t\t %"PRIu32"", status.backup.seconds_to_deploy);
    } else if (status.backup.state == 2) {
        LOGR(tag, "Deploying");
    } else if (status.backup.state == 3) {
        LOGR(tag, "Finished unsuccessful");
    } else if (status.backup.state == 4) {
        LOGR(tag, "Not active");
    }

    if (gs_gssb_ant6_get_board_status(i2c_addr, i2c_timeout_ms, &status.board) != GS_OK)
        return CMD_ERROR_FAIL;

    LOGR(tag, "Board\r");
    LOGR(tag, "\tSeconds since boot:\t\t\t %"PRIu32"", status.board.seconds_since_boot);
    LOGR(tag, "\tNumber of reboots:\t\t\t %hhu", status.board.reboot_count);

    return CMD_ERROR_NONE;
}


int gssb_i4_get_status_all_channels(char *fmt, char *params, int nparams)
{
    gs_gssb_i4_status_t status;

    if (gs_gssb_i4_get_release_status(i2c_addr, i2c_timeout_ms, &status.release) != GS_OK)
        return CMD_ERROR_FAIL;

    LOGR(tag, "Channel 0\r");
    if (status.release.channel_0_state == 1) {
        LOGR(tag, "\tBurn state:\t\t\t\t %s", "Burning");
    }
    else {
        LOGR(tag, "\tBurn state:\t\t\t\t %s", "Idle");
    }

    LOGR(tag, "\tBurn time left:\t\t\t\t %hhu", status.release.channel_0_burn_time_left);

    if (status.release.channel_0_status_0 == 1) {
        LOGR(tag, "\tAntenna state:\t\t\t\t %s", "Released");
    }
    else {
        LOGR(tag, "\tAntenna state:\t\t\t\t %s", "Not released");
    }

    if (status.release.channel_0_status_1 == 2) {
        LOGR(tag, "\tAntenna state:\t\t\t\t %s", "Released");
    }
    else {
        LOGR(tag, "\tAntenna state:\t\t\t\t %s", "Not released");
    }

    LOGR(tag, "\tBurn attempts:\t\t\t\t %hhu", status.release.channel_0_burn_tries);

    LOGR(tag, "Channel 1\r");
    if (status.release.channel_1_state == 1) {
        LOGR(tag, "\tBurn state:\t\t\t\t %s", "Burning");
    }
    else {
        LOGR(tag, "\tBurn state:\t\t\t\t %s", "Idle");
    }

    LOGR(tag, "\tBurn time left:\t\t\t\t %hhu", status.release.channel_1_burn_time_left);

    if (status.release.channel_1_status_0 == 1) {
        LOGR(tag, "\tAntenna state:\t\t\t\t %s", "Released");
    }
    else {
        LOGR(tag, "\tAntenna state:\t\t\t\t %s", "Not released");
    }

    if (status.release.channel_1_status_1== 2) {
        LOGR(tag, "\tAntenna state:\t\t\t\t %s", "Released");
    }
    else {
        LOGR(tag, "\tAntenna state:\t\t\t\t %s", "Not released");
    }

    LOGR(tag, "\tBurn attempts:\t\t\t\t %hhu", status.release.channel_1_burn_tries);

    if (gs_gssb_i4_get_backup_status(i2c_addr, i2c_timeout_ms, &status.backup) != GS_OK)
        return CMD_ERROR_FAIL;

    LOGR(tag, "Backup\r");
    LOGR(tag, "\tState:\t\t\t\t\t ");
    if (status.backup.state == 0) {
        LOGR(tag, "Finished successful");
    } else if (status.backup.state == 1) {
        LOGR(tag, "Waiting to deploy");
        LOGR(tag, "\tSeconds to backup deploy:\t\t %"PRIu32"", status.backup.seconds_to_deploy);
    } else if (status.backup.state == 2) {
        LOGR(tag, "Deploying");
    } else if (status.backup.state == 3) {
        LOGR(tag, "Finished unsuccessful");
    } else if (status.backup.state == 4) {
        LOGR(tag, "Not active");
    }

    if (gs_gssb_i4_get_board_status(i2c_addr, i2c_timeout_ms, &status.board) != GS_OK)
        return CMD_ERROR_FAIL;

    LOGR(tag, "Board\r");
    LOGR(tag, "\tSeconds since boot:\t\t\t %"PRIu32"", status.board.seconds_since_boot);
    LOGR(tag, "\tNumber of reboots:\t\t\t %hhu", status.board.reboot_count);

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
            LOGE(tag, "Minutes until deploy out of range [0 - 5000]");
            return CMD_ERROR_SYNTAX;
        } else {
            settings.minutes = (uint16_t)minutes;
        }

        if ((backup_active != 1) && (backup_active != 0)) {
            LOGE(tag, "Backup active out of range [0 - 1]");
            return CMD_ERROR_SYNTAX;
        } else {
            settings.backup_active = (uint8_t)backup_active;
        }

        if ((max_burn_duration > 127) || (max_burn_duration < 0)) {
            LOGE(tag, "Max burn duration out of range [0 - 127]");
            return CMD_ERROR_SYNTAX;
        } else {
            settings.max_burn_duration = (uint8_t)max_burn_duration;
        }

        if (gs_gssb_ant6_set_backup_settings(i2c_addr, i2c_timeout_ms, settings) != GS_OK)
        //if (gs_gssb_ar6_set_backup_settings(i2c_addr, i2c_timeout_ms, settings) != GS_OK)
        //if (gs_gssb_i4_set_backup_settings(i2c_addr, i2c_timeout_ms, settings) != GS_OK)
            return CMD_ERROR_FAIL;

        LOGR(tag, "\rReboot board for changes to take effect\r");
    } else {
        if (gs_gssb_ant6_get_backup_settings(i2c_addr, i2c_timeout_ms, &settings) != GS_OK)
        //if (gs_gssb_ar6_get_backup_settings(i2c_addr, i2c_timeout_ms, &settings) != GS_OK)
        //if (gs_gssb_i4_get_backup_settings(i2c_addr, i2c_timeout_ms, &settings) != GS_OK)
            return CMD_ERROR_FAIL;
    }

    LOGR(tag, "Backup deploy settings:\r\tMinutes until deploy:\t\t\t %hi\r\tBackup deployment:\t\t\t ", settings.minutes);
    if (settings.backup_active == 1) {
        LOGR(tag, "Active\r");
    } else {
        LOGR(tag, "Not active\r");
    }
    LOGR(tag, "\tMax burn duration:\t\t\t %hhu\r", settings.max_burn_duration);

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
        return CMD_ERROR;
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
        LOGI(tag, "Resetting interstage...");
        gs_gssb_soft_reset(addr, i2c_timeout_ms);
        LOGR(tag, "istage on: %d. Off: %d. Rep: %d. (rc: %d)",
             knife_on, knife_off, repeats, rc);

        // Deploy manual
        if (gs_gssb_istage_settings_unlock(addr, i2c_timeout_ms) != GS_OK)
            return CMD_ERROR_FAIL;
        if (gs_gssb_istage_set_arm(addr, i2c_timeout_ms, 0x08) != GS_OK)
            return CMD_ERROR_FAIL;
        // Burn
        if (gs_gssb_istage_burn(addr, i2c_timeout_ms) != GS_OK)
            return CMD_ERROR_FAIL;
        LOGR(tag, "GSSB istage %d deploying! (rc: %d)", addr, rc);

        return CMD_OK;
    }
    else
    {
        LOGE(tag, "Error parsing params");
        return CMD_ERROR;
    }
}