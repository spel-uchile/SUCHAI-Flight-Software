/**
 * @file  FreeRTOS/scheduler.c
 * @author Ignacio Ibanez Aliaga 
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * Creation of functions related with thread for systems operating FreeRTOS.
 * 
 */

#include "osScheduler.h"

/**
 * starts the scheduler of the system operating
 */
void osScheduler(os_thread* thread_id, int n_thread)
{
    printf("[INFO] Starting FreeRTOS scheduler...\n");
    vTaskStartScheduler();

    /* FIXME: Should the system restart? */
    while(1)
    {
    	printf("[ERROR] FreeRTOS scheduler stopped!\n");
    }
}
