#include "main.h"
#include "taskTest.h"

static const char* tag = "test_sgp4";

int main(void)
{
    /* On reset */
    on_reset();
    printf("\n\n--------- FLIGHT SOFTWARE START ---------\n");
    printf("\t Version: %s\n", SCH_SW_VERSION);
    printf("\t Device : %d (%s)\n", SCH_DEVICE_ID, SCH_NAME);
    printf("-----------------------------------------\n\n");

    /* Init software subsystems */
    log_init(LOG_LEVEL, 0);      // Logging system
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

    int n_threads = 3;
    os_thread threads_id[n_threads];

    LOGI(tag, "Creating basic tasks...");
    /* Crating system task (the others are created inside taskInit) */
    int t_inv_ok = osCreateTask(taskDispatcher,"invoker", SCH_TASK_DIS_STACK, NULL, 3, &threads_id[0]);
    int t_exe_ok = osCreateTask(taskExecuter, "receiver", SCH_TASK_EXE_STACK, NULL, 4, &threads_id[1]);
//    int t_ini_ok = osCreateTask(taskInit, "init", SCH_TASK_INI_STACK, NULL, 3, &threads_id[3]);
    int t_test_ok = osCreateTask(taskTest, "test", SCH_TASK_DEF_STACK, "../data.csv", 2, &threads_id[2]);

    /* Check if the task were created */
    if(t_inv_ok != 0){ LOGE(tag, "Task invoker not created!"); return 1; }
    if(t_exe_ok != 0){ LOGE(tag, "Task receiver not created!"); return 1; }
    if(t_test_ok != 0){ LOGE(tag, "Task test not created!"); return 1; }

    /* Start the scheduler. Should never return */
    osScheduler(threads_id, n_threads);
    return 1;
}
