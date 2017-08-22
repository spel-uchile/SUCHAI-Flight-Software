/**
 * @file  FreeRTOS/scheduler.c
 * @author Ignacio Ibanez Aliaga 
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * Creation of functions related with thread for systems operating FreeRTOS.
 * 
 */

#include <osScheduler.h>

/**
 * starts the scheduler of the system operating
 */
void osScheduler(os_thread** thread_id, int n_thread)
{
    printf(">>Starting FreeRTOS scheduler [->]\r\n");
    vTaskStartScheduler();

    /* FIXME: Should the system restart? */
//    while(1)
//    {
//    	printf("\n>>FreeRTOS [FAIL]\n");
//    }
}
