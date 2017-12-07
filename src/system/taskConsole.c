/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2013, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
 *      Copyright 2013, Tomas Opazo Toro, tomas.opazo.t@gmail.com
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

#include "taskConsole.h"

#define CON_BUFF_LEN 100

static const char *tag = "Console";

const char console_banner[] =
"\n______________________________________________________________________________\n\
                     ___ _   _  ___ _  _   _   ___ \n\
                    / __| | | |/ __| || | /_\\ |_ _|\n\
                    \\__ \\ |_| | (__| __ |/ _ \\ | | \n\
                    |___/\\___/ \\___|_||_/_/ \\_\\___|\n\
______________________________________________________________________________\n\n";


//"\n\n====== WELCOME TO THE SUCHAI CONSOLE - PRESS ANY KEY TO START ======\n\r"

void taskConsole(void *param)
{
    LOGD(tag, "Started");

    portTick delay_ms = 250;
    cmd_t *new_cmd =  NULL;
    char buffer[CON_BUFF_LEN];

    /* Initializing console */
    console_init();

    LOGI(tag, console_banner);

    while(1)
    {
        osDelay(delay_ms);

        /* Read console and parse commands (blocking) */
        console_read(buffer, CON_BUFF_LEN);
        new_cmd = console_parse(buffer);

        if(new_cmd != NULL)
        {
#if LOG_LEVEL >= LOG_LVL_DEBUG
            char *name = cmd_get_name(new_cmd->id);
            LOGD(tag, "Command sent: %d (%s)", new_cmd->id, name);
            free(name);
#endif
            /* Queue NewCmd - Blocking */
            cmd_send(new_cmd);
        }
        else
        {
            LOGW(tag, "Null command was read!");
        }
    }
}

int console_init(void)
{
    LOGD(tag, "Init...\n");
//    osSemaphoreTake(&log_mutex, portMAX_DELAY);
    cmd_print_all();
//    osSemaphoreGiven(&log_mutex);
    return 0;
}

int console_read(char *buffer, int len)
{
    /* FIXME: Move to drivers to support different systems */
    fgets(buffer, len-1, stdin);
    return 0;
}

cmd_t * console_parse(char *buffer)
{
    cmd_t *new_cmd = NULL;
    char tmp_cmd[CON_BUFF_LEN];
    char tmp_arg[CON_BUFF_LEN];
    int next;
    int n_args;

    LOGV(tag, "Parsing: %s", buffer);
    n_args = sscanf(buffer, "%s %n", tmp_cmd, &next);
    strncpy(tmp_arg, buffer+next, CON_BUFF_LEN);
    LOGV(tag, "Parsed %d: %s, %s (%d))", n_args, tmp_cmd, tmp_arg, next);

    if (n_args == 1)
    {
        new_cmd = cmd_get_str(tmp_cmd);
        if(new_cmd)
        {
            if(next > 1)
                cmd_add_params_str(new_cmd, tmp_arg);
            else
                cmd_add_params_str(new_cmd, "");
        }
    }

    return new_cmd;
}
