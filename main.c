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
#include <signal.h>

/* OS includes */
#include "osThread.h"
#include "osScheduler.h"
#include "osQueue.h"
#include "osSemphr.h"

/* system includes */
#include "config.h"
#include "globals.h"
#include "utils.h"
#include "repoData.h"
#include "repoCommand.h"

/* Task includes */
#include "taskTest.h"
#include "taskDispatcher.h"
#include "taskExecuter.h"
#include "taskHousekeeping.h"
#include "taskConsole.h"

static void on_reset(void);
static void on_close(int signal);

int main(void)
{
    /* On reset */
    on_reset();

    /* Initializing shared Queues */
    dispatcher_queue = osQueueCreate(25,sizeof(cmd_t *));
    executer_cmd_queue = osQueueCreate(1,sizeof(cmd_t *));
    executer_stat_queue = osQueueCreate(1,sizeof(int));

    osDelay(1000);

    int n_threads = 4;
    os_thread threads_id[n_threads];

    /* Crating system task (the others are created inside taskDeployment) */
    osCreateTask(taskDispatcher,"dispatcher", 2*configMINIMAL_STACK_SIZE,NULL,3, &threads_id[0]);
    osCreateTask(taskExecuter, "executer", 5*configMINIMAL_STACK_SIZE, NULL, 4, &threads_id[1]);

#if SCH_RUN_TESTS
    osCreateTask(taskTest, "test", 2*configMINIMAL_STACK_SIZE, "TEST1", 2, &threads_id[2]);
#endif

    /* Creating monitors tasks */
    osCreateTask(taskHousekeeping, "housekeeping", 2*configMINIMAL_STACK_SIZE, NULL, 2, &threads_id[2]);
    osCreateTask(taskConsole, "console", 2*configMINIMAL_STACK_SIZE, NULL, 2, &threads_id[3]);

    /* Start the scheduler. Should never return */
    osScheduler(threads_id, n_threads);

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
    struct sigaction act;
    act.sa_handler = on_close;
    sigaction(SIGINT, &act, NULL);  // Register CTR+C signal handler
    sigaction(SIGTERM, &act, NULL);

    /* Init subsystems */
    log_init();      // Logging system
    cmd_repo_init(); //Command repository initialization
    dat_repo_init(); //Update status repository
}

/**
 * Performs a clean exit
 *
 * @param signal Int. Signal number
 */
void on_close(int signal)
{
    dat_repo_close();

    LOGI("Main", "Exit system!");
    exit(signal);
}
