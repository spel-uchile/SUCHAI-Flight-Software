/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020 Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2020, Tomas Opazo Toro, tomas.opazo.t@gmail.com
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

#include "suchai/taskConsole.h"

static const char *tag = "Console";
static const char *cmd_hist = "/tmp/suchai_fs_history.txt";
static char console_prompt[15] = "SUCHAI> ";
const char console_banner[] =
"\n______________________________________________________________________________\n\
                     ___ _   _  ___ _  _   _   ___ \n\
                    / __| | | |/ __| || | /_\\ |_ _|\n\
                    \\__ \\ |_| | (__| __ |/ _ \\ | | \n\
                    |___/\\___/ \\___|_||_/_/ \\_\\___|\n\
______________________________________________________________________________\n\n";


//"\n\n====== WELCOME TO THE SUCHAI CONSOLE - PRESS ANY KEY TO START ======\n\r";

void taskConsole(void *param)
{
    LOGI(tag, "Started");

    portTick delay_ms = 250;
    cmd_t *new_cmd =  NULL;
    char buffer[SCH_BUFF_MAX_LEN];

    /* Initializing console */
    console_init();

    LOGI(tag, console_banner);

    while(1)
    {
        osDelay(delay_ms);

        /* Read console and parse commands (blocking, except in ESP32) */
        memset(buffer, '\0', SCH_BUFF_MAX_LEN);
        if(console_read(buffer, SCH_BUFF_MAX_LEN-1) != 0)
            continue;

        new_cmd = cmd_build_from_str(buffer);

        if(new_cmd != NULL)
        {
            if(log_lvl >= LOG_LVL_DEBUG)
            {
                char *name = cmd_get_name(new_cmd->id);
                LOGD(tag, "Command sent: %d (%s)", new_cmd->id, name);
                free(name);
            }
            /* Queue NewCmd - Blocking */
            cmd_send(new_cmd);
        }
        else
        {
            LOGW(tag, "An invalid command was read!");
        }
    }
}

int console_init(void)
{
    LOGD(tag, "Init...\n");

#ifdef LINUX
    linenoiseHistoryLoad(cmd_hist); /* Load the history at startup */
#endif

    cmd_print_all();
    return 0;
}

int console_read(char *buffer, int len)
{
#ifdef LINUX
    char *line;
    line = linenoise(console_prompt);
    if(line == NULL)
        return -1;

    linenoiseHistoryAdd(line);
    linenoiseHistorySave(cmd_hist);
    strncpy(buffer, line, len-1);
    free(line);
#else
    char *s = fgets(buffer, len, stdin);
    // Clean CR and LF character to be compatible with linenoise behaviour
    if(s == NULL)
        return -1;

    int i;
    for(i=0; i<len; i++)
    {
        if(s[i] == '\n' || s[i] == '\r')
            s[i] = 0;
    }
#endif

    /* Parse '/exit' to do a clean exit */
    if(!strncmp(buffer, "/exit", 5))
    {
        memset(buffer, 0, len);
        strncpy(buffer, "obc_exit", len-1);
        return 0;
    }

    /* Parse remote commands mode using "<node>: command" */
    int next = 0;
    int node;
    if(sscanf(buffer, "%d: %n", &node, &next) == 1)
    {
        if(next < len && next != 0)
        {
            char *tmp_buff = strndup(buffer + next, len - next);
            memset(buffer, 0, len);
            snprintf(buffer, len, "com_send_cmd %d %s", node, tmp_buff);
            free(tmp_buff);
        }
    }

    return 0;
}

void console_set_prompt(char *prompt)
{
    if(prompt != NULL) {
        strncpy(console_prompt, prompt, 12);
        strcat(console_prompt, "> ");
    }
}
