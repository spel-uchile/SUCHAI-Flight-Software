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
    cmd_add("test_mult_exe", test_mult_exe,"",0);
    cmd_add("test_print_int", test_print_int, "", 0);
    cmd_add("test_print_char", test_print_char, "", 0);
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

int test_mult_exe(char* fmt, char* params, int nparams)
{
    char* cmd1 = "test_print_int";
    char* cmd2 = "test_print_char";
    cmd_t *test_int = cmd_get_str(cmd1);
    cmd_add_params_str(test_int, "");
    cmd_t *test_char = cmd_get_str(cmd2);
    cmd_add_params_str(test_char, "");
    cmd_send(test_int);
    cmd_send(test_char);
    return CMD_OK;
}

int test_print_int(char* fmt, char* params, int nparams)
{
    int a = 0;
    while(a<10){
        printf("%d\n",a);
        a++;
    }
    return CMD_OK;
}

int test_print_char(char* fmt, char* params, int nparams)
{
    char a = 'a';
    while(a<'j'){
        printf("%cc\n",a);
        a++;
    }
    return CMD_OK;
}
