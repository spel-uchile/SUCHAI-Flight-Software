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

#include "taskExecuter.h"

static const char *tag = "Executer";

void taskExecuter(void *param)
{
    LOGI(tag, "Started");

    cmd_t *run_cmd = NULL;
    int cmd_stat, queue_stat;
        
    while(1)
    {
        /* Read the CMD that Dispatcher sent - BLOCKING */
        queue_stat = osQueueReceive(executerCmdQueue, &run_cmd, portMAX_DELAY);

        if(queue_stat == pdPASS)
        {
            LOGI(tag, "Running a command...");
            /* Commands may take a long time, so reset the WDT */
            ClrWdt();

            /* Execute the command */
            cmd_stat = run_cmd->function(run_cmd->fmt, run_cmd->params, run_cmd->nparams);
            cmd_free(run_cmd);

            /* Commands may take a long time, so reset the WDT */
            ClrWdt();
            
            LOGI(tag, "Command result: %d", cmd_stat);
            
            /* Send the result to Dispatcher - BLOCKING */
            osQueueSend(executerStatQueue, &cmd_stat, portMAX_DELAY);
        }
    }
}
