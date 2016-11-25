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

#include "include/taskHousekeeping.h"

extern osQueue dispatcherQueue; /* Commands queue */

void taskHousekeeping(void *param)
{
    printf(">>[Houskeeping] Started\r\n");
    
    portTick delay_ms    = 1000;    //Task period in [ms]
    portTick delay_ticks = osDefineTime(delay_ms); //Task period in ticks

    unsigned int elapsed_sec = 0;       // Seconds count
    unsigned int _10sec_check = 1;//10;     //10[s] condition
    unsigned int _10min_check = 2;//10*60;  //10[m] condition
    unsigned int _1hour_check = 3;//60*60;  //1[h] condition

    DispCmd NewCmd;
    NewCmd.idOrig = CMD_IDORIG_THOUSEKEEPING; //Housekeeping

    portTick xLastWakeTime = osTaskGetTickCount();
    
    while(1)
    {

        osTaskDelayUntil(&xLastWakeTime, delay_ticks); //Suspend task
        elapsed_sec += delay_ms/1000; //Update seconds counts

        /* 10 seconds actions */
        if((elapsed_sec % _10sec_check) == 0)
        {
            printf("[Houskeeping] _10sec_check\n");

            NewCmd.cmdId = obc_id_get_rtos_memory;
            NewCmd.param = 0;
            osQueueSend(dispatcherQueue, &NewCmd, portMAX_DELAY);
        }

        /* 10 minutes actions */
        if((elapsed_sec % _10min_check) == 0)
        {
            printf("[Houskeeping] _10min_check\n");
            NewCmd.cmdId = drp_id_print_CubesatVar;
            NewCmd.param = 0;
            osQueueSend(dispatcherQueue, &NewCmd, portMAX_DELAY);
        }

        /* 1 hours actions */
        if((elapsed_sec % _1hour_check) == 0)
        {
            printf("[Houskeeping] _1hour_check\n");

            NewCmd.cmdId = drp_id_update_dat_CubesatVar_hoursWithoutReset;
            NewCmd.param = 1; //Add 1 hour
            osQueueSend(dispatcherQueue, &NewCmd, portMAX_DELAY);
        }
    }
}
