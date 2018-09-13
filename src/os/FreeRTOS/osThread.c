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
#ifdef AVR32
    // FreeRTOS 7.0.0
    portBASE_TYPE created = xTaskCreate((*functionTask), (signed char*)name, size, parameters, priority, NULL);
#else
    // FreeRTOS > 8.0.0
    BaseType_t created = xTaskCreate((*functionTask), name, size, parameters, priority, NULL);
#endif
    return created == pdPASS ? 0 : 1;
}

void osTaskDelete(void *task_handle)
{
    vTaskDelete(task_handle);
}
