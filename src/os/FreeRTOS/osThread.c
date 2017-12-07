/**
 * @file  FreeRTOS/thread.c
 * @author Ignacio Ibanez Aliaga 
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * Creation of functions related with time for systems operating FreeRTOS.
 * 
 */
#include <osThread.h>

/**
 * create a task in FreeRTOS
 */
void osCreateTask(void (*functionTask)(void *), char* name, unsigned short size, void * parameters, unsigned int priority, os_thread *thread)
{
    xTaskCreate((*functionTask), name, size, parameters, priority, NULL);
}
