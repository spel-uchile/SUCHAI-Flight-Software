#include "include/taskTest.h"

//extern os_queue xQueue1;

void taskTest(void *param)
{
    printf(">>[TASK TEST] Started\r\n");

    portTick delay_ms    = 1000;    //Task period in [ms]
    portTick delay_ticks = osDefineTime(delay_ms); //Task period in ticks

    unsigned int elapsed_sec = 0;       // Seconds count
    unsigned int _1sec_check = 1;//10;     //10[s] condition
    unsigned int _2sec_check = 2;//10*60;  //10[m] condition
    unsigned int _3sec_check = 3;//60*60;  //1[h] condition

    char *task_name = param != NULL ? (char *)param : "HSK";

    portTick xLastWakeTime = osTaskGetTickCount();

    while(1)
    {

        osTaskDelayUntil(&xLastWakeTime, delay_ticks); //Suspend task
        elapsed_sec += delay_ms/1000; //Update seconds counts

        /* 1 seconds actions */
        if((elapsed_sec % _1sec_check) == 0)
        {
            printf("[Test] _1sec_check\n");
            cmd_t *cmd_1s = cmd_get_str("cmd1");

            cmd_1s->params = (char *)malloc(sizeof(char)*25);
            strcpy(cmd_1s->params, "SEC-CMD1 12");
            osQueueSend(dispatcherQueue, &cmd_1s, portMAX_DELAY);
        }

        /* 2 seconds actions */
        if((elapsed_sec % _2sec_check) == 0)
        {
            printf("[Test] _2sec_check\n");
            cmd_t *cmd_2s = cmd_get_str("cmd2");

            cmd_2s->params = (char *)malloc(sizeof(char)*25);
            strcpy(cmd_2s->params, "1.08 2.09 12 23");
            osQueueSend(dispatcherQueue, &cmd_2s, portMAX_DELAY);
        }

        /* 3 seconds actions */
        if((elapsed_sec % _3sec_check) == 0)
        {
            printf("[Test] _3sec_check\n");
            cmd_t *cmd_3s = cmd_get_str("cmd3");

            cmd_3s->params = (char *)malloc(sizeof(char)*25);
            strcpy(cmd_3s->params, "SEC-CMD3 HOLA SEC-CMD3 13.078 456");
            osQueueSend(dispatcherQueue, &cmd_3s, portMAX_DELAY);
        }
    }

}
