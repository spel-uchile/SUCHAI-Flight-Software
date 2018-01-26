/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2017, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
 *      Copyright 2017, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *      Copyright 2017, Matias Ramirez Martinez, nicoram.mt@gmail.com
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
    cmd_add("reset", obc_reset, "", 0);
    cmd_add("get_mem", obc_get_os_memory, "", 0);
    cmd_add("set_time", obc_set_time,"%d",1);
    cmd_add("show_time", obc_show_time,"%d",1);
    cmd_add("test_fp", test_fp, "%d %s %d", 3);
}

int obc_reset(char *fmt, char *params, int nparams)
{
    printf("Resetting system NOW!!\n");

    #ifdef LINUX
        raise(SIGINT);
    #else
        //TODO __asm__ volatile("reset");
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
        #else
            size_t mem_heap = xPortGetFreeHeapSize();
        #endif
        printf("Free RTOS memory: %d\n", (int)mem_heap);
        return CMD_OK;
    #endif
}

int obc_set_time(char* fmt, char* params,int nparams)
{
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

int test_fp(char* fmt, char* params,int nparams)
{
    int num1, num2;
    char str[CMD_MAX_STR_PARAMS];
    if(sscanf(params, fmt, &num1, &str, &num2) == nparams)
    {
        printf("Los parametros leidos son: %d ; %s ; %d \n",num1, str ,num2);
        return CMD_OK;
    }
    else
    {
        LOGW(tag, "test_fp used with invalid params: %s", params);
        return CMD_FAIL;
    }
}