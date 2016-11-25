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
 
#include "include/repoCommand.h"

/* Add external cmd arrays */
extern cmdFunction obc_Function[];
extern cmdFunction drp_Function[];

extern int obc_sysReq[];
extern int drp_sysReq[];

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

