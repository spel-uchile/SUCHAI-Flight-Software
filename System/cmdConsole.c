/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2013, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
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

#include "cmdConsole.h"

void cmd_console_init(void)
{
    //void cmd_add(char* name, cmdFunction function, char* fparams)
    cmd_add("test", con_debug_msg, "%s", 1);
    //cmd_add("help", con_help, 0);

}

/**
 * Display a generic quantity argument error
 * @param param void
 * @return 1 - OK
 */
int con_error_count_arg(char *fmt, char *params, int nparams)
{
    printf("[Console Error] Arguments quantity dismatch\r\n"); //Type con_help for available commands\r\n");
    return CMD_OK;
}

/**
 * Display a generic invalid argument error
 * @param param void
 * @return 1 - OK
 */
int con_error_invalid_arg(char *fmt, char *params, int nparams)
{
    printf("[Console Error] Arguments invalid\r\n"); //Type con_help for available commands\r\n");
    return CMD_OK;
}

/**
 * Display a generic unknown command error
 * @param param void
 * @return 1 - OK
 */
int  con_error_unknown_cmd(char *fmt, char *params, int nparams)
{
    printf("[Console Error] Unknown command\r\n"); //Type con_help for available commands\r\n");
    return CMD_OK;
}

/**
 * Display a generic command too long error
 * @param param void
 * @return 1 - OK
 */
int  con_error_cmd_toolong(char *fmt, char *params, int nparams)
{
    printf("[Console Error] Command too long\r\n"); //Type con_help for available commands\r\n");
    return CMD_OK;
}

/**
 * Display a test message
 * @param param void message as char array
 * @return 1 - OK, 0 fail
 */
int con_debug_msg(char *fmt, char *params, int nparams)
{
    //char *msg = (char *)param;
    char *msg;

    if(sscanf(params,fmt, msg) == nparams)
    {
        printf("[Debug Msg] %s\n", msg);
        return CMD_OK;
    }
    return CMD_FAIL;
}

/**
 * Display help text.
 * Add custom help text inside this funcion
 * 
 * @param param void
 * @return 1 - OK
 */
int con_help(char *fmt, char *params, int nparams)
{
    printf("List of commands:\n");
    cmd_print_all();
    return CMD_OK;
}

/**
 * TEST AND EXAMPLE
 *
 * @param param void
 * @return 1 - OK
 */
int con_print_4_int(char *fmt, char *params, int nparams)
{
//    VALIDATE_PARAMS(param);

    /*int par1 = ((int *)param)[0];
    int par2 = ((int *)param)[1];
    int par3 = ((int *)param)[2];

    printf("Mis parametros son: %d, %d, %d", par1, par2, par3);*/
    return CMD_OK;
}

/**
 * TEST AND EXAMPLE
 *
 * @param param void
 * @return 1 - OK
 *
 * char *params;
 * sprintf(params, cmd->fmt, value, string);
 * con_print_str_float("%s %f", "hola 23.1");
 * con_print_ints("%d %d %d", "1 2 3");
 * con_print_sp("%d %f %s", "1 2.0 hola");
 */
int con_print_str_float(char *fmt, char *params, int nparams)
{
    float num;
    char *str;

    if(!sscanf(params, fmt, &num, &str) == 2)
        return CMD_FAIL;

    printf("Mis parametros son: %f, %s", num, str);
    return CMD_OK;
}

/**
 * Display a generic propmp
 * @param param void
 * @return 1 - OK
 */
int con_promt(char *fmt, char *params, int nparams)
{
    printf(">>");
    return CMD_OK;
}

