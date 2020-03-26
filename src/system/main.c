/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2020, Camilo Rojas Milla, camrojas@uchile.cl
 *      Copyright 2020, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *      Copyright 2020, Matias Ramirez Martinez, nicoram.mt@gmail.com
 *      Copyright 2020, Tamara Gutierrez Rojo tamigr.2293@gmail.com
 *      Copyright 2020, Ignacio Ibanez Aliaga, ignacio.ibanez@usach.cl
 *      Copyright 2020, Diego Ortego Prieto, diortego@dcc.uchile.cl
 *      Copyright 2020, Elias Obreque Sepulveda, elias.obreque@uchile.cl
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
#include "osDelay.h"
#include "repoData.h"
#include "pthread.h"

const char *tag = "main";

void pre_tick_hook(void);
void pos_tick_hook(void);
int run(time_t start, unsigned int seconds, int dt_s);

int main(void)
{
    /* On reset */
    on_reset();
    printf("\n\n--------- FLIGHT SOFTWARE START ---------\n");
    printf("\t Version: %s\n", SCH_SW_VERSION);
    printf("\t Device : %d (%s)\n", SCH_DEVICE_ID, SCH_NAME);
    printf("-----------------------------------------\n\n");

    /* Init software subsystems */
    log_init(LOG_LEVEL, -1);      // Logging system
    cmd_repo_init(); // Command repository initialization
    dat_repo_init(); // Update status repository

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

#ifdef SIMULATOR
    /**
     * Wait the initialization task finish, so all threads are running
     */
    pthread_join(threads_id[3], NULL);
    /**
     * Main loop, run the simulation
     * start: start time in unix epoch time (seconds)
     * seconds: total simulation time in seconds
     * dt_s: simulation time increment in seconds
     */
    int rc = run(time(NULL), 120, 1);
    exit(rc);
#else
    osScheduler(threads_id, n_threads);
    exit(0);
#endif
}

/**
 * Simulator main loop
 *  Outer loop count seconds with dt_s resolution, set date time
 *  Inner loop count milliseconds, set system tick
 * @param start Start time in unix time (seconds)
 * @param seconds Simulation total time in seconds
 * @param dt_s Outer loop resolution
 * @return 0
 */
int run(time_t start, unsigned int seconds, int dt_s)
{
    int current_s = 0;
    int current_ms = 0;
    int tick_ms = 0;
    double progress = 0.0;
    time_t current_time = start;

    for(current_s = 0; current_s < seconds; current_s+=dt_s)
    {
        progress = 100.0*current_s/seconds;
        LOGI(tag, "Progress: %0.01f%%...", progress);

        for(current_ms = 0; current_ms < 1000*dt_s; current_ms++)
        {
            pre_tick_hook();
            osTaskSetTickCount(1000*tick_ms++); // Tick in us
            pos_tick_hook();
        }

        current_time += dt_s;
        dat_set_time(current_time);
    }

    return 0;
}

/**
 * This function is called before set the system tick.
 * Incoming messages can be put into CSP queue here.s
 */
void pre_tick_hook(void)
{
    //TODO: Put messages into the FS
};

/**
 * This function is called after set the system tick.
 * CSP messages are received and processed here
 */
void pos_tick_hook(void)
{
    extern osQueue csp_if_queue_tx;  // Defined in taskInit.c
    csp_packet_t *buff;
    int rc = osQueueReceive(csp_if_queue_tx, &buff, 0);

    if(rc > 0) {
      printf("OUT: "); print_buff(buff->data, buff->length);
      csp_buffer_free(buff);
    }
};

/* FreeRTOS Hooks */
#if  defined(FREERTOS) && !defined(NANOMIND) && !defined(ESP32)
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
