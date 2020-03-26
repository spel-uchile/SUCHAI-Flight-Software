/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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

#include "repoCommand.h"

const static char *tag = "repoCmd";

/* Global variables */
cmd_list_t cmd_list[SCH_CMD_MAX_ENTRIES];
int cmd_index = 0;
char cmd_is_sorted = 1;

int cmd_add(char *name, cmdFunction function, char *fparams, int nparam)
{
    if (cmd_index < SCH_CMD_MAX_ENTRIES)
    {
        // Create new command
        size_t l_name = strlen(name);
        size_t l_fparams = strlen(fparams);
        cmd_list_t cmd_new;
        cmd_new.fmt = (char *)malloc(sizeof(char)*(l_fparams+1));
        strncpy(cmd_new.fmt, fparams, l_fparams+1);
        cmd_new.function = function;
        cmd_new.name = (char *)malloc(sizeof(char)*(l_name+1));
        strncpy(cmd_new.name, name, l_name+1);
        cmd_new.nparams = nparam;

        // Copy to command buffer
        osSemaphoreTake(&repo_cmd_sem, portMAX_DELAY);
        {
            cmd_list[cmd_index] = cmd_new;
            if (strcmp(name, "null") != 0 && cmd_is_sorted && cmd_index > 1)
            {
                if (strcmp(cmd_list[cmd_index - 1].name, cmd_list[cmd_index].name) > 0)
                    cmd_is_sorted = 0;
            }
            cmd_index++;
        }
        osSemaphoreGiven(&repo_cmd_sem);
        return cmd_index;
    }
    else
    {
        LOGW(tag, "Unable to add cmd: %s. Buffer full (%d)", name, cmd_index);
        return CMD_ERROR;
    }
}

cmd_t * cmd_get_str(char *name)
{
    cmd_t *cmd_new = NULL;

    //Find inside command buffer
    int i, ok;
    for(i=0; i<SCH_CMD_MAX_ENTRIES; i++)
    {
        osSemaphoreTake(&repo_cmd_sem, portMAX_DELAY);
        ok = strcmp(name, cmd_list[i].name);
        osSemaphoreGiven(&repo_cmd_sem);

        if(ok == 0)
        {
            // Create the command by index
            cmd_new = cmd_get_idx(i);
            break;
        }
    }

    if(cmd_new == NULL)
    {
        LOGW(tag, "Command not found: %s", name);
    }
    return cmd_new;
}

cmd_t * cmd_get_idx(int idx)
{
    cmd_t *cmd_new = NULL;

    if (idx < SCH_CMD_MAX_ENTRIES)
    {
        // Get found command
        osSemaphoreTake(&repo_cmd_sem, portMAX_DELAY);
        cmd_list_t cmd_found = cmd_list[idx];
        osSemaphoreGiven(&repo_cmd_sem);

        // Creates a new command
        cmd_new = (cmd_t *)malloc(sizeof(cmd_t));

        // Fill parameters
        cmd_new->id = idx;
        cmd_new->fmt = cmd_found.fmt;
        cmd_new->function = cmd_found.function;
        cmd_new->nparams = cmd_found.nparams;
        cmd_new->params = NULL;
    }
    else
    {
        LOGW(tag, "Command index found: %d", idx);
    }

    return cmd_new;
}

char * cmd_get_name(int idx)
{
    char *name = NULL;
    if (idx < SCH_CMD_MAX_ENTRIES)
    {
        // Get found command
        osSemaphoreTake(&repo_cmd_sem, portMAX_DELAY);
        cmd_list_t cmd_found = cmd_list[idx];
        osSemaphoreGiven(&repo_cmd_sem);

        LOGV(tag, "Cmd name found: %s", cmd_found.name);
        name = (char *)malloc(strlen(cmd_found.name)+1);
        if(name == NULL)
        {
            LOGW(tag, "Error allocating memory in cmd_get_name");
        }
        else
        {
            strcpy(name, cmd_found.name);
        }
    }
    else
    {
        LOGW(tag, "Command index not found: %d", idx);
    }

    return name;
}

void cmd_add_params_raw(cmd_t *cmd, void *params, int len)
{
    // Check pointers
    if(cmd != NULL && params != NULL)
    {
        LOGD(tag, "Copying %d bytes as parameters", len);
        cmd->params = (char *)malloc((size_t)len);
        memcpy(cmd->params, params, (size_t)len);
    }
}

void cmd_add_params_str(cmd_t *cmd, char *params)
{
    size_t len_param = strlen(params);
    if(len_param > SCH_CMD_MAX_STR_PARAMS)
    {
        LOGW(tag, "Parameters larger than CMD_MAX_STR_PARAMS will be shorted!( (%d > %d)",
             (int)len_param, SCH_CMD_MAX_STR_PARAMS);
        len_param = SCH_CMD_MAX_STR_PARAMS;
    }

    // Check pointers
    if(cmd != NULL && len_param)
    {
        cmd->params = (char *)malloc(sizeof(char)*(len_param+1));
        strncpy(cmd->params, params, len_param+1);
    }
}

void cmd_add_params_var(cmd_t *cmd, ...)
{
    if(cmd != NULL)
    {
        va_list args;
        va_start(args, cmd);

        //Parsing arguments to string
        char str_params[SCH_CMD_MAX_STR_PARAMS];
        vsprintf(str_params, cmd->fmt, args);

        va_end(args);

        //Fill parameters as string
        cmd_add_params_str(cmd, str_params);
    }
}

cmd_t *cmd_parse_from_str(char *buff)
{
    cmd_t *new_cmd = NULL;
    size_t len = strlen(buff);
    int ok = 0;

    // Do not use sscanf in empty string
    if(len > 0)
    {
        int next;
        char tmp_cmd[len + 1];
        memset(tmp_cmd, '\0', (size_t)len+1);

        // Scan a command and parameter string: <command> [parameters]
        LOGV(tag, "New TC: %s (%d)", buff, (int)len);
        ok = sscanf(buff, "%s %n", tmp_cmd, &next);
        LOGV(tag, "Parsed cmd: %s (a: %d, n: %d)", tmp_cmd, ok, next);

        // Check that at least one str was found (the command name)
        if (ok > 0)
        {
            new_cmd = cmd_get_str(tmp_cmd);

            // Check if the command exist
            if (new_cmd != NULL)
            {
                // Check if a parameter was found too (optional)
                if (next < len)
                {
                    memset(tmp_cmd, '\0', (size_t)len+1);
                    strncpy(tmp_cmd, buff + next, (size_t) (len - next));
                    LOGV(tag, "Parsed args: %s", tmp_cmd);
                    cmd_add_params_str(new_cmd, tmp_cmd);
                }
                else
                {
                    cmd_add_params_str(new_cmd, "");
                }
            }
            else
            {
                ok = 0;
            }
        }
    }

    if(ok <= 0)
    {
        LOGE(tag, "Error parsing command!");
    }

    // Return the command filled with parameters or a NULL pointer
    return new_cmd;
}

void cmd_free(cmd_t *cmd)
{
    if(cmd != NULL)
    {
        // Free the params if allocated, we don't need free cmd->fmt because
        // it has not been copied with malloc (see cmd_get_idx)
        free(cmd->params);
        // Free the structure itself
        free(cmd);
    }
}

//static void quicksort_by_name(cmd_list_t* commands, int start, int end)
//{
//    if (start >= end)
//        return;
//
//    int pivot = start;
//
//    for (int i = pivot+1; i <= end; i++)
//    {
//        if (strcmp(commands[i].name, commands[pivot].name) < 0)
//        {
//            cmd_list_t aux = commands[i];
//            for (int j = i; j > pivot; j--)
//                commands[j] = commands[j-1];
//            commands[pivot] = aux;
//            pivot++;
//        }
//    }
//
//    quicksort_by_name(commands, start, pivot);
//    quicksort_by_name(commands, pivot+1, end);
//}
//
//static void sort_cmd_list()
//{
//    if (!cmd_is_sorted)
//    {
//        LOGD(tag, "Sorting Command List");
//
//        quicksort_by_name(cmd_list, 0, cmd_index-1);
//        cmd_is_sorted = 1;
//
//        LOGD(tag, "Command List Sorted");
//    }
//}

void cmd_print_all(void)
{

    LOGD(tag, "Command list");
    osSemaphoreTake(&repo_cmd_sem, portMAX_DELAY);

    //Make sure no LOG functions are used in this zone
    osSemaphoreTake(&log_mutex, portMAX_DELAY);
    printf("%5s %s %25s\n", "Index", "Name", "Params");
    int i;
    for(i=0; i<cmd_index; i++)
    {
        int printed = printf("%5d %s", i, cmd_list[i].name);
        if (*cmd_list[i].fmt != '\0')
            printf(" %s\n", cmd_list[i].fmt);
        else
            printf("\n");
    }
    osSemaphoreGiven(&log_mutex);
    //End log_mutex, can use LOG functions

    osSemaphoreGiven(&repo_cmd_sem);

}

int cmd_repo_init(void)
{
    // Init repository mutex
    osSemaphoreCreate(&repo_cmd_sem);
    cmd_index = 0;  // Reset registered command counter

    // Init repos
#if SCH_TEST_ENABLED
    cmd_test_init();
#endif
    cmd_obc_init();
    cmd_drp_init();
    cmd_console_init();
#if SCH_FP_ENABLED
    cmd_fp_init();
#endif
#ifdef SCH_USE_NANOPOWER
    cmd_eps_init();
#endif
#if SCH_COMM_ENABLE
    cmd_com_init();
    cmd_tm_init();
#endif
#ifdef SCH_SEN_ENABLED
    cmd_sensors_init();
#endif
#ifdef SCH_USE_GSSB
    cmd_gssb_init();
#endif

    int n_cmd;
    int last_cmd_index = cmd_index;

    // Fill command buffer with cmd_null command
    do
    {
        n_cmd = cmd_add("null", cmd_null, "", 0);
    }
    while(n_cmd < SCH_CMD_MAX_ENTRIES);

    // Restore the number of not null commands
    cmd_index = last_cmd_index;

    return CMD_OK;
}

void cmd_repo_close(void)
{
    int i;
    for(i=0; i<SCH_CMD_MAX_ENTRIES; i++)
    {
        free(cmd_list[i].name);
        free(cmd_list[i].fmt);
    }

    cmd_index = 0;
}

int cmd_null(char *fparams, char *params, int nparam)
{
    LOGD(tag, "cmd_null was used with params format: %s and params string: %s", fparams, params);
    return CMD_ERROR;
}

int cmd_print(cmd_t* cmd)
{
    LOGV(tag, "Command Name:%s\n",cmd_get_name(cmd->id));
    LOGV(tag, "\tid: %d\n\tnparams: %d\n\tfmt: %s\n\tparams: %s\n\tfunction: %p\n", cmd->id, cmd->nparams, cmd->fmt, cmd->params, cmd->function);
    return 0;
}

char* cmd_get_fmt(char* name)
{
    char* format = malloc(sizeof(char)*30);
    int i, ok;
    for(i=0; i<SCH_CMD_MAX_ENTRIES; i++)
    {
        osSemaphoreTake(&repo_cmd_sem, portMAX_DELAY);
        ok = strcmp(name, cmd_list[i].name);
        osSemaphoreGiven(&repo_cmd_sem);

        if(ok == 0)
        {
            strcpy(format,cmd_list[i].fmt);
            break;
        }
    }
    return format;
}
