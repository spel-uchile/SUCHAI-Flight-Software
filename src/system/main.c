/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2018, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2018, Camilo Rojas Milla, camrojas@uchile.cl
 *      Copyright 2018, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *      Copyright 2018, Matias Ramirez Martinez, nicoram.mt@gmail.com
 *      Copyright 2018, Tamara Gutierrez Rojo TGR_93@hotmail.com
 *      Copyright 2018, Ignacio Ibanez Aliaga, ignacio.ibanez@usach.cl
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

#include "main.h"

const char *tag = "main";

#ifdef ESP32
void app_main()
#else
int main(void)
#endif
{
    /* On reset */
    on_reset();

    /* Init software subsystems */
    log_init();      // Logging system
    cmd_repo_init(); // Command repository initialization
    dat_repo_init(); // Update status repository

    /* Initializing shared Queues */
    dispatcher_queue = osQueueCreate(25,sizeof(cmd_t *));
    if(dispatcher_queue == 0)
        LOGE(tag, "Error creating dispatcher queue");
    executer_stat_queue = osQueueCreate(1,sizeof(int));
    if(executer_stat_queue == 0)
        LOGE(tag, "Error creating executer stat queue");
    executer_cmd_queue = osQueueCreate(1,sizeof(cmd_t *));
    if(executer_cmd_queue == 0)
        LOGE(tag, "Error creating executer cmd queue");

    int n_threads = 4;
    os_thread threads_id[n_threads];

    LOGI(tag, "Creating basic tasks...");
    /* Crating system task (the others are created inside taskInit) */
    // FIXME: This memory values seems not work on nanomind (tested 5,5,10,5)
    osCreateTask(taskWatchdog, "watchdog", 4*256, NULL, 2, &threads_id[0]);
    osCreateTask(taskDispatcher,"dispatcher", 15*256, NULL, 3, &threads_id[1]);
    osCreateTask(taskExecuter, "executer", 15*256, NULL, 4, &threads_id[2]);
    osCreateTask(taskInit, "init", 15*256, NULL, 4, &threads_id[3]);

#ifndef ESP32
    /* Start the scheduler. Should never return */
    osScheduler(threads_id, n_threads);
    return 0;
#endif

}

#ifdef FREERTOS
#ifndef NANOMIND
/**
 * Task idle handle function. Performs operations inside the idle task
 * configUSE_IDLE_HOOK must be set to 1
 */
void vApplicationIdleHook(void)
{
    //Add hook code here
}


/**
 * Task idle handle function. Performs operations inside the idle task
 * configUSE_TICK_HOOK must be set to 1
 */
void vApplicationTickHook(void)
{
#ifdef AVR32
    LED_Toggle(LED0);
#endif
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
    printf("[ERROR][-1][%s] Stack overflow!", (char *)pcTaskName);

    /* Stack overflow handle */
    while(1);
}
#endif
#endif