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

#include "include/taskExecuter.h"

extern osQueue executerCmdQueue; /* Comands queue*/
extern osQueue executerStatQueue; /* Comands queue*/

void taskExecuter(void *param)
{
    printf(">>[Executer] Started\n");

    ExeCmd RunCmd;
    int cmdStat, queueStat, cmdParam;
        
    while(1)
    {
        /* Read the CMD that Dispatcher sent - BLOCKING */
        queueStat = osQueueReceive(executerCmdQueue, &RunCmd, portMAX_DELAY);
        
        if(queueStat == pdPASS)
        {
            printf("[Executer] Running a command...\n");
            /* Commands may take a long time, so reset the WDT */
            ClrWdt();

            /* Execute the command */
            cmdParam = RunCmd.param;
            cmdStat = RunCmd.fnct((void *)&cmdParam);

            /* Commands may take a long time, so reset the WDT */
            ClrWdt();
            
            printf("[Executer] Command result: %d\n", cmdStat);
            
            /* Send the result to Dispatcher - BLOCKING */
            osQueueSend(executerStatQueue, &cmdStat, portMAX_DELAY);

        }
    }
}
