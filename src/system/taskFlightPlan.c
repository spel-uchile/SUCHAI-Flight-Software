//
// Created by grynn on 12-12-17.
//

#include "taskFlightPlan.h"

static const char *tag = "FlightPlan";
char* table = "flightPlan";
int timeinit=60;   // ################################################################  Must change  ###################

void taskFlightPlan(void *param){

    LOGD(tag, "Started");

    portTick delay_ms = 2000;          //Task period in [ms]

    unsigned int elapsed_sec = 0;      // Seconds counter

    portTick xLastWakeTime = osTaskGetTickCount();

    storage_table_flight_plan_init(table,1);

    char* command;
    char* args;
    int repeat;

    storage_flight_plan_set(4000,"ping","5",1,"flightPlan");
    storage_flight_plan_set(6000,"get_mem","",1,"flightPlan");
    storage_flight_plan_set(8000,"help","",1,"flightPlan");
    LOGD(tag, "insertions ready");
    storage_flight_plan_erase(4000,"flightPlan");
    LOGD(tag, "Delet ready");

    while(1)
    {
        osTaskDelayUntil(&xLastWakeTime, delay_ms); //Suspend task
        elapsed_sec += delay_ms; //Update seconds counts

        command=storage_flight_plan_get_command(elapsed_sec,table);
        args=storage_flight_plan_get_args(elapsed_sec,table);
        repeat = storage_flight_plan_get_repeat(elapsed_sec,table);
        LOGD(tag, "Tiempo: %d", elapsed_sec);
        LOGD(tag, "Comando: %s", command);
        LOGD(tag, "Argumentos: %s", args);
        LOGD(tag, "Repeticiones: %d", repeat);

        if(command == NULL || args == NULL)
            continue;

        cmd_t *new_cmd = cmd_get_str(command);
        cmd_add_params_str(new_cmd, args);

        for(int i=0; i<repeat; i++)
        {
            cmd_send(new_cmd);
            LOGD(tag, "Ha enviado comando");
        }
        free(command);
        free(args);
    }
}