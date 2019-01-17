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
    cmd_add("istage_set_node", gssb_set_gssb_node, "%d", 1);
    cmd_add("istage_set_addr", gssb_set_addr_csp, "%d", 1);
    cmd_add("istage_ident", gssb_gssb_ident_csp, "", 0);
    cmd_add("istage_get_config", gssb_istage_get_settings, "", 0);
    cmd_add("istage_set_config", gssb_istage_settings_csp, "%d %d %d %d %d %d %d", 7);
    cmd_add("istage_reset", gssb_istage_reboot_csp, "", 0);
    cmd_add("istage_deploy", gssb_istage_deploy_csp, "", 0);
    cmd_add("istage_set_arm", gssb_istage_arm_csp, "%d", 1);
    cmd_add("istage_set_state", gssb_istage_state_csp, "%d", 1);
    cmd_add("istage_get_sensors", gssb_istage_sensors_csp, "", 0);
    cmd_add("istage_get_status", gssb_istage_status_csp, "", 0);
    cmd_add("istage_get_sunsensor", gssb_sunsensor_read_csp, "", 0);
    cmd_add("istage_get_temp", gssb_sunsensor_temp_csp, "", 0);
    cmd_add("istage_get_temp1", gssb_sunsensor_temp1_csp, "", 0);
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

/* This commands are related to inter-stage panels and only available for the
 * Nanomind A3200 with inter-stage panels using the GSSB interface and drivers.
 */
#ifdef NANOMIND
static uint8_t gssb_csp_node = 1;
static uint16_t gssb_csp_timeout = 1000;
static uint8_t i2c_addr = 5;
static uint16_t i2c_timeout_ms = 50;

int gssb_set_gssb_node(char* fmt, char* params, int nparams)
{
    int node;
    if(sscanf(params, fmt, &node) == nparams)
    {
        gssb_node(node, 1000);
        return CMD_OK;
    }

    return CMD_FAIL;
}

int gssb_set_addr_csp(char* fmt, char* params, int nparams)
{
    int dev;
    if(sscanf(params, fmt, &dev) == nparams)
    {
        gssb_dev(dev, 50);
        return CMD_OK;
    }

    return CMD_FAIL;
}

int gssb_gssb_ident_csp(char* fmt, char* params, int nparams)
{
    gssb_csp_ident_t resp;

    if (gssb_ident(&resp) < 0)
        return CMD_FAIL;

    if (resp.i2c_res != E_NO_ERR) {
        printf("GSSB device did not respond\r\n");
        return CMD_FAIL;
    }

    printf("Version: \t%s\r\n", (char *) resp.version);
    printf("Device UUID: %"PRIu32"\r\n", resp.uuid);

    return CMD_OK;
}

int gssb_istage_get_settings(char* fmt, char* params, int nparams)
{
    istage_burn_settings_t settings;

    if (gssb_interstage_settings_get(&settings) < 0)
        return CMD_FAIL;

    if (settings.status == 2)
    {
        printf("\r\nDeploy mode:\t\t ARMED AUTO\r\n");
    }
    else
    {
        printf("\r\nDeploy mode:\t\t NOT ARMED\r\n");
    }

    printf("Deploy delay from boot:\t %"PRIu16" s\r\n",
           settings.short_cnt_down);

    printf("\r\n-------- Deploy algorithm config ----------\r\n");
    printf("Knife on time:\t\t %i ms\r\n", settings.std_time_ms);
    printf("Increment:\t\t %i ms\r\n", settings.increment_ms);
    printf("Max repeats:\t\t %i\r\n", settings.max_repeat);
    printf("Repeat time:\t\t %i s\r\n", settings.rep_time_s);
    printf("Switch polarity:\t %"PRIu8"\r\n", settings.switch_polarity);
    printf("Settings locked:\t %"PRIu8"\r\n", settings.locked);
}

int gssb_istage_settings_csp(char* fmt, char* params, int nparams)
{

    // Get config first
    istage_burn_settings_t settings;
    if (gssb_interstage_settings_get(&settings) < 0)
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
        if ((std_time > 65535) || (std_time < 0))
        {
            LOGE(tag, "Knife on time of %d out of range [0 - 65535]", nparams);
            return CMD_FAIL;
        }
        else
            settings.std_time_ms = (uint16_t) std_time;

        if ((increment_ms > 65535) || (increment_ms < 0))
        {
            LOGE(tag, "Increment of %d out of range [0 - 65535]", increment_ms);
            return CMD_FAIL;
        }
        else
            settings.increment_ms = (uint16_t) increment_ms;

        if ((short_cnt_down > 65535) || (short_cnt_down < 0))
        {
            LOGE(tag, "Auto deploy delay of %d out of range [0 - 65535]", short_cnt_down);
            return CMD_FAIL;
        }
        else
            settings.short_cnt_down = (uint16_t) short_cnt_down;

        if ((max_repeat > 255) || (max_repeat < 0)) {
            LOGE(tag, "Max repeats of %d out of range [0 - 255]", max_repeat);
            return CMD_FAIL;
        }
        else
            settings.max_repeat = (uint8_t) max_repeat;

        if ((rep_time_s > 255) || (rep_time_s < 0)) {
            LOGE(tag,  "Time between repeats of %d out of range [0 - 255]", rep_time_s);
            return CMD_FAIL;
        }
        else
            settings.rep_time_s = (uint8_t) rep_time_s;

        if ((switch_polarity > 1) || (switch_polarity < 0)) {
            LOGE(tag, "Polarity can only be selected to 0 or 1 not %d", switch_polarity);
            return CMD_FAIL;
        }
        else
            settings.switch_polarity = (uint8_t) switch_polarity;

        if ((reboot_deploy_cnt > 255) || (reboot_deploy_cnt < 0)) {
            LOGE(tag, "Reboot deploy number of %d out of range [0 - 255]", reboot_deploy_cnt);
            return CMD_FAIL;
        }
        else
            settings.reboot_deploy_cnt = (uint8_t) reboot_deploy_cnt;

        // Finally send the new settings
        if (gssb_interstage_settings_set(&settings) < 0)
        {
            LOGE(tag, "Error sending settings")
            return CMD_FAIL;
        }
        else
            return CMD_OK;
    }

    LOGE(tag, "Error parsing parameters");
    return CMD_FAIL;
}

int gssb_istage_reboot_csp(char* fmt, char* params, int nparams)
{
    if (gssb_reboot() <= 0)
        return CMD_FAIL;

    return CMD_OK;
}

int gssb_istage_deploy_csp(char* fmt, char* params, int nparams)
{

    if (gssb_istage_deploy() <= 0)
        return CMD_FAIL;

    return CMD_OK;
}

int gssb_istage_arm_csp(char* fmt, char* params, int nparams)
{
    int state;
    if(sscanf(params, fmt, &state) == nparams)
    {
        if (gssb_istage_arm(state) <= 0)
        {
            LOGE(tag, "Error sending command");
            return CMD_FAIL;
        }
        else
            return CMD_OK;
    }

    LOGE(tag, "Error parsing arguments");
    return CMD_OK;
}

int gssb_istage_state_csp(char* fmt, char* params, int nparams)
{

    int state;
    if(sscanf(params, fmt, &state) == nparams)
    {
        if (gssb_istage_set_state(state) <= 0)
        {
            LOGE(tag, "Error setting state");
            return CMD_FAIL;
        }
        else
            return CMD_OK;
    }

    LOGE(tag, "Error parsing parameters");
    return CMD_FAIL;
}

int gssb_istage_sensors_csp(char* fmt, char* params, int nparams)
{
    gssb_istage_sensors_t sensors;

    // Get sensor values
    if (gssb_istage_sensors(&sensors) <= 0)
    {
        LOGE(tag, "Error reading sensors");
        return CMD_FAIL;
    }

    if (sensors.i2c_res != E_NO_ERR)
    {
        LOGE(tag, "GSSB device did not respond");
        return CMD_FAIL;
    }

    // TODO: Store this values
    printf("Panel Temperature: %d\r\n", (int) sensors.panel_temp/10);
    printf("Coarse Sunsensor: %d\r\n", sensors.sun_voltage);

    return CMD_OK;
}

int gssb_istage_status_csp(char* fmt, char* params, int nparams)
{
    gssb_istage_status_t status;
    char *state_str;

    if (gssb_interstage_status(&status) <= 0)
    {
        LOGE(tag, "Error reading status");
        return CMD_FAIL;
    }

    if (status.i2c_res != E_NO_ERR)
    {
        LOGE(tag, "GSSB device did not respond");
        return CMD_FAIL;
    }

    switch (status.state)
    {
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

    //TODO: Do something with this values
    printf("Current state:\t\t\t\t %s\r\n", state_str);
    if (status.release_status == 1)
        printf("Antenna state:\t\t\t\t %s\r\n", "Released");
    else
        printf("Antenna state:\t\t\t\t %s\r\n", "Not released");

    printf("Delay till deploy:\t\t\t %"PRIu16" s\r\n", status.deploy_in_s);
    printf("Number of attemps since boot:\t\t %"PRIu8"\r\n", status.number_of_deploys);
    printf("Knife that will be use in next deploy:\t %"PRIu8"\r\n", status.active_knife);
    printf("Total deploy attemps:\t\t\t %"PRIu16"\r\n", status.total_number_of_deploys);
    printf("Reboot deploy cnt:\t\t\t %"PRIu8"\r\n", status.reboot_deploy_cnt);

    return CMD_OK;
}

int gssb_sunsensor_read_csp(char* fmt, char* params, int nparams)
{
    gssb_csp_cmd_resp_t resp;
    uint16_t sun[4];
    int i;


    if (sunsensor_sample_get_data_csp(gssb_csp_node, gssb_csp_timeout, i2c_addr, i2c_timeout_ms, &resp) <= 0)
    {
        LOGE(tag, "Error reading sensors");
        return CMD_FAIL;
    }

    if (resp.return_code != E_NO_ERR)
    {
        LOGE(tag, "Error in I2C bus");
        return CMD_FAIL;
    }

    //Merge bytes to int
    for (i=0; i <= 7; i += 2)
        sun[i/2] = (resp.buff[i] << 8) + resp.buff[i+1];

    //TODO: Do something with the values
    printf("\r\n ---- Values ----\r\n");
    for (i=0; i < 4; i++)
        printf("Sun sensor %d: %d\r\n", i, sun[i]);

    return CMD_OK;
}

int gssb_sunsensor_temp_csp(char* fmt, char* params, int nparams)
{
    gssb_csp_cmd_resp_t resp;
    float temp;

    if (sunsensor_sample_get_temp_csp(gssb_csp_node, gssb_csp_timeout, i2c_addr, i2c_timeout_ms, &resp) <= 0)
    {
        LOGE(tag, "Error reading sensors");
        return CMD_FAIL;
    }

    if (resp.return_code != E_NO_ERR)
    {
        LOGE(tag, "Error in I2C bus");
        return CMD_FAIL;
    }

    /* if neg temp else */
    if (resp.buff[0] & 0x80)
        temp = ((~((resp.buff[0] << 8) + resp.buff[1] - 1)) >> 2 ) * -0.03125;
    else
        temp = (((resp.buff[0] << 8) + resp.buff[1]) >> 2 ) * 0.03125;

    //TODO: Do something with this values
    printf("Temp: %f\n", temp);

    return CMD_OK;
}


int gssb_sunsensor_temp1_csp(char* fmt, char* params, int nparams)
{
    gssb_csp_cmd_resp_t resp;
    float temp;

    if (sunsensor_sample_get_temp_csp(gssb_csp_node, gssb_csp_timeout, i2c_addr, i2c_timeout_ms, &resp) <= 0)
    {
        LOGE(tag, "Error reading sensors");
        return CMD_FAIL;
    }

    if (resp.return_code != E_NO_ERR)
    {
        LOGE(tag, "Error in I2C bus");
        return CMD_FAIL;
    }

    /* if neg temp else */
    if (resp.buff[0] & 0x80)
        temp = ((~((resp.buff[0] << 8) + resp.buff[1] - 1)) >> 2 ) * -0.03125;
    else
        temp = (((resp.buff[0] << 8) + resp.buff[1]) >> 2 ) * 0.03125;

    //TODO: Do something with this values
    printf("Temp: %f\n", temp);

    return CMD_OK;
}
#endif //NANOMIND