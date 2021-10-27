/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2021, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2021, Camilo Rojas Milla, camrojas@uchile.cl
 *      Copyright 2021, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *      Copyright 2021, Matias Ramirez Martinez, nicoram.mt@gmail.com
 *      Copyright 2021, Tamara Gutierrez Rojo tamigr.2293@gmail.com
 *      Copyright 2021, Ignacio Ibanez Aliaga, ignacio.ibanez@usach.cl
 *      Copyright 2021, Diego Ortego Prieto, diortego@dcc.uchile.cl
 *      Copyright 2021, Elias Obreque Sepulveda, elias.obreque@uchile.cl
 *      Copyright 2021, Javier Morales Rodriguez, javiermoralesr95@gmail.com
 *      Copyright 2021, Luis Jimenez Verdugo, luis.jimenez@ing.uchile.cl
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

#include "suchai/mainFS.h"

const char *tag = "fs_main";

int suchai_main(void)
{
    /* On reset */
    on_reset();
    printf("\r\n\r\n--------- FLIGHT SOFTWARE START ---------\r\n");
    printf("\t Version: %s\r\n", SCH_SW_VERSION);
    printf("\t Device : %d (%s)\r\n", SCH_DEVICE_ID, SCH_NAME);
    printf("-----------------------------------------\r\n\r\n");

    /* Init software subsystems */
    log_init(SCH_LOG_LEVEL, 0);      // Logging system
    osSemaphoreCreate(&repo_cmd_sem);
    osSemaphoreCreate(&repo_data_sem);

    /* Initializing shared Queues */
    dispatcher_queue = osQueueCreate(25,sizeof(cmd_t *));
    executer_stat_queue = osQueueCreate(1,sizeof(int));
    executer_cmd_queue = osQueueCreate(1,sizeof(cmd_t *));

    if(dispatcher_queue == 0) LOGE(tag, "Error creating dispatcher queue");
    if(executer_stat_queue == 0) LOGE(tag, "Error creating executer stat queue");
    if(executer_cmd_queue == 0) LOGE(tag, "Error creating executer cmd queue");

    int n_threads = 4;
    os_thread threads_id[n_threads];

    LOGI(tag, "Creating basic tasks...");
    /* Crating system task (the others are created inside taskInit) */
    int t_inv_ok = osCreateTask(taskDispatcher,"invoker", SCH_TASK_DIS_STACK, NULL, 3, &threads_id[1]);
    int t_exe_ok = osCreateTask(taskExecuter, "receiver", SCH_TASK_EXE_STACK, NULL, 4, &threads_id[2]);
    int t_wdt_ok = osCreateTask(taskWatchdog, "watchdog", SCH_TASK_WDT_STACK, NULL, 2, &threads_id[0]);
    int t_ini_ok = osCreateTask(taskInit, "init", SCH_TASK_INI_STACK, NULL, 3, &threads_id[3]);

    /* Check if the task were created */
    if(t_inv_ok != 0) LOGE(tag, "Task invoker not created!");
    if(t_exe_ok != 0) LOGE(tag, "Task receiver not created!");
    if(t_wdt_ok != 0) LOGE(tag, "Task watchdog not created!");
    if(t_ini_ok != 0) LOGE(tag, "Task init not created!");

    /* Start the scheduler. Should never return */
    osScheduler(threads_id, n_threads);
    return 0;
}
