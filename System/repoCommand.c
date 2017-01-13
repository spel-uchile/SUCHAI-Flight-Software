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

#include "repoCommand.h"

#define CMD_MAX_LEN 100
#define CMD_MAX_STR_PARAMS 64

/* Global variables */
cmd_list_t cmd_list[CMD_MAX_LEN];
int cmd_index = 0;

void cmd_add(char* name, cmdFunction function, char* fparams, int nparam)
{
    if (cmd_index < CMD_MAX_LEN)
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
        cmd_list[cmd_index] = cmd_new;
        cmd_index++;
    }
}

cmd_t * cmd_get_str(char *name)
{
    cmd_t *cmd_new = NULL;

    //Find inside command buffer
    int i, ok;
    for(i=0; i<CMD_MAX_LEN; i++)
    {
        ok = strcmp(name, cmd_list[i].name);
        if(ok == 0)
        {
            // Create the command by index
            cmd_new = cmd_get_idx(i);
            break;
        }
    }

    return cmd_new;
}

cmd_t * cmd_get_idx(int idx)
{
    cmd_t *cmd_new = NULL;

    if (idx < CMD_MAX_LEN)
    {
        // Get found command
        cmd_list_t cmd_found = cmd_list[idx];

        // Creates a new command
        cmd_new = (cmd_t *)malloc(sizeof(cmd_t));

        // Fill parameters
        cmd_new->id = idx;
        cmd_new->fmt = cmd_found.fmt;
        cmd_new->function = cmd_found.function;
        cmd_new->nparams = cmd_found.nparams;
        cmd_new->params = NULL;
    }

    return cmd_new;
}

void cmd_add_params_str(cmd_t *cmd, char *params)
{
    size_t len_param = strlen(params);

    // Check pointers
    if(cmd != NULL && len_param)
    {
        cmd->params = (char *)malloc(sizeof(char)*(len_param+1));
        strncpy(cmd->params, params, len_param+1);
    }

    return;
}

void cmd_add_params_var(cmd_t *cmd, ...)
{
    va_list args;
    va_start(args, cmd->fmt);

    //Parsing arguments to string
    char str_params[CMD_MAX_STR_PARAMS];
    vsprintf(str_params, cmd->fmt, args);

    va_end(args);

    //Fill parameters as string
    cmd_add_params_str(cmd, str_params);
}

void cmd_free(cmd_t *cmd)
{
    if(cmd != NULL) {
        // Free the params if allocated
        free(cmd->params);
        // Free the structure itself
        free(cmd);
    }
}

void cmd_print_all(void)
{

    printf("Index\t; name\t; Params\n");
    int i;
    for(i=0; i<cmd_index; i++)
    {
        printf("%d\t; %s\t; %s\n", i, cmd_list[i].name, cmd_list[i].fmt);
    }
}

int cmd_repo_init(void)
{
    // Create a null command
    char *name = "null";
    char *fparams = "null"; //line duplicate
    size_t l_name = strlen(name);
    size_t l_fparams = strlen(fparams);
    cmd_list_t cnull;
    cnull.nparams = 0;
    cnull.fmt = (char *)malloc(sizeof(char)*l_fparams+1);
    strncpy(cnull.fmt, "null", l_fparams+1);
    cnull.function = cmd_null;
    cnull.name = (char *)malloc(sizeof(char)*l_name+1);
    strncpy(cnull.name, "null", l_name+1);

    // Reset the command buffer
    int i;
    for(i=0; i < CMD_MAX_LEN; i++){
        cmd_list[i] = cnull;
    }

    // Init repos
    obc_onResetCmdOBC();
    drp_onResetCmdDRP();
    con_onResetCmdCON();
    test_cmd_init();

    return 1;
}



int cmd_null(char *fparams, char *params, int nparam)
{
    printf("cmd_null was used with params format %s params at %s\n", fparams, params);
    return 1;
}
