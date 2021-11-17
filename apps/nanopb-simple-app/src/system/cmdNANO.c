//
// Created by franco on 17-11-21.
//
#include <stdio.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "simple.pb.h"
#include "../../utils/include/newRepoData.h"

#include "app/system/cmdNANO.h"

static const char* tag = "cmdNANO";

/// Commands set up
int cmd_app_init(void){
    cmd_add("get_simple_data", get_simple_data, "", 0);
    cmd_add("set_simple_data", set_simple_data, "%d", 1);
}

/// Commands functions
int set_simple_data(char *fmt, char *params, int nparams)
{
#ifdef LINUX
    int32_t data;
    if(sscanf(params, fmt, &data) != 1){
        return CMD_ERROR;
    }
    simple_message message = simple_message_init_zero;
    message.data = data;
    int rc = set_simple_message(&message);
    if(rc == SCH_ST_OK){
        LOGI(tag, "simple data saved");
        return CMD_OK;
        }
    else {
        LOGE(tag, "simple data could not be saved");
        return CMD_ERROR;
    }

#endif
}

int get_simple_data(char *fmt, char *params, int nparams){
#ifdef LINUX
    simple_message message;
    int rc = get_simple_message(&message);

    if(rc == SCH_ST_OK){
        LOGI(tag, "simple data: %d", message.data);
        return CMD_OK;
    }else{
        LOGE(tag, "simple data could not be read");
        return CMD_ERROR;
    }
#endif
}

