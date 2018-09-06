/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2018, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
 *      Copyright 2018, Tomas Opazo Toro, tomas.opazo.t@gmail.com
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



void taskExecuter(void *param)
{

    static const char *tag = "Executer";

    LOGI((char*)param, "Started");


    cmd_t *run_cmd = NULL;

    int cmd_stat, queue_stat;
        
    while(1)
    {
        /* Read the CMD that Dispatcher sent - BLOCKING */
        queue_stat = osQueueReceive(executer_cmd_queue, &run_cmd, portMAX_DELAY);

        if(queue_stat == pdPASS)
        {
#if LOG_LEVEL >= LOG_LVL_INFO
            char *cmd_name = cmd_get_name(run_cmd->id);
            LOGI((char*)param, "Running the command: %s...", cmd_name);
            free(cmd_name);
#endif
            /* Commands may take a long time, so reset the WDT */
            //ClrWdt();

            /* Execute the command */
            // TODO: Check that we are dereferencing a valid function pointer
            cmd_stat = run_cmd->function(run_cmd->fmt, run_cmd->params, run_cmd->nparams);
            //Finish time
            portTick finish_time = osTaskGetTickCount();
            //int dummy;
            //portTick init_time;
            //sscanf(run_cmd->params, run_cmd->fmt, &dummy, &
            cmd_free(run_cmd);
            run_cmd = NULL;
            /* Commands may take a long time, so reset the WDT */
            //ClrWdt();
            LOGI((char*)param, "Command result: %d ; End time: %d ", cmd_stat, finish_time);

            /* Send the result to Dispatcher - BLOCKING */
            osQueueSend(executer_stat_queue, &cmd_stat, portMAX_DELAY);
        }
    }
}
