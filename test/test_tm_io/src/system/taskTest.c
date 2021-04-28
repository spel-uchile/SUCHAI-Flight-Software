//
// Created by gedoix on 10-01-19.
//

#include "include/taskTest.h"

#define TEST_FAILS 0

static const char* tag = "tm_io_test_task";

void taskTest(void* param)
{
    LOGI(tag, "Started");
    LOGI(tag, "---- Telemetries Input/Output test ----");

    int test_amount = 5;

    cmd_t *drp_ebf = cmd_get_str("drp_ebf");
    cmd_add_params_var(drp_ebf, 1010);
    cmd_send(drp_ebf);

    for(int current_test = 0; current_test < test_amount; current_test++)
    {
        osDelay(1000);

        LOGI(tag, "---- Sending Telemetry %d ----", current_test+1);

        for (dat_status_address_t i = 0; i < dat_status_last_address; i++)
        {
            value32_t value;
            int test_value = (current_test+1)*100 + (int)i;
            value.i = test_value;
            if(dat_get_status_var_def(i).type == 'f')
                value.f = (float)test_value;
            dat_set_status_var(i, value);
        }

        cmd_t* cmd = cmd_get_str("tm_send_status");
        cmd_add_params_var(cmd, 1);

        cmd_send(cmd);
    }

    osDelay(1000);

    LOGI(tag, "---- Sending Exit Command ----");

    cmd_t *cmd_exit = cmd_get_str("obc_reset");
    cmd_send(cmd_exit);
}
