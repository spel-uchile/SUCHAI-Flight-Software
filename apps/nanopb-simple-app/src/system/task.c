//
// Created by franco on 17-11-21.
//
#include "app/system/task.h"

static const char *tag = "nanopb-simple-task";

void task(void *param)
{
    LOGI(tag, "STARTED");
    cmd_t *cmd = cmd_build_from_str("set_simple_data 13");
    cmd_send(cmd);
    cmt_t *cmd2 = cmd_build_from_str("get_simple_data");
    cmd_send(cmd2);
    osTaskDelete(NULL);
}
