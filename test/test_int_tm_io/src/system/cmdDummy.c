//
// Created by lurrea on 28-10-21.
//

#include "app/system/cmdDummy.h"

static const char* tag = "test_tm_io_cmd";

void cmd_app_dummy_init(){
    cmd_add("sto_dum", store_data_dummy, "%s %hi %f", 3);
    cmd_add("cmp_dum", compare_data_dummy, "%s %hi %f %d %d", 5);
}

int store_data_dummy(char *fmt, char *params, int nparams) {
    int16_t int_arg;
    char str_arg[40];
    float float_arg;

    if(params == NULL)
        return CMD_SYNTAX_ERROR;

    if(sscanf(params, fmt, str_arg, &int_arg, &float_arg) != nparams)
        return CMD_SYNTAX_ERROR;

    char str1[128];
    snprintf(str1, 128, "%s %d", str_arg, 1);

    dummy_data_t data;
    data.index = dat_get_system_var(dat_drp_idx_dummy);
    data.timestamp = dat_get_time();
    memset(data.str1, 0, SCH_ST_STR_SIZE);
    strcpy(data.str1, str1);
    data.v1 = int_arg + 1;
    data.v2 = int_arg + 2;
    data.v3 = float_arg;
    data.v4 = int_arg + 4;
    int rc = dat_add_payload_sample(&data, dummy_sensor);
    return rc != -1 ? CMD_OK : CMD_ERROR;
}

int compare_data_dummy(char *fmt, char *params, int nparams){
    int16_t int_arg;
    char str_arg[40];
    float float_arg;
    int payload_id;
    int payload_idx;

    if(params == NULL)
        return CMD_SYNTAX_ERROR;

    if(sscanf(params, fmt, str_arg, &int_arg, &float_arg, &payload_id, &payload_idx) != nparams)
        return CMD_SYNTAX_ERROR;

    char str1[128];
    snprintf(str1, 128, "%s %d", str_arg, 1);

    dummy_data_t data;
    dat_get_payload_sample(&data, payload_id, payload_idx);

    LOGI(tag, data.str1);
    LOGI(tag, str1);

    assert(strcmp(data.str1, str1) == 0);
    assert(data.v1 == int_arg + 1);
    assert(data.v2 == int_arg + 2);
    assert(data.v3 == float_arg);
    assert(data.v4 == int_arg + 4);

    return CMD_OK;
}