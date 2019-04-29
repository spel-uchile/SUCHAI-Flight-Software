/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2018, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
 *      Copyright 2018, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *      Copyright 2018, Matias Ramirez Martinez, nicoram.mt@gmail.com
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
 
#include "cmdOBC.h"

static const char* tag = "cmdOBC";

void cmd_obc_init(void)
{
    cmd_add("ident", obc_ident, "", 0);
    cmd_add("debug_obc", obc_debug, "%d", 1);
    cmd_add("reset", obc_reset, "", 0);
    cmd_add("get_mem", obc_get_os_memory, "", 0);
    cmd_add("set_time", obc_set_time,"%d",1);
    cmd_add("show_time", obc_show_time,"%d",1);
    cmd_add("reset_wdt", obc_reset_wdt, "", 0);
    cmd_add("system", obc_system, "%s", 1);
    cmd_add("set_pwm_duty", obc_set_pwm_duty, "%d %d", 2);

#ifdef NANOMIND
    cmd_add("istage_pwr", gssb_pwr, "%d %d", 2);
    cmd_add("istage_select", gssb_select_dev, "%d", 1);
    cmd_add("istage_set_addr", gssb_set_addr, "%d", 1);
    cmd_add("istage_ident", gssb_ident, "", 0);
    cmd_add("istage_get_config", gssb_istage_get_settings, "", 0);
    cmd_add("istage_set_config", gssb_istage_settings, "%d %d %d %d %d %d %d", 7);
    cmd_add("istage_reset", gssb_istage_reboot, "", 0);
    cmd_add("istage_deploy", gssb_istage_deploy, "", 0);
    cmd_add("istage_unlock", gssb_unlock, "", 0);
    cmd_add("istage_set_arm", gssb_istage_arm, "%d", 1);
    cmd_add("istage_get_sensors", gssb_istage_sensors, "", 0);
    cmd_add("istage_get_status", gssb_istage_status, "", 0);
#endif
}

int obc_ident(char* fmt, char* params, int nparams)
{
    printf("Name: %s\nID  : %d\nVer : %s\nNode: %d\n",
            SCH_NAME, SCH_DEVICE_ID, SCH_SW_VERSION, SCH_COMM_ADDRESS);
    return CMD_OK;
}

int obc_debug(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Parameter null");
        return CMD_FAIL;
    }
    int dbg_type;
    if(sscanf(params, fmt, &dbg_type) == nparams)
    {
        #ifdef AVR32
            switch(dbg_type)
            {
                case 0: LED_Toggle(LED0); break;
                case 1: LED_Toggle(LED1); break;
                case 2: LED_Toggle(LED2); break;
                case 3: LED_Toggle(LED3); break;
                default: LED_Toggle(LED0);
            }
        #endif
        #ifdef NANOMIND
            if(dbg_type <= LED_A)
                led_toggle((led_names_t)dbg_type);
        #endif
        #ifdef ESP32
            static int level = 0;
            level = ~level;
            gpio_set_level(BLINK_GPIO, level);
        #endif
        #ifdef LINUX
            LOGV(tag, "OBC Debug (%d)", dbg_type);
        #endif
        return CMD_OK;
    }
    LOGW(tag, "Command obc_debug used with invalid param!");
    return CMD_FAIL;
}

int obc_reset_wdt(char *fmt, char *params, int nparams)
{
    int rc = CMD_OK;
    #ifdef NANOMIND
       wdt_clear();
    #endif
    #ifdef AVR32
       wdt_clear();
    #endif
    return rc;
}

int obc_reset(char *fmt, char *params, int nparams)
{
    printf("Resetting system NOW!!\n");

    #ifdef LINUX
        if(params != NULL && strcmp(params, "reboot")==0)
            system("sudo reboot");
        else
            exit(0);
    #endif
    #ifdef AVR32
        reset_do_soft_reset();
    #endif
    #ifdef NANOMIND
        cpu_reset();
    #endif

    /* Never get here */
    return CMD_OK;
}

int obc_get_os_memory(char *fmt, char *params, int nparams)
{

    #ifdef LINUX
        int c;
        FILE* file = fopen( "/proc/self/status", "r" );
        if (file)
        {
            while ((c = getc(file)) != EOF)
                putchar(c);
            fclose(file);
            return CMD_OK;
        }
        else
        {
            return CMD_FAIL;
        }
    #else
        #ifdef AVR32
            size_t mem_heap = 0; //xPortGetFreeHeapSize();
        #elif defined(NANOMIND)
            size_t mem_heap = 0;
        #else
            size_t mem_heap = xPortGetFreeHeapSize();
        #endif
        printf("Free RTOS memory: %d\n", (int)mem_heap);
        return CMD_OK;
    #endif
}

int obc_set_time(char* fmt, char* params,int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Parameter null");
        return CMD_FAIL;
    }
    int time_to_set;
    if(sscanf(params, fmt, &time_to_set) == nparams){
        int rc = dat_set_time(time_to_set);
        if (rc == 0)
            return CMD_OK;
        else
            return CMD_FAIL;
    }
    else
    {
        LOGW(tag, "set_time used with invalid params: %s", params);
        return CMD_FAIL;
    }
}

int obc_show_time(char* fmt, char* params,int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Parameter null");
        return CMD_FAIL;
    }
    int format;
    if(sscanf(params, fmt, &format) == nparams)
    {
        int rc = dat_show_time(format);
        if (rc == 0)
            return CMD_OK;
        else
            return  CMD_FAIL;
    }
    else
    {
        LOGW(tag, "show_time used with invalid params: %s", params);
        return CMD_FAIL;
    }
}

int obc_system(char* fmt, char* params, int nparams)
{
#ifdef LINUX
    if(params != NULL)
    {
        int rc = system(params);
        if(rc < 0)
        {
            LOGE(tag, "Call to system failed! (%d)", rc)
            return CMD_FAIL;
        }
        else
        {
            LOGV(tag, "Call to system returned (%d)", rc);
            return CMD_OK;
        }
    }
    else
    {
        LOGE(tag, "Parameter null");
        return CMD_FAIL;
    }
#else
    LOGW(tag, "Command not suported!");
    return CMD_FAIL;
#endif
}

int obc_set_pwm_duty(char* fmt, char* params, int nparams)
{
#ifdef NANOMIND
    int channel;
    int duty;
    if(sscanf(params, fmt, &channel, &duty) == nparams)
    {
        LOGI(tag, "Setting duty %d to Channel %d", duty, channel);
        gs_pwm_enable(channel);
        gs_pwm_set_duty(channel, duty);
        return CMD_OK;

    }
    else
    {
        LOGW(tag, "set_pwm_duty used with invalid params: %s", params);
        return CMD_FAIL;
    }
#else
    LOGW(tag, "Command not suported!");
    return CMD_FAIL;
#endif
}

/**
 * This commands are related to inter-stage panels and only available for the
 * Nanomind A3200 with inter-stage panels using the GSSB interface and drivers.
 */
#ifdef NANOMIND
#define GSSB_I2C_DEV 2

int gssb_pwr(char* fmt, char* params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "NULL params");
        return CMD_FAIL;
    }

    int vcc_on, vcc2_on;
    if(sscanf(params, fmt, &vcc_on, &vcc2_on) == nparams)
    {
        if (vcc_on > 0)
            pwr_switch_enable(PWR_GSSB);
        else
            pwr_switch_disable(PWR_GSSB);

        if (vcc2_on > 0)
            pwr_switch_enable(PWR_GSSB2);
        else
            pwr_switch_disable(PWR_GSSB2);

        LOGI(tag, "Set GSSB switch state to: %d %d", vcc_on, vcc2_on);
        return CMD_OK;
    }

    LOGE(tag, "Error parsing parameters");
    return CMD_FAIL;
}

int gssb_select_dev(char *fmt, char *params, int nparams)
{
    int address;
    if(sscanf(params, fmt, &address) == nparams)
    {
        // Set current i2c address of the driver
        if(address > 0)
        {
            gssb_select_device(GSSB_I2C_DEV, address);
        }

        LOGI(tag, "Current GSSB I2C device: %d, address: %#x (%d)",
                gssb_current_i2c_dev(),
                gssb_current_i2c_addr(),
                gssb_current_i2c_addr())
    }

    return CMD_FAIL;
}

int gssb_set_addr(char *fmt, char *params, int nparams)
{
    int new_address;
    int rc;
    if(sscanf(params, fmt, &new_address) == nparams)
    {
        rc = gssb_set_i2c_addr(new_address);
        if(rc)
        {
            rc = gssb_commit_i2c_addr();
            if(rc)
            {
                LOGD(tag, "Address set to 0x%X", new_address);
                return CMD_OK;
            }
        }
    }

    return CMD_FAIL;
}

int gssb_ident(char *fmt, char *params, int nparams)
{
    int rc;
    rc = gssb_get_version();
    int uuid = gssb_get_uuid();

    if (!rc || uuid < 0)
        return CMD_FAIL;

    //printf("Version: \t%s\r\n", (char *) resp.version);
    printf("Device UUID: %"PRIu32"\r\n", uuid);

    return CMD_OK;
}

int gssb_istage_get_settings(char* fmt, char* params, int nparams)
{
    gssb_set1_t set1;
    gssb_set2_get_t set2;
    gssb_set2_set_t set2_set;

    if(gssb_get_burn_settings1(&set1) == 0)
        return CMD_FAIL;

    if(gssb_get_burn_settings2(&set2) == 0)
        return CMD_FAIL;

    printf("\r\n-------- Deploy algorithm config ----------\r\n");
    printf("Knife on time:\t\t %i ms\r\n", set1.knife_on_time);
    printf("Increment:\t\t %i ms\r\n", set1.increment);
    printf("Deploy delay:\t\t %i\r\n", set1.deploy_delay);
    printf("Max repeats:\t\t %i\r\n", set1.max_repeats);
    printf("Repeat time:\t\t %i s\r\n", set1.repeat_delay);
    printf("Switch polarity:\t %"PRIu8"\r\n", set2.sw_polarity);
    printf("Deploy mode:\t %"PRIu8"\r\n", set2.mode);
    printf("Settings locked:\t %"PRIu8"\r\n", set2.locked);

    return CMD_OK;
}

int gssb_istage_settings(char *fmt, char *params, int nparams)
{
    LOGE(tag, "NOT IMPLEMENTED COMMAND!");

    // Unlock settings
    int rc = gssb_unlock_settings(1);
    if(!rc)
    {
        LOGE(tag, "Error unlocking settings!");
        return CMD_ERROR;
    }

    // Get config set 1 and 2
    gssb_set1_t set1; gssb_set2_get_t set2; gssb_set2_set_t set2_set;
    if(gssb_get_burn_settings1(&set1) == 0 || gssb_get_burn_settings2(&set2) == 0)
    {
        LOGE(tag, "Error retrieving settings");
        return CMD_FAIL;
    }

    int std_time, increment_ms, short_cnt_down, max_repeat, rep_time_s;
    int switch_polarity, reboot_deploy_cnt;

    // %d &d %d %d %d %d %d, nparams: 7
    if(sscanf(params, fmt, &std_time, &increment_ms, &short_cnt_down, &max_repeat,
            &rep_time_s, &switch_polarity, &reboot_deploy_cnt) == nparams)
    {
        /* Check settings range */
        if ((std_time > 8000) || (std_time < 0))
        {
            LOGE(tag, "Knife on time of %d out of range [0 - 8000]", nparams);
            return CMD_FAIL;
        }
        else
            set1.knife_on_time = (uint16_t) std_time;

        if ((increment_ms > 65535) || (increment_ms < 0))
        {
            LOGE(tag, "Increment of %d out of range [0 - 65535]", increment_ms);
            return CMD_FAIL;
        }
        else
            set1.increment = (uint16_t) increment_ms;

        if ((short_cnt_down > 65535) || (short_cnt_down < 0))
        {
            LOGE(tag, "Auto deploy delay of %d out of range [0 - 65535]", short_cnt_down);
            return CMD_FAIL;
        }
        else
            set1.deploy_delay = (uint16_t) short_cnt_down;

        if ((max_repeat > 255) || (max_repeat < 0)) {
            LOGE(tag, "Max repeats of %d out of range [0 - 255]", max_repeat);
            return CMD_FAIL;
        }
        else
            set1.max_repeats = (uint8_t) max_repeat;

        if ((rep_time_s > 255) || (rep_time_s < 0)) {
            LOGE(tag,  "Time between repeats of %d out of range [0 - 255]", rep_time_s);
            return CMD_FAIL;
        }
        else
            set1.repeat_delay = (uint8_t) rep_time_s;

        if ((switch_polarity > 1) || (switch_polarity < 0)) {
            LOGE(tag, "Polarity can only be selected to 0 or 1 not %d", switch_polarity);
            return CMD_FAIL;
        }
        else
            set2_set.sw_polarity = (uint8_t) switch_polarity;

        if ((reboot_deploy_cnt > 255) || (reboot_deploy_cnt < 0)) {
            LOGE(tag, "Reboot deploy number of %d out of range [0 - 255]", reboot_deploy_cnt);
            return CMD_FAIL;
        }
        else
            set2_set.reboot_cnt = (uint8_t) reboot_deploy_cnt;

        // Show the new settings
        printf("\r\n-------- New settings ----------\r\n");
        printf("Knife on time:\t\t %i ms\r\n", set1.knife_on_time);
        printf("Increment:\t\t %i ms\r\n", set1.increment);
        printf("Deploy delay:\t\t %i\r\n", set1.deploy_delay);
        printf("Max repeats:\t\t %i\r\n", set1.max_repeats);
        printf("Repeat time:\t\t %i s\r\n", set1.repeat_delay);
        printf("Switch polarity:\t %"PRIu8"\r\n", set2_set.sw_polarity);
        printf("Reboot counter:\t %"PRIu8"\r\n", set2_set.reboot_cnt);
        printf("Deploy mode:\t %"PRIu8"\r\n", set2.mode);
        printf("Settings locked:\t %"PRIu8"\r\n", set2.locked);
        printf("-------- ---------------- ----------\r\n");

        // Finally send new settings
        int rc1, rc2 = 0;
        rc1 = gssb_set_burn_settings1(&set1);
        rc2 = gssb_set_burn_settings2(&set2_set);
        if(rc1 == 0 || rc2 == 0)
        {
            LOGE(tag, "Error sending settings (%d, %d)", rc1, rc2);
            return CMD_FAIL;
        }

        gssb_reboot();
        return CMD_OK;

    }

    LOGE(tag, "Error parsing parameters");
    return CMD_FAIL;
}

int gssb_istage_reboot(char *fmt, char *params, int nparams)
{
    if (gssb_reboot())
        return CMD_OK;

    return CMD_FAIL;
}

int gssb_istage_deploy(char *fmt, char *params, int nparams)
{
    if (gssb_manual_deploy())
        return CMD_OK;

    return CMD_FAIL;
}

int gssb_unlock(char *fmt, char *params, int nparams)
{
    int rc = gssb_unlock_settings(1);
    return rc ? CMD_OK : CMD_FAIL;
}

int gssb_istage_arm(char *fmt, char *params, int nparams)
{
    int state, rc;
    gssb_set3_t mode;

    if(sscanf(params, fmt, &state) == nparams)
    {
        // First unlock settings
        rc = gssb_unlock_settings(1);
        if(!rc)
        {
            LOGE(tag, "Error unlocking settings!");
            return CMD_ERROR;
        }

        // Second, set the current mode or state
        if(state == GSSB_ARMED_NONE)
        {
            LOGD(tag, "Setting armed NONE");
            mode = armed_not;
            rc = gssb_set_burn_settings3(mode);
            gssb_reboot();
            if(rc)
                return CMD_OK;
        }
        else if (state == GSSB_ARMED_AUTO)
        {
            LOGD(tag, "Setting armed AUTO");
            mode = armed_auto;
            rc = gssb_set_burn_settings3(mode);
            gssb_reboot();
            if(rc)
                return CMD_OK;
        }
        else if(state == GSSB_ARMED_MANUAL)
        {
            //TODO: First set to GSSB_ARMED_NONE
            LOGD(tag, "Setting armed MANUAL");
            rc = gssb_changestate(1); // Set armed to manual
            if(rc)
                return CMD_OK;
        }
        else
        {
            LOGE(tag, "Error setting ARM state");
            return CMD_FAIL;
        }
    }

    LOGE(tag, "Error parsing arguments");
    return CMD_FAIL;
}

int gssb_istage_sensors(char *fmt, char *params, int nparams)
{
    int16_t temp_sensor = gssb_get_temperature();
    int16_t sun_sensor = gssb_get_panel_sunsens();

    if(temp_sensor == -1 || sun_sensor == -1)
    {
        LOGE(tag, "Error reading sensors!");
        return CMD_ERROR;
    }

    // TODO: Store this values
    printf("Panel Temperature: %d\r\n", (int)temp_sensor);
    printf("Coarse Sunsensor: %d\r\n", (int)sun_sensor);

    return CMD_OK;
}

int gssb_istage_status(char *fmt, char *params, int nparams)
{
    gssb_cnt_t cnt;
    int rc = gssb_get_counters(&cnt);
    int rel_status = gssb_get_release_status();

    if(rel_status == -1 || rc == 0)
    {
        LOGE(tag, "Error reading status (rc: %d, rel: %d)", rc, rel_status);
        return CMD_ERROR;
    }

    printf("Antenna release status: %d\r\n", rel_status);
    printf("Deploy state: %"PRIu8"\r\n", cnt.release_state);
    printf("Delay till deploy:\t\t\t %"PRIu16" ms\r\n", cnt.delay_deploy);
    printf("Number of attemps since boot:\t\t %"PRIu8"\r\n", cnt.burns_since_boot);
    printf("Knife that will be use in next deploy:\t %"PRIu8"\r\n", cnt.knife_next_brn);
    printf("Total deploy attemps:\t\t\t %"PRIu16"\r\n", cnt.burns_total);
    printf("Reboot deploy cnt:\t\t\t %"PRIu8"\r\n", cnt.reboot_dep_cnt);

    return CMD_OK;
}

#endif //NANOMIND