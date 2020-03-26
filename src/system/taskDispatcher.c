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
        status = osQueueReceive(dispatcher_queue, &new_cmd, portMAX_DELAY);

        if(status == pdPASS)
        {
            /* Check if command is executable */
            if (check_if_executable(new_cmd))
            {

                /* Send the command to executer Queue - BLOCKING */
                osQueueSend(executer_cmd_queue, &new_cmd, portMAX_DELAY);
                LOGD(tag, "Cmd: %X, Param: %p, Orig: %X", new_cmd->id, &(new_cmd->params), -1);

                /* Get the result from Executer Stat Queue - BLOCKING */
                osQueueReceive(executer_stat_queue, &cmd_result, portMAX_DELAY);
            }
        }
    }
}

int check_if_executable(cmd_t *new_cmd)
{
    return 1;
}
