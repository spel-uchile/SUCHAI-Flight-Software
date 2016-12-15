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

#include "taskExecuter.h"

extern osQueue executerCmdQueue; /* Comands queue*/
extern osQueue executerStatQueue; /* Comands queue*/

void taskExecuter(void *param)
{
    printf(">>[Executer] Started\n");

    cmd_t run_cmd;
    int cmd_stat, queue_stat;
        
    while(1)
    {
        /* Read the CMD that Dispatcher sent - BLOCKING */
        queue_stat = osQueueReceive(executerCmdQueue, &run_cmd, portMAX_DELAY);

        if(queue_stat == pdPASS)
        {
            printf("[Executer] Running a command...\n");
            /* Commands may take a long time, so reset the WDT */
            ClrWdt();

            /* Execute the command */
            cmd_stat = run_cmd.function(run_cmd.nparam, run_cmd.params);
            free(run_cmd.params);

            /* Commands may take a long time, so reset the WDT */
            ClrWdt();
            
            printf("[Executer] Command result: %d\n", cmd_stat);
            
            /* Send the result to Dispatcher - BLOCKING */
            osQueueSend(executerStatQueue, &cmd_stat, portMAX_DELAY);

        }
    }
}
