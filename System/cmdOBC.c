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
 
#include "cmdOBC.h"

/**
 * This function registers the list of command in the system, initializing the
 * functions array. This function must be called at every system start up.
 */
void cmd_obc_init(void)
{
//    cmd_add("reset", obc_reset, "");
//    cmd_add("get_mem", obc_get_rtos_memory, 0);
}

/**
 * Perform a microcontroller software reset
 * 
 * @param param Not used
 * @return 1, but function never returns
 */
int obc_reset(char *fmt, char *params, int nparams)
{
    printf("Resetting system NOW!!\n");

    OBC_SYS_RESET();

    /* Never get here */
    return CMD_OK;
}

/**
 * Performs debug taks over current RTOS. Get rtos memory usage in bytes
 *
 * @param param Not used
 * @return Availible heap memory in bytes
 */
int obc_get_rtos_memory(char *fmt, char *params, int nparams)
{
    #if __linux__
        int mem_heap = 666;
        printf("Linux memory: %d\n", mem_heap);
    #else
        size_t mem_heap = xPortGetFreeHeapSize();
        printf("Free RTOS memory: %d\n", mem_heap);
    #endif
        
    //return mem_heap;
    return CMD_OK;
}
