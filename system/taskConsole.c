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

//extern xQueueHandle dispatcher_queue;

#define CON_BUFF_LEN 100

const char console_baner[] =   
"______________________________________________________________________________\n\
                     ___ _   _  ___ _  _   _   ___ \n\
                    / __| | | |/ __| || | /_\\ |_ _|\n\
                    \\__ \\ |_| | (__| __ |/ _ \\ | | \n\
                    |___/\\___/ \\___|_||_/_/ \\_\\___|\n\
______________________________________________________________________________\n\n";


//"\n\n====== WELCOME TO THE SUCHAI CONSOLE - PRESS ANY KEY TO START ======\n\r"

void taskConsole(void *param)
{
#if (SCH_GRL_VERBOSE)
    printf(">>[Console] Started\r\n");
#endif

    portTick Delayms = osDefineTime(250);
    cmd_t *new_cmd =  NULL;
    char buffer[CON_BUFF_LEN];

    /* Initializing console */
    console_init();

#if (SCH_GRL_VERBOSE>=1)
    printf(console_baner);
#endif

    while(1)
    {
        osDelay(Delayms);

        /* Read console and parse commands (blocking) */
        console_read(buffer, CON_BUFF_LEN);
        new_cmd = console_parse(buffer);

        if(new_cmd != NULL)
        {
            #if (SCH_GRL_VERBOSE >=1)
                /* Print the parsed command */
                printf("[Console] Se genera comando: %d\n", new_cmd->id);
            #endif

            /* Queue NewCmd - Blocking */
            osQueueSend(dispatcher_queue, &new_cmd, portMAX_DELAY);
        }
    }
}

int console_init(void)
{
    printf("[Console] Init...\n");
    cmd_print_all();
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
    char tmp_cmd[CON_BUFF_LEN];
    char tmp_arg[CON_BUFF_LEN];
    int n_args;

    printf("[Console] Parsing: %s\n", buffer);
    n_args = sscanf(buffer, "%s %s", tmp_cmd, tmp_arg);
    printf("[Console] Parsed: %s, %s\n", tmp_cmd, tmp_arg);

    if (n_args != 2) return 0;

    cmd_t *new_cmd = cmd_get_str(tmp_cmd);

    if(new_cmd == NULL) return 0;

    /* TODO: Parse args according to command */
    new_cmd->params = (char *)malloc(sizeof(char)*(strlen(tmp_arg)+1));
    strcpy(new_cmd->params, tmp_arg);

    return new_cmd;
}
