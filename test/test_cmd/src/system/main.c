/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2020, Camilo Rojas Milla, camrojas@uchile.cl
 *      Copyright 2020, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *      Copyright 2020, Matias Ramirez Martinez, nicoram.mt@gmail.com
 *      Copyright 2020, Tamara Gutierrez Rojo TGR_93@hotmail.com
 *      Copyright 2020, Ignacio Ibanez Aliaga, ignacio.ibanez@usach.cl
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

int main(void)
{

    /* On reset */
    on_reset();

    /* Init software subsystems */
    log_init(LOG_LEVEL, 0);      // Logging system
    cmd_repo_init(); // Command repository initialization
    dat_repo_init(); // Update status repository

    LOGI(tag, "Creating tasks...");

    /* Initializing shared Queues */
    dispatcher_queue = osQueueCreate(25,sizeof(cmd_t *));
    executer_cmd_queue = osQueueCreate(1,sizeof(cmd_t *));
    executer_stat_queue = osQueueCreate(1,sizeof(int));

    int n_threads = 3;
    os_thread threads_id[n_threads];

    /* Crating system task (the others are created inside taskDeployment) */
    osCreateTask(taskDispatcher,"dispatcher", 2*configMINIMAL_STACK_SIZE,NULL,3, &threads_id[0]);
    osCreateTask(taskExecuter, "executer", 5*configMINIMAL_STACK_SIZE, NULL, 4, &threads_id[1]);

    osCreateTask(taskTest, "test", 2*configMINIMAL_STACK_SIZE, "TEST1", 2, &threads_id[2]);


#ifndef ESP32
    /* Start the scheduler. Should never return */
    osScheduler(threads_id, n_threads);
    return 0;
#endif

}
