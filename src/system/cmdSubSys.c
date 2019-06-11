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
    char* param="15 get_gps_data";
    com_send_rpt("%d %s", param, 2);
    LOGD(tag, "Getting data!");
    return CMD_OK;
}

int get_dpl_data(char *fmt, char *params, int nparams) {
    char* param="12 get_dpl_data";
    com_send_rpt("%d %s", param, 2);
    LOGD(tag, "Getting data!");
    return CMD_OK;
}

int get_prs_data(char *fmt, char *params, int nparams) {
    char* param="14 get_prs_data";
    com_send_rpt("%d %s", param, 2);
    LOGD(tag, "Getting data!");
    return CMD_OK;
}

int open_dpl_la(char *fmt, char *params, int nparams) {
    char* param="12 open_dpl_la";
    com_send_rpt("%d %s", param, 2);
    return CMD_OK;
}

int close_dpl_la(char *fmt, char *params, int nparams) {
    char* param="12 close_dpl_la";
    com_send_rpt("%d %s", param, 2);
    return CMD_OK;
}

int open_dpl_sm(char *fmt, char *params, int nparams) {
    char* param="12 open_dpl_sm";
    com_send_rpt("%d %s", param, 2);
    return CMD_OK;
}

int close_dpl_sm(char *fmt, char *params, int nparams) {
    char* param="12 close_dpl_sm";
    com_send_rpt("%d %s", param, 2);
    return CMD_OK;
}

int send_iridium_data(char *fmt, char *params, int nparams) {
    char* param="6 ";
    gps_data_t gps_data_;
    prs_data_t prs_data_;
    dpl_data_t dpl_data_;

    //cmd_t *cmd_send_iridium = cmd_get_str("send_rpt");
    dat_get_recent_payload_sample(&gps_data_, gps_sensors, 1);
    dat_get_recent_payload_sample(&prs_data_, prs_sensors, 1);
    dat_get_recent_payload_sample(&prs_data_, dpl_sensors, 1);
    LOGI(tag, "Obtaining gps_data, time:%u,  lat:%f,  lon:%f, alt:%f, vel_x:%f, vel_y:%f, sat_num:%d, mode:%d", gps_data_.timestamp ,gps_data_.latitude, gps_data_.longitude, gps_data_.height, gps_data_.velocity_x, gps_data_.velocity_y, gps_data_.satellites_number, gps_data_.mode);
    LOGI(tag, "Obtaining prs_data, time:%u,  prs:%f,  temp:%f, alt:%f", prs_data_.timestamp ,prs_data_.pressure, prs_data_.temperature, prs_data_.height)
    LOGI(tag, "Obtaining dpl_data, time:%u,  lin_act:%d,  serv_mot:%d", dpl_data_.timestamp ,dpl_data_.lineal_actuator, dpl_data_.servo_motor);

//    com_data_t data;
//    data.node = (uint8_t)6;
////    data.frame.frame = 0;
//    data.frame.type = 1;
//    memset(data.frame.data.data8, 0, sizeof(data.frame.data.data8));
//
//    char msg[COM_FRAME_MAX_LEN];
//
//    sprintf(msg, "%u %0.5f %0.5f %0.3f %0.3f %0.3f %d %d %0.3f %0.3f %0.3f %d %d %s", gps_data_[0].timestamp,gps_data_[0].latitude,gps_data_[0].longitude,gps_data_[0].height,gps_data_[0].velocity_x,gps_data_[0].velocity_y,gps_data_[0].satellites_number,gps_data_[0].mode, prs_data_[0].pressure, prs_data_[0].temperature, prs_data_[0].height, dpl_data_[0].lineal_actuator, dpl_data_[0].servo_motor, "EOF");
//    memcpy(data.frame.data.data8, (char *)&msg, strlen(msg));
//    printf("msg=%lu\n", strlen(msg));
//    return com_send_data(NULL, (char *)&data, 0);
}

int send_iridium_msg1(char *fmt, char *params, int nparams) {
    char* param="6 StartingMission";
    com_send_rpt("%d %s", param, 2);
    return CMD_OK;
}

int send_iridium_msg2(char *fmt, char *params, int nparams) {
    char* param="6 EndMission";
    com_send_rpt("%d %s", param, 2);
    return CMD_OK;
}
