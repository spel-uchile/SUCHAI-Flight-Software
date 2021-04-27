//
// Created by gedoix on 10-01-19.
//

#include "include/taskTest.h"

#define TEST_FAILS 0

static const char* tag = "tm_io_test_task";

void taskTest(void* param)
{
    char* tag = (char*) param;

    LOGI(tag, "Started");
    LOGI(tag, "---- Telemetries Input/Output test ----");

    int test_amount = 5;

    for (int current_test = 0; current_test < test_amount; current_test++)
    {
        osDelay(5000);

        LOGI(tag, "---- Sending Telemetry %d ----", current_test+1);

        dat_status_t status;

        for (dat_status_address_t i = 0; i < dat_status_last_address; i++)
        {
            dat_set_system_var(i, (current_test+1)*100 + ((int) i));
        }

        dat_status_to_struct(&status);

        cmd_t* cmd = cmd_get_str("send_status");
        cmd_add_params_var(cmd, 1);

        cmd_send(cmd);
    }

    osDelay(5000);

    LOGI(tag, "---- Sending Exit Command ----");

    cmd_t *cmd_exit = cmd_get_str("reset");
    cmd_send(cmd_exit);
}
