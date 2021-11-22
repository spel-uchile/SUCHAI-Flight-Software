//
// Created by lurrea on 27-10-21.
//
#include "app/system/taskTest.h"

static const char* tag = "test_tm_io_task";

void taskTest(void *params){

    cmd_t *reset = cmd_build_from_str("drp_ebf 1010");
    cmd_send(reset);

    cmd_t *ping = cmd_build_from_str("com_ping 3");
    cmd_send(ping);

    int sampleNum = 1;
    float floatStored[sampleNum];
    int16_t intStored[sampleNum];
    char *stringStored = "KAJLSJALKSJLK";
    char buf[127];

    cmd_t *cmd;

    // Store dummy data in the Dummy payload
    for (int i = 0; i < sampleNum; ++i) {
        osDelay(1000);
        int16_t int_test = rand();
        float float_test = rand()/(float)rand();
        snprintf(buf, 127, "sto_dum %s %hi %f", stringStored, int_test, float_test);
        cmd = cmd_build_from_str(buf);
        cmd_send(cmd);

        intStored[i] = int_test;
        floatStored[i] = float_test;
    }

    //Send all dummy data generated to the same node
    char buf2[32];
    int payload = dummy_sensor;
    int actNode = 3;
    snprintf(buf2, 32, "tm_send_all %d %d", payload, actNode);
    cmd = cmd_build_from_str(buf2);
    cmd_send(cmd);

    //Compare all the data received with the expected results
    for(int i = 0; i < sampleNum; i++){
        snprintf(buf, 127, "cmp_dum %s %hi %f %d %d", stringStored, intStored[i], floatStored[i], payload,  i+sampleNum);
        cmd = cmd_build_from_str(buf);
        cmd_send(cmd);
    }

    LOGI(tag, "---- Finish test ----");
    osTaskDelete(NULL);
}

