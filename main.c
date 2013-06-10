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

#if defined(__XC16__)
    #include <xc.h>
#else
    #include <p24FJ256GA110.h>
#endif

#include <stdio.h>

/* Drivers includes */

/* System includes */
#include "SUCHAI_config.h"

/* RTOS Includes */
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "list.h"

/* Task includes */
#include "taskTest.h"

/* Config Words */

int main(void)
{
    /* Initializing shared Queues */

    /* Initializing shared Semaphore */

    /* Crating all task (the others are created inside taskDeployment) */
    xTaskCreate(taskTest, (signed char*)"taskTest", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    /* Configure Peripherals */

    /* Start the scheduler. Should never return */
    printf(">>Starting FreeRTOS scheduler [->]\r\n");
    vTaskStartScheduler();

    while(1)
    {
        printf(">>\nFreeRTOS [FAIL]\n");
    }
    return 0;
}

/**
 * Task idle handle function. Performs operations inside the idle task
 * configUSE_IDLE_HOOK must be set to 1
 */
void vApplicationIdleHook(void)
{
    ClrWdt();
}

/**
 * Stack overflow handle function.
 * configCHECK_FOR_STACK_OVERFLOW must be set to 1 or 2
 * 
 * @param pxTask Task handle
 * @param pcTaskName Task name
 */
void vApplicationStackOverflowHook(xTaskHandle* pxTask, signed char* pcTaskName)
{
    printf(">> Stak overflow! - TaskName: %s\n", (char *)pcTaskName);
    
    /* Stack overflow handle */
    while(1);
}
