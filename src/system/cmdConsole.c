/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2020, Tomas Opazo Toro, toopazo@ing.uchile.cl
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
    cmd_add("test", con_debug_msg, "%s", 1);
    cmd_add("help", con_help, "", 0);
    cmd_add("log_set", con_set_logger, "%d %d", 2);
}

/**
 * Display a test message
 * @param param void message as char array
 * @return 1 - OK, 0 fail
 */
int con_debug_msg(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Parameter null");
        return CMD_SYNTAX_ERROR;
    }

    printf("%s", params);
    return CMD_OK;
}

int con_help(char *fmt, char *params, int nparams)
{
    printf("List of commands:\n");
    cmd_print_all();
    return CMD_OK;
}

int con_set_logger(char *fmt, char *params, int nparams)
{
    int lvl;
    int node;

    if(params == NULL || (sscanf(params, fmt, &lvl, &node) != nparams))
        return CMD_SYNTAX_ERROR;

    if(lvl > LOG_LVL_VERBOSE)
        return CMD_ERROR;

    log_set((log_level_t)lvl, node);
    return CMD_OK;
}