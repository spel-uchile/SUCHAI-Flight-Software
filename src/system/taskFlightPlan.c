//
// Created by grynn on 12-12-17.
//

#include "taskFlightPlan.h"

static const char *tag = "FlightPlan";
char* table = "flightPlan";
int timeinit=60;   // ################################################################  Must change  ###################

void taskFlightPlan(void *param){

    LOGD(tag, "Started");

    portTick delay_ms = 30000;          //Task period in [ms]

    unsigned int elapsed_sec = 0;      // Seconds counter

    portTick xLastWakeTime = osTaskGetTickCount();

    storage_table_flight_plan_init_times(timeinit,table);

    const unsigned char* command;
    const unsigned char* args;
    int repeat;

    while(1)
    {
        osTaskDelayUntil(&xLastWakeTime, delay_ms); //Suspend task
        elapsed_sec += delay_ms; //Update seconds counts

        command = storage_flight_plan_get_command(elapsed_sec,table);
        args = storage_flight_plan_get_args(elapsed_sec,table);
        repeat = storage_flight_plan_get_repeat(elapsed_sec,table);

        if(command == NULL || args == NULL)
            continue;

        cmd_t *new_cmd = cmd_get_str((char *)command);
        cmd_add_params_str(new_cmd, (char *)args);

        for(int i=0; i<repeat; i++)
        {
            cmd_send(new_cmd);
        }
    }
}