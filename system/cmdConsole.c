/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2017, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2017, Tomas Opazo Toro, toopazo@ing.uchile.cl
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

static const char *tag = "cmdConsole";

void cmd_console_init(void)
{
    //void cmd_add(char* name, cmdFunction function, char* fparams)
    cmd_add("test", con_debug_msg, "%s", 1);
    cmd_add("help", con_help, "", 0);
    cmd_add("ping", con_ping, "%d", 1);
}

/**
 * Display a test message
 * @param param void message as char array
 * @return 1 - OK, 0 fail
 */
int con_debug_msg(char *fmt, char *params, int nparams)
{
    char msg[CMD_MAX_STR_PARAMS];
    if(sscanf(params,fmt, msg) == nparams)
    {
        printf("[Debug Msg] %s\n", msg);
        return CMD_OK;
    }
    return CMD_FAIL;
}

int con_help(char *fmt, char *params, int nparams)
{
//    osSemaphoreTake(&log_mutex, portMAX_DELAY);
    printf("List of commands:\n");
    cmd_print_all();
//    osSemaphoreGiven(&log_mutex);
    return CMD_OK;
}

int con_ping(char *fmt, char *params, int nparams)
{
    int node;
    if(sscanf(params, fmt, &node) == nparams)
    {
        int rc = csp_ping((uint8_t)node, 3000, 10, CSP_O_NONE);
        LOGI(tag, "Ping to %d took %d", node, rc);
        if(rc > 0)
            return CMD_OK;
        else
            return CMD_FAIL;
    }
    return CMD_FAIL;
}
