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

/* Add external cmd arrays */
extern cmdFunction obc_Function[];
extern cmdFunction drp_Function[];
extern cmdFunction conFunction[];

extern int obc_sysReq[];
extern int drp_sysReq[];

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
void cmd_add(char *name, cmdFunction function, char *params)
{    
   
    // Create new command 
    cmd_t cmd_new;
    cmd_new.function = function;
    cmd_new.id = cmd_index;
    strncpy(cmd_new.name, name, CMD_NAME_LEN);
    strncpy(cmd_new.params, params, CMD_NAME_LEN);
    
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
 * Print the list registered commands
 */
void cmd_print_all(void){
    int i;
    for(i=0; i<cmd_index; i++)
    {
        printf("Cmd: %s. Id: %d. Par:%s\n", cmd_list[i].name, cmd_list[i].id, cmd_list[i].params);
    }
}

/**
 * Returns the energy level asociated to each command
 *
 * @param cmdID Command intentifier
 * @return Requiered energy level
 */
int repo_getsysReq(int cmdID)
{
    int cmdOwn, cmdNum;
    int result;

    cmdNum = (unsigned char)cmdID;
    cmdOwn = (unsigned char)(cmdID>>8);

    switch (cmdOwn)
    {
        case CMD_OBC:
            if(cmdNum >= OBC_NCMD)
                result = CMD_SYSREQ_MIN;
            else
                result = obc_sysReq[cmdNum];
            break;

        case CMD_DRP:
            if(cmdNum >= DRP_NCMD)
                result = CMD_SYSREQ_MIN;
            else
                result = drp_sysReq[cmdNum];
            break;
            
        default:
            result = CMD_SYSREQ_MIN;
            break;
    }

    return result;
}

/**
 * Returns a pointer with the function asociated to each cmdID. If the id is not
 * registered, returns the standar null command.
 *
 * @param cmdID Command id
 * @return Pointer to function of type cmdFunction
 */
cmdFunction repo_getCmd(int cmdID)
{
    int cmdOwn, cmdNum;
    cmdFunction result;

    cmdNum = (unsigned char)cmdID;
    cmdOwn = (unsigned char)(cmdID>>8);

    switch (cmdOwn)
    {
        case CMD_OBC:
            if(cmdNum >= OBC_NCMD)
                result = cmdNULL;
            else
                result = obc_Function[cmdNum];
            break;

        case CMD_DRP:
            if(cmdNum >= DRP_NCMD)
                result = cmdNULL;
            else
                result = drp_Function[cmdNum];
            break;
        
        default:
            result = cmdNULL;
            break;
    }

    return result;
}

/**
 * Initializes all cmd arrays
 *
 * @return 1, allways successful
 */
int repo_onResetCmdRepo(void)
{
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
int cmdNULL(void *param)
{
    int arg=*( (int *)param );
    printf("cmdNULL was used with param %d\n", arg);

    return 1;
}

