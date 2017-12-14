//
// Created by grynn on 12-12-17.
//

#include "taskFlightPlan.h"

static const char *tag = "FlightPlan";
char* table = "flight-plan";
int timeinit=0;   // ################################################################  Must change  ###################

void taskFlighPlan (void *param){

    LOGD(tag, "Started");

    portTick delay_ms = 10000;          //Task period in [ms]

    unsigned int elapsed_sec = 0;      // Seconds counter

    char *task_name = malloc(sizeof(char)*14);
    strcpy(task_name, "FlightPlan");

    portTick xLastWakeTime = osTaskGetTickCount();

    storage_table_flight_plan_init_times(timeinit,table);

    const unsigned char* command;
    const unsigned char* args;
    int repeat;

    while(1){

        osTaskDelayUntil(&xLastWakeTime, delay_ms); //Suspend task
        elapsed_sec += delay_ms; //Update seconds counts

        command = storage_flight_plan_get_command(elapsed_sec,table);
        args = storage_flight_plan_get_args(elapsed_sec,table);
        repeat = storage_flight_plan_get_repeat(elapsed_sec,table);

        for(int i=0; i<repeat; i++)
        {
            //ejecutar el commando     
        }

    }
}