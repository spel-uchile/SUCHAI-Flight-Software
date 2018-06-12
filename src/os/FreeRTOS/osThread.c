/**
 * @file  FreeRTOS/thread.c
 * @author Ignacio Ibanez Aliaga 
 * @date 26-10-2016
 * @copyright GNU Public License.
 *
 * Creation of functions related with time for systems operating FreeRTOS.
 * 
 */
#include "osThread.h"

/**
 * create a task in FreeRTOS
 */
int osCreateTask(void (*functionTask)(void *), char* name, unsigned short size, void * parameters, unsigned int priority, os_thread *thread)
{
    BaseType_t created = xTaskCreate((*functionTask), name, size, parameters, priority, NULL);
    return created == pdPASS ? 0 : 1;
}

void osTaskDelete(void *task_handle)
{
    vTaskDelete(task_handle);
}
