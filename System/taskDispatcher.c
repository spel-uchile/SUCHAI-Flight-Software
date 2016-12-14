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

#include "include/taskDispatcher.h"

//extern osQueue dispatcherQueue; /* Commands queue */
//extern osQueue executerCmdQueue; /* Executer commands queue */
//extern osQueue executerStatQueue; /* Executer result queue */

void taskDispatcher(void *param)
{
	printf(">>[Dispatcher] Started\n");

    portBASE_TYPE status; /* Status of cmd reading operation */

    cmd_t new_cmd; /* The new cmd readed */
    int cmd_result;

    while(1)
    {
        /* Read newCmd from Queue - Blocking */
        status = osQueueReceive(dispatcherQueue, &newCmd, portMAX_DELAY);

        if(status == pdPASS)
        {
			/* Gets command metadata*/
            cmdId = newCmd.cmdId;
            idOrig = newCmd.idOrig;
            sysReq = newCmd.sysReq;
            cmdParam = newCmd.param;

            /* Check if command is executable */
            if(check_if_executable(&newCmd))
            {
				printf("[Dispatcher] Cmd: %X, Param: %d, Orig: %X\n", new_cmd.id, new_cmd.nparam, -1);

                /* Send the command to executer Queue - BLOCKING */
                xQueueSend(executerCmdQueue, &new_cmd, portMAX_DELAY);
                osQueueSend(executerCmdQueue, &exeCmd, portMAX_DELAY);

                /* Get the result from Executer Stat Queue - BLOCKING */
                osQueueReceive(executerStatQueue, &cmdResult, portMAX_DELAY);
                xQueueReceive(executerStatQueue, &cmd_result, portMAX_DELAY);
            }
        }
    }
}

int check_if_executable(cmd_t *newCmd)
{
//    int cmdId, idOrig, sysReq, param; /* Cmd metadata */
//
//    cmdId = newCmd->cmdId;
//    idOrig = newCmd->idOrig;
//    sysReq = 0; //repo_getsysReq(cmdId);
//    param = newCmd->param;
//
//    if(cmdId == CMD_CMDNULL)
//    {
//        printf("[Dispatcher] Cmd: %X from %X refused because was NULL\n", cmdId, idOrig);
//        return 0;
//    }
//
//    #if (SCH_CHECK_IF_EXECUTABLE_SOC == 0)
//        dat_setCubesatVar(dat_eps_soc, CMD_SYSREQ_MAX);
//    #endif
//
//    // Compare sysReq with SOC
//    if(dat_getCubesatVar(dat_eps_soc) < sysReq)
//    {
//        printf("[Dispatcher] Cmd: %X from %X sysReq %d refused because of SOC %d\n", cmdId, idOrig, sysReq, dat_getCubesatVar(dat_eps_soc));
//        return 0;
//    }

    return 1;
}
