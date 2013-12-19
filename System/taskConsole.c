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

#include "taskConsole.h"

extern xQueueHandle dispatcherQueue;



const char console_baner[] =   
"______________________________________________________________________________\n\
                     ___ _   _  ___ _  _   _   ___ \n\
                    / __| | | |/ __| || | /_\\ |_ _|\n\
                    \\__ \\ |_| | (__| __ |/ _ \\ | | \n\
                    |___/\\___/ \\___|_||_/_/ \\_\\___|\n\
______________________________________________________________________________\n\n";


//"\n\n====== WELCOME TO THE SUCHAI CONSOLE - PRESS ANY KEY TO START ======\n\r"

void taskConsole(void *param)
{
#if (SCH_GRL_VERBOSE)
    con_printf(">>[Console] Started\r\n");
    char ret[10];
#endif

    const unsigned int Delayms = 250 / portTICK_RATE_MS;
    DispCmd NewCmd;
    NewCmd.idOrig = CMD_IDORIG_TCONSOLE; /* Consola */
    NewCmd.cmdId = CMD_CMDNULL;  /* cmdNULL */
    NewCmd.param = 0;

    /* Initializing console */
    con_init();

#if (SCH_GRL_VERBOSE>=1)
    con_printf((char *)console_baner);
#endif

    while(1)
    {
        vTaskDelay(Delayms);

        /* Parsing command - return CmdDisp structure*/
        NewCmd = con_cmd_handler();

        /* cmdId = 0xFFFF means no new command */
        if(NewCmd.cmdId != CMD_CMDNULL)
        {
            con_printf("\r\n");

            #if (SCH_GRL_VERBOSE >=1)
                /* Print the command code */
                sprintf( ret, "0x%X", (unsigned int)NewCmd.cmdId );
                con_printf("[Console] Se genera comando: ");
                con_printf(ret); con_printf("\n\0");
            #endif

            /* Queue NewCmd - Blocking */
            xQueueSend(dispatcherQueue, &NewCmd, portMAX_DELAY);
        }        
    }
}
