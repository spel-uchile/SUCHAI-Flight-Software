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

#include <stdio.h>

/* System includes */
#include "SUCHAI_config.h"

/* Task includes */
#include "System/include/taskTest.h"
#include "System/include/taskDispatcher.h"
#include "System/include/taskExecuter.h"
#include "System/include/taskHousekeeping.h"

#include "OS/include/osThread.h"
#include "OS/include/osScheduler.h"
#include "OS/include/osQueue.h"
#include "OS/include/osSemphr.h" 


#if !__linux__
    #if defined(__XC16__)
        #include <xc.h>
    #else
        #include <p24FJ256GA110.h>
    #endif

    /* Drivers includes */

    /* RTOS Includes */
    #include "FreeRTOSConfig.h"
    #include "FreeRTOS.h"
    #include "task.h"
    #include "queue.h"
    #include "semphr.h"
    #include "list.h"

    /* Config Words */
    // CONFIG3
    #pragma config WPFP = WPFP511           // Write Protection Flash Page Segment Boundary (Highest Page (same as page 170))
    #pragma config WPDIS = WPDIS            // Segment Write Protection Disable bit (Segmented code protection disabled)
    #pragma config WPCFG = WPCFGDIS         // Configuration Word Code Page Protection Select bit (Last page(at the top of program memory) and Flash configuration words are not protected)
    #pragma config WPEND = WPENDMEM         // Segment Write Protection End Page Select bit (Write Protect from WPFP to the last page of memory)

    // CONFIG2
    #pragma config POSCMOD = XT             // Primary Oscillator Select (XT oscillator mode selected)
    #pragma config I2C2SEL = PRI            // I2C2 Pin Select bit (Use Default SCL2/SDA2 pins for I2C2)
    #pragma config IOL1WAY = OFF            // IOLOCK One-Way Set Enable bit (Unlimited Writes To RP Registers)
    #pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSCO functions as CLKO (FOSC/2))
    #pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Both Clock Switching and Fail-safe Clock Monitor are disabled)
    #pragma config FNOSC = PRIPLL           // Oscillator Select (Primary oscillator (XT, HS, EC) with PLL module (XTPLL,HSPLL, ECPLL))
    #pragma config IESO = ON                // Internal External Switch Over Mode (IESO mode (Two-speed start-up) enabled)

    // CONFIG1
    #pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
    #pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
    #pragma config WINDIS = OFF             // Watchdog Timer Window (Standard Watchdog Timer is enabled,(Windowed-mode is disabled))
    #pragma config FWDTEN = ON              // Watchdog Timer Enable (Watchdog Timer is enabled)
    #pragma config ICS = PGx1               // Comm Channel Select (Emulator functions are shared with PGEC1/PGED1)
    #pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
    #pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
    #pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)
#endif

/* Global variables */
osSemaphore repoDataSem;
osQueue dispatcherQueue, executerCmdQueue, executerStatQueue;

static void on_reset(void);

int main(void)
{
        //lenguaje C orientado a microcontroladores rev 1
        //quidel.inele.ufro.cl
        //cc5x
        //printf("%i\n",0xFFFFFFFF);
        /* Initializing shared Queues */
        dispatcherQueue = osQueueCreate(25,sizeof(DispCmd));
        executerCmdQueue = osQueueCreate(1,sizeof(ExeCmd));
        executerStatQueue = osQueueCreate(1,sizeof(int));

        /* Initializing shared Semaphore */
        osSemaphoreCreate(&repoDataSem);

        /* Crating all task (the others are created inside taskDeployment) */
        osCreateTask(taskDispatcher,"dispatcher",2*configMINIMAL_STACK_SIZE,NULL,3);
        osCreateTask(taskExecuter, "executer", 5*configMINIMAL_STACK_SIZE, NULL, 4);
        osCreateTask(taskHousekeeping, "housekeeping", 2*configMINIMAL_STACK_SIZE, NULL, 2);
        
        /* Configure Peripherals */
        
        /* On reset */
        on_reset();

        /* Start the scheduler. Should never return */
        osScheduler();

    return 0;
}

#if !__linux__
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

#endif

/**
 * Performs initialization actions
 */
void on_reset(void)
{
    repo_onResetCmdRepo(); //Command repository initialization
    dat_onResetCubesatVar(); //Update status repository
}
