//
// Created by grynn on 12-12-17.
//

#include "taskFlightPlan.h"

static const char *tag = "FlightPlan";

void taskFlighPlan (void *param){

    LOGD(tag, "Started");

    portTick delay_ms = 1000;          //Task period in [ms]

    unsigned int elapsed_sec = 0;      // Seconds counter
    unsigned int sec_check = 10;     // seconds to check actions

    char *task_name = malloc(sizeof(char)*14);
    strcpy(task_name, "FlightPlan");

    portTick xLastWakeTime = osTaskGetTickCount();

    while(1){

        osTaskDelayUntil(&xLastWakeTime, delay_ms); //Suspend task
        elapsed_sec += delay_ms/1000; //Update seconds counts

        if(elapsed_sec % sec_check == 0){

        }

    }
}