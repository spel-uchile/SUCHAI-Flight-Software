//
// Created by kaminari on 10-07-18.
//

#include "cmdSubSys.h"

static const char* tag = "cmdSubSys";

void cmd_subsys_init(void) {
    cmd_add("get_gps_data", get_gps_data, "", 0);
    cmd_add("get_dpl_data", get_dpl_data, "", 0);
    cmd_add("get_prs_data", get_prs_data, "", 0);
    cmd_add("open_dpl_la", open_dpl_la, "", 0);
    cmd_add("close_dpl_la", close_dpl_la, "", 0);
    cmd_add("open_dpl_sm", open_dpl_sm, "", 0);
    cmd_add("close_dpl_sm", close_dpl_sm, "", 0);
}

int get_gps_data(char *fmt, char *params, int nparams) {
    char* param="5 get_gps_data";
    cmd_t *cmd_send_gps = cmd_get_str("send_rpt");
    cmd_add_params_str(cmd_send_gps, param);
    cmd_send(cmd_send_gps);
    printf("Getting data!\n");
    return CMD_OK;
}

int get_dpl_data(char *fmt, char *params, int nparams) {
    char* param="2 get_dpl_data";
    cmd_t *cmd_send_gps = cmd_get_str("send_rpt");
    cmd_add_params_str(cmd_send_gps, param);
    cmd_send(cmd_send_gps);
    printf("Getting data!\n");
    return CMD_OK;
}

int get_prs_data(char *fmt, char *params, int nparams) {
    char* param="4 get_prs_data";
    cmd_t *cmd_send_gps = cmd_get_str("send_rpt");
    cmd_add_params_str(cmd_send_gps, param);
    cmd_send(cmd_send_gps);
    printf("Getting data!\n");
    return CMD_OK;
}

int open_dpl_la(char *fmt, char *params, int nparams) {
    char* param="2 open_dpl_la";
    cmd_t *cmd_open_lal = cmd_get_str("send_cmd");
    cmd_add_params_str(cmd_open_lal, param);
    cmd_send(cmd_open_lal);
    return CMD_OK;
}

int close_dpl_la(char *fmt, char *params, int nparams) {
    char* param="2 close_dpl_la";
    cmd_t *cmd_close_lal = cmd_get_str("send_cmd");
    cmd_add_params_str(cmd_close_lal, param);
    cmd_send(cmd_close_lal);
    return CMD_OK;
}

int open_dpl_sm(char *fmt, char *params, int nparams) {
    char* param="2 open_dpl_sm";
    cmd_t *cmd_open_sm = cmd_get_str("send_cmd");
    cmd_add_params_str(cmd_open_sm, param);
    cmd_send(cmd_open_sm);
    return CMD_OK;
}

int close_dpl_sm(char *fmt, char *params, int nparams) {
    char* param="2 close_dpl_sm";
    cmd_t *cmd_close_sm = cmd_get_str("send_cmd");
    cmd_add_params_str(cmd_close_sm, param);
    cmd_send(cmd_close_sm);
    return CMD_OK;
}