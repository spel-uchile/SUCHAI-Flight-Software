#include "include/taskTest.h"

#define TEST_FAILS 0

//const static char *tag = "taskTest";

void taskTest(void *param)
{
    char *tag = (char *)param;
    LOGI(tag, "Started");
    LOGI(tag, "---- Testing commands interface ----");

    int test, curr;
    int n_test = 35;
    char params[10];

    for(test = 0; test < n_test; test++)
    {
        osDelay(1000);
        LOGI(tag, "-- Sending %d commands --", test);
        for (curr = 0; curr < test; curr++)
        {
            cmd_t *cmd = cmd_get_str("test");
            sprintf(params, "%d", curr);
            cmd_add_params_str(cmd, params);
            cmd_send(cmd);
        }
    }

    osDelay(2000);
    LOGI(tag, "---- Test Finished ----");
}
