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
