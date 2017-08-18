/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2017, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
 *      Copyright 2017, Tomas Opazo Toro, tomas.opazo.t@gmail.com
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

#include "taskDispatcher.h"

static const char *tag = "Dispatcher";

void taskDispatcher(void *param)
{
	LOGI(tag, "Started");

    int status; /* Status of cmd reading operation */

    cmd_t *new_cmd = NULL; /* The new cmd read */
    int cmd_result;

    while(1)
    {
        /* Read new_cmd from Queue - Blocking */
        status = osQueueReceive(dispatcherQueue, &new_cmd, portMAX_DELAY);

        if(status == pdPASS)
        {
            /* Check if command is executable */
            if(check_if_executable(new_cmd))
            {
				LOGD(tag, "Cmd: %X, Param: %p, Orig: %X", new_cmd->id, &(new_cmd->params), -1);

                /* Send the command to executer Queue - BLOCKING */
                osQueueSend(executerCmdQueue, &new_cmd, portMAX_DELAY);

                /* Get the result from Executer Stat Queue - BLOCKING */
                osQueueReceive(executerStatQueue, &cmd_result, portMAX_DELAY);
            }
        }
    }
}

int check_if_executable(cmd_t *new_cmd)
{
//    int cmdId, idOrig, sysReq, param; /* Cmd metadata */
//
//    cmdId = new_cmd->cmdId;
//    idOrig = new_cmd->idOrig;
//    sysReq = 0; //repo_getsysReq(cmdId);
//    param = new_cmd->param;
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
