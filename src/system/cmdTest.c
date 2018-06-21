/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
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
#include "cmdTest.h"

static const char* tag = "cmdTest";

void cmd_test_init(void)
{
    cmd_add("test_fp_params", test_fp_params, "%d %s %d", 3);
    cmd_add("test_print_int", test_print_int, "", 0);
    cmd_add("test_print_char", test_print_char, "", 0);
    cmd_add("test_print_char2", test_print_char2, "", 0);
}

int test_fp_params(char* fmt, char* params,int nparams)
{
    int num1, num2;
    char str[CMD_MAX_STR_PARAMS];
    if(sscanf(params, fmt, &num1, &str, &num2) == nparams)
    {
        printf("The parameters are: %d ; %s ; %d \n",num1, str ,num2);
        return CMD_OK;
    }
    else
    {
        LOGW(tag, "test_fp used with invalid params: %s", params);
        return CMD_FAIL;
    }
}

int test_print_int(char* fmt, char* params, int nparams)
{
    int a = 0;
    portTick delay_ms = 1000;
    portTick xLastWakeTime = osTaskGetTickCount();
    while(a<10){
        printf("%d\n",a);
        osTaskDelayUntil(&xLastWakeTime, delay_ms);
        a++;
    }
    return CMD_OK;
}

int test_print_char(char* fmt, char* params, int nparams)
{
    char a = 'a';
    portTick delay_ms = 500;
    portTick xLastWakeTime = osTaskGetTickCount();
    while(a<'j'){
        printf("%c\n",a);
        osTaskDelayUntil(&xLastWakeTime, delay_ms);
        a++;
    }
    return CMD_OK;
}

int test_print_char2(char* fmt, char* params, int nparams)
{
    char a = 'A';
    portTick delay_ms = 500;
    portTick xLastWakeTime = osTaskGetTickCount();
    while(a<'J'){
        printf("%c\n",a);
        osTaskDelayUntil(&xLastWakeTime, delay_ms);
        a++;
    }
    return CMD_OK;
}