#include "taskTest.h"

#define TEST_FAILS 0

const static char *tag = "taskTest";

void taskTest(void *param)
{
    LOGI(tag, "Started");
    LOGI(tag, "---- Testing commands interface ----")

    LOGI(tag, "Test: test_str_int from string")
    cmd_t *test_cmd = cmd_get_str("test_str_int");
    cmd_add_params_str(test_cmd, "STR1 12");
    osQueueSend(dispatcher_queue, &test_cmd, portMAX_DELAY);
    osDelay(500);

    LOGI(tag, "Test: test_double_int from vars")
    cmd_t *test_cmd2 = cmd_get_str("test_double_int");
    cmd_add_params_var(test_cmd2, 1.08, 2.09, 12, 23);
    osQueueSend(dispatcher_queue, &test_cmd2, portMAX_DELAY);
    osDelay(500);

    LOGI(tag, "Test: test_str_double_int from string")
    cmd_t *test_cmd3= cmd_get_str("test_str_double_int");
    cmd_add_params_str(test_cmd3, "STR1 12.456 STR2 13.078 456");
    osQueueSend(dispatcher_queue, &test_cmd3, portMAX_DELAY);
    osDelay(500);

#if TEST_FAILS
    LOGI(tag, "Test: test_str_int from string with bad parameters numbers")
    cmd_t *test_cmd5 = cmd_get_str("test_str_int");
    cmd_add_params_str(test_cmd5, "STR1 12 12");
    osQueueSend(dispatcher_queue, &test_cmd5, portMAX_DELAY);
    osDelay(500);

    LOGI(tag, "Test: test_str_int from string with bad parameters type")
    cmd_t * test_cmd4 = cmd_get_str("test_str_int");
    cmd_add_params_str(test_cmd4, "STR1 a12");
    osQueueSend(dispatcher_queue, &test_cmd4, portMAX_DELAY);
#endif

    LOGI(tag, "---- Testing DRP commands ----");
    LOGI(tag, "Test: print_vars");
    test_cmd = cmd_get_str("print_vars");
    cmd_add_params_str(test_cmd, "");
    cmd_send(test_cmd);
    osDelay(500);

    LOGI(tag, "Test: update_sys_var");
    test_cmd = cmd_get_str("update_sys_var");
    cmd_add_params_var(test_cmd, dat_obc_opmode, 123);
    cmd_send(test_cmd);
    osDelay(500);
    // Check if the command worked
    int op_mode = dat_get_system_var(dat_obc_opmode);
    assertf(op_mode == 123, tag, "Failed to set variable, read: %d", op_mode);

    LOGI(tag, "Test: update_hours_alive");
    test_cmd = cmd_get_str("update_hours_alive");
    cmd_add_params_var(test_cmd, 123);
    cmd_send(test_cmd);
    osDelay(500);
    // Check if the command worked
    int hours_alive = dat_get_system_var(dat_obc_hours_alive);
    assertf(hours_alive == 123, tag, "Failed to set variable, read: %d", hours_alive);

    LOGI(tag, "Test: print_vars");
    test_cmd = cmd_get_str("print_vars");
    cmd_add_params_str(test_cmd, "");
    cmd_send(test_cmd);
    osDelay(500);

    LOGI(tag, "---- Testing OBC commands ----");
    LOGI(tag, "Test: get_mem");
    test_cmd = cmd_get_str("get_mem");
    cmd_add_params_str(test_cmd, "");
    cmd_send(test_cmd);
    osDelay(500);

    //LAST COMMAND - EXIT OR RESET THE SYSTEM
    LOGI(tag, "Test: reset");
    test_cmd = cmd_get_str("reset");
    cmd_add_params_str(test_cmd, "");
    cmd_send(test_cmd);
}
