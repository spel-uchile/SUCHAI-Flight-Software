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
    cmd_add("send_iridium_data", send_iridium_data, "", 2);
    cmd_add("send_iridium_msg1", send_iridium_msg1, "", 0);
    cmd_add("send_iridium_msg2", send_iridium_msg2, "", 0);
}

int get_gps_data(char *fmt, char *params, int nparams) {
    char* param="5 get_gps_data";
    cmd_t *cmd_send_gps = cmd_get_str("send_rpt");
    cmd_add_params_str(cmd_send_gps, param);
    cmd_send(cmd_send_gps);
    LOGD(tag, "Getting data!");
    return CMD_OK;
}

int get_dpl_data(char *fmt, char *params, int nparams) {
    char* param="2 get_dpl_data";
    cmd_t *cmd_send_gps = cmd_get_str("send_rpt");
    cmd_add_params_str(cmd_send_gps, param);
    cmd_send(cmd_send_gps);
    LOGD(tag, "Getting data!");
    return CMD_OK;
}

int get_prs_data(char *fmt, char *params, int nparams) {
    char* param="4 get_prs_data";
    cmd_t *cmd_send_gps = cmd_get_str("send_rpt");
    cmd_add_params_str(cmd_send_gps, param);
    cmd_send(cmd_send_gps);
    LOGD(tag, "Getting data!");
    return CMD_OK;
}

int open_dpl_la(char *fmt, char *params, int nparams) {
    char* param="2 open_dpl_la";
    cmd_t *cmd_open_lal = cmd_get_str("send_rpt");
    cmd_add_params_str(cmd_open_lal, param);
    cmd_send(cmd_open_lal);
    return CMD_OK;
}

int close_dpl_la(char *fmt, char *params, int nparams) {
    char* param="2 close_dpl_la";
    cmd_t *cmd_close_lal = cmd_get_str("send_rpt");
    cmd_add_params_str(cmd_close_lal, param);
    cmd_send(cmd_close_lal);
    return CMD_OK;
}

int open_dpl_sm(char *fmt, char *params, int nparams) {
    char* param="2 open_dpl_sm";
    cmd_t *cmd_open_sm = cmd_get_str("send_rpt");
    cmd_add_params_str(cmd_open_sm, param);
    cmd_send(cmd_open_sm);
    return CMD_OK;
}

int close_dpl_sm(char *fmt, char *params, int nparams) {
    char* param="2 close_dpl_sm";
    cmd_t *cmd_close_sm = cmd_get_str("send_rpt");
    cmd_add_params_str(cmd_close_sm, param);
    cmd_send(cmd_close_sm);
    return CMD_OK;
}

int send_iridium_data(char *fmt, char *params, int nparams) {
    char* param="6 ";
    gps_data gps_data_[1];
    prs_data prs_data_[1];
    dpl_data dpl_data_[1];
    //cmd_t *cmd_send_iridium = cmd_get_str("send_rpt");
    storage_table_gps_get(DAT_GPS_TABLE, gps_data_, 1);
    storage_table_prs_get(DAT_PRS_TABLE, prs_data_, 1);
    storage_table_dpl_get(DAT_DPL_TABLE, dpl_data_, 1);

    com_data_t data;
    data.node = (uint8_t)6;
    data.frame.frame = 0;
    data.frame.type = 1;
    memset(data.frame.data.data8, 0, sizeof(data.frame.data.data8));

    char msg[COM_FRAME_MAX_LEN];
    
    sprintf(msg, "%s %0.5f %0.5f %0.3f %0.3f %0.3f %d %d %0.3f %0.3f %0.3f %d %d %s", gps_data_[0].timestamp,gps_data_[0].latitude,gps_data_[0].longitude,gps_data_[0].height,gps_data_[0].velocity_x,gps_data_[0].velocity_y,gps_data_[0].satellites_number,gps_data_[0].mode, prs_data_[0].pressure, prs_data_[0].temperature, prs_data_[0].height, dpl_data_[0].lineal_actuator, dpl_data_[0].servo_motor, "EOF");
    memcpy(data.frame.data.data8, (char *)&msg, strlen(msg));
    printf("msg=%d\n", strlen(msg));
    return com_send_data_nreply(NULL, (char *)&data, 0);
}

int send_iridium_msg1(char *fmt, char *params, int nparams) {
    char* param="6 StartingMission";
    cmd_t *cmd_msg = cmd_get_str("send_rpt");
    cmd_add_params_str(cmd_msg, param);
    cmd_send(cmd_msg);
    return CMD_OK;
}
int send_iridium_msg2(char *fmt, char *params, int nparams) {
    char* param="6 EndMission";
    cmd_t *cmd_msg = cmd_get_str("send_rpt");
    cmd_add_params_str(cmd_msg, param);
    cmd_send(cmd_msg);
    return CMD_OK;
}
