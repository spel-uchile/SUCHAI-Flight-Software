//
// Created by kaminari on 10-07-18.
//

#include "cmdSubSys.h"

static const char* tag = "cmdSubSys";

void cmd_subsys_init(void) {
    cmd_add("get_gps_data", get_gps_data, "", 0);
}

int get_gps_data(char *fmt, char *params, int nparams) {
    char* param="10 get_gps_data";
    cmd_t *cmd_send_gps = cmd_get_str("send_cmd");
    cmd_add_params_str(cmd_send_gps, param);
    cmd_send(cmd_send_gps);
    printf("Getting data!\n");
    return CMD_OK;
}