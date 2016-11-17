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

#include <string.h>

#include "cmdRepository.h"

# define CMD_MAX_LEN 100
cmd_t cmd_list[CMD_MAX_LEN];
int cmd_index = 0;

/**
 * Register a command in the system
 * 
 * @param name Str. Name of the comand, max CMD_NAME_LEN characters
 * @param function Pointer to command function
 * @param params Str. to define command parameters
 * @return None
 */
void cmd_add(char* name, cmdFunction function, int nparam)
{    
   
    // Create new command 
    cmd_t cmd_new;
    cmd_new.function = function;
    cmd_new.id = cmd_index;
    cmd_new.nparam = nparam; 
   strncpy(cmd_new.name, name, CMD_NAME_LEN);
    
    // Copy to command buffer
    if (cmd_index < CMD_MAX_LEN){
        cmd_list[cmd_index] = cmd_new;
    }
    
    cmd_index++;
}

/**
 * Get command by name
 * 
 * @param name Str. Command name
 * @return cmd_t Command structure. Null if command does not exists.
 */
cmd_t cmd_get_str(char *name)
{
    cmd_t cmd_new;
    
    //Find inside command buffer
    int i, ok;
    for(i=0; i<CMD_MAX_LEN; i++)
    {
        ok = strncmp(name, cmd_list[i].name, CMD_MAX_LEN);
        if(ok == 0)
        {
            cmd_new = cmd_list[i];
            return cmd_new;
        }
    }

    return cmd_new;
}

/**
 * Get command by index or id
 * 
 * @param idx Int command index or Id.
 * @return cmd_t Command structure. Null if command does not exists. 
 */
cmd_t cmd_get_idx(int idx)
{
    cmd_t cmd_new;
    
    if (idx < CMD_MAX_LEN)
    {
        cmd_new = cmd_list[idx];
    }
    
    return cmd_new;
}

/**
 * Print the list registered commands
 */
void cmd_print_all(void){
    
    printf("Name\t; Index\t; Params\n");
    int i;
    for(i=0; i<cmd_index; i++)
    {
        printf("%s\t; %d\t; %d\n", cmd_list[i].name, cmd_list[i].id, cmd_list[i].nparam);
    }
}

/**
 * Initializes the command buffer adding null_cmd
 * 
 * @return 1
 */
int cmd_init(void)
{
    // Create a null command 
    cmd_t cnull;
    cnull.function = cmd_null;
    cnull.id = cmd_index;
    cnull.nparam = 0;
    strncpy(cnull.name, "null", CMD_NAME_LEN);
    
    // Reset the command buufer
    int i;
    for(i=0; i < CMD_MAX_LEN; i++){
        cmd_list[i] = cnull;
    }
    
    // Init repos
    obc_onResetCmdOBC();
    drp_onResetCmdDRP();
    con_onResetCmdCON();
    
    return 1;
}

/**
 * Null command, just print to stdout
 *
 * @param param Not used
 * @return 1, allways successful
 */
int cmd_null(int nparam, void *param)
{
    printf("cmd_null was used with %d params at 0x%X\n", nparam, (int)&param);
    return 1;
}
