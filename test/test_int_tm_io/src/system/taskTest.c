//
// Created by lurrea on 27-10-21.
//
#include "app/system/taskTest.h"

static const char* tag = "test_tm_io_task";

void taskTest(void *params){

    //reset the db to prevent problems with the new stored data
    cmd_t *reset = cmd_build_from_str("drp_ebf 1010");
    cmd_send(reset);

    int sampleNum = 20;
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
    char buf2[64];
    int payload = dummy_sensor;
    int actNode = 3;
    snprintf(buf2, 64, "tm_send_all %d %d", payload, actNode);
    cmd = cmd_build_from_str(buf2);
    cmd_send(cmd);

    // Wait some time to receive all the data
    while(dat_get_status_var(dat_drp_idx_dummy).i < 2*sampleNum )
        sleep(1);

    //Compare all the data received with the expected results
    for(int i = 0; i < sampleNum; i++){
        snprintf(buf, 127, "cmp_dum %s %hi %f %d %d", stringStored, intStored[i], floatStored[i], payload,  i+sampleNum);
        cmd = cmd_build_from_str(buf);
        cmd_send(cmd);
    }
    // Terminate the execution of the test
    cmd = cmd_build_from_str("obc_reset");
    cmd_send(cmd);
}

