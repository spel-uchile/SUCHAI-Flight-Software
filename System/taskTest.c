#include "include/taskTest.h"

#define TEST_FAILS 1

const static char *tag = "taskTest";

void taskTest(void *param)
{
    LOGI(tag, "Started");
    LOGI(tag, "---- Testing commands interface ----")

    LOGI(tag, "Test: test_str_int from string")
    cmd_t *test_cmd = cmd_get_str("test_str_int");
    cmd_add_params_str(test_cmd, "STR1 12");
    osQueueSend(dispatcherQueue, &test_cmd, portMAX_DELAY);
    osDelay(500);

    LOGI(tag, "Test: test_double_int from vars")
    cmd_t *test_cmd2 = cmd_get_str("test_double_int");
    cmd_add_params_var(test_cmd2, 1.08, 2.09, 12, 23);
    osQueueSend(dispatcherQueue, &test_cmd2, portMAX_DELAY);
    osDelay(500);

    LOGI(tag, "Test: test_str_double_int from string")
    cmd_t *test_cmd3= cmd_get_str("test_str_double_int");
    cmd_add_params_str(test_cmd3, "STR1 12.456 STR2 13.078 456");
    osQueueSend(dispatcherQueue, &test_cmd3, portMAX_DELAY);
    osDelay(500);


#if TEST_FAILS
    LOGI(tag, "Test: test_str_int from string with bad parameters numbers")
    cmd_t *test_cmd5 = cmd_get_str("test_str_int");
    cmd_add_params_str(test_cmd5, "STR1 12 12");
    osQueueSend(dispatcherQueue, &test_cmd5, portMAX_DELAY);
    osDelay(500);

    LOGI(tag, "Test: test_str_int from string with bad parameters type")
    cmd_t * test_cmd4 = cmd_get_str("test_str_int");
    cmd_add_params_str(test_cmd4, "STR1 a12");
    osQueueSend(dispatcherQueue, &test_cmd4, portMAX_DELAY);
#endif

}
