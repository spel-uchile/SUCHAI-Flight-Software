#include <iostream>
#include <cstdio>
#include "sim_main.h"

extern "C"
{
#include "init.h"
#include "os/os.h"
#include "osThread.h"
#include "osScheduler.h"
#include "taskDispatcher.h"
#include "taskExecuter.h"
#include "taskInit.h"
}

static const char *tag = "OBC";

SuchaiFS::SuchaiFS()
{
    Initialize();
}

void SuchaiFS::Initialize() {

    /* On reset */
    on_reset();
    //TODO: User simulator logger
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

    int n_threads = 3;
    os_thread threads_id[n_threads];

    LOGI(tag, "Creating basic tasks...");
    /* Crating system task (the others are created inside taskInit) */
    int t_inv_ok = osCreateTask(taskDispatcher,"invoker", SCH_TASK_DIS_STACK, NULL, 3, &threads_id[0]);
    int t_exe_ok = osCreateTask(taskExecuter, "receiver", SCH_TASK_EXE_STACK, NULL, 4, &threads_id[1]);
    int t_ini_ok = osCreateTask(taskInit, "init", SCH_TASK_INI_STACK, NULL, 3, &threads_id[2]);

    /* Check if the task were created */
    if(t_inv_ok != 0) LOGE(tag, "Task invoker not created!");
    if(t_exe_ok != 0) LOGE(tag, "Task receiver not created!");
    if(t_ini_ok != 0) LOGE(tag, "Task init not created!");

    //Wait until init task finish
    pthread_join(threads_id[2], NULL);
}

/**
 * Main loop, run the simulation
 */
void SuchaiFS::MainRoutine(int count) {
    PreTickHook();
    osTaskSetTickCount(count*1000);
    PosTickHook();
}

/**
 * This method is called before set the system tick.
 * Incoming messages can be put into CSP queue here.s
 */
void SuchaiFS::PreTickHook() {
//    extern csp_iface_t csp_if_queue; // Defined in taskInit.c
//    csp_packet_t *packet = (csp_packet_t *)csp_buffer_get(SCH_BUFF_MAX_LEN);
//    if(packet == NULL) {
//        LOGE(tag, "PreTickHook: cannot allocate buffer!");
//        return;
//    }
//
//    int ret = fgets(packet->data, SCH_BUFF_MAX_LEN, stdin);
//    if(ret > 0) {
//        printf("OUT: "); print_buff(packet->data, packet->length);
//        csp_qfifo_write(packet, &csp_if_queue, NULL);
//    } else{
//        csp_buffer_free(packet);
//    }
}

/**
 * This method is called after set the system tick.
 * CSP messages are received and processed here
 */
void SuchaiFS::PosTickHook() {
  extern osQueue csp_if_queue_tx;
  csp_packet_t *buff;
  int rc = osQueueReceive(csp_if_queue_tx, &buff, 0);

  if(rc > 0) {
    printf("OUT: "); print_buff(buff->data, buff->length);
    csp_buffer_free(buff);
  }
}

/**
 * Set Flight Software current date time in unix epoch.
 * @param time
 */
void SuchaiFS::SetTime(time_t time) {
    dat_set_time((int)time);
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
int main()
{
    // Simulation parameters (can be read from argv)
    time_t start = time(NULL);
    int total_s = 3600;
    int dt_s = 1;

    // Simulation runtime variables
    int current_s = 0;
    int current_ms = 0;
    int tick_ms = 0;
    double progress = 0.0;
    time_t current_time = start;

    // Simulated Flight Software (FS)
    SuchaiFS FS = SuchaiFS();

    // Run simulation
    for(current_s = 0; current_s < total_s; current_s+=dt_s)
    {
        progress = 100.0*current_s/total_s;
        LOGI(tag, "Progress: %0.01f%%...", progress);

        for(current_ms = 0; current_ms < 1000*dt_s; current_ms++)
        {
            FS.MainRoutine(tick_ms++);
        }

        current_time += dt_s;
        FS.SetTime(current_time);
    }

    return 0;
}