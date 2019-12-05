#include "include/taskTest.h"

#define TEST_FAILS 0

//const static char *tag = "taskTest";

void taskTest(void *param)
{
    char *tag = (char *)param;
    LOGI(tag, "Started");
    LOGI(tag, "---- Commands load test ----");

    int test, curr;
    int n_start = 5;
    int n_end = 25;
    char params[10];

    for(test = n_start; test < n_end; test++)
    {
        osDelay(100);
        LOGI(tag, "-- Sending %d commands --", test);
        for (curr = 0; curr < test; curr++)
        {
            cmd_t *cmd = cmd_get_str("test");
            sprintf(params, "%d", curr);
            cmd_add_params_str(cmd, params);
            cmd_send(cmd);
        }
    }

    LOGI(tag, "----Sending final commands ----");
    cmd_t *cmd_log = cmd_get_str("test");
    cmd_add_params_str(cmd_log, "----Test-Finished----");
    cmd_send(cmd_log);

    cmd_t *cmd_exit = cmd_get_str("obc_reset");
    cmd_send(cmd_exit);
}
