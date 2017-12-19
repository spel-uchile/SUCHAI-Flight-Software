//
// Created by grynn on 12-12-17.
//

#include "taskFlightPlan.h"

static const char *tag = "FlightPlan";
char* table = "flightPlan";

void taskFlightPlan(void *param)
{

    LOGD(tag, "Started");

    portTick delay_ms = 1000;          //Task period in [ms]

    time_t elapsed_sec;   // Seconds counter

    portTick xLastWakeTime = osTaskGetTickCount();

    storage_table_flight_plan_init(table,1);

    //storage_flight_plan_set(date_to_unixtime(19,12,2017,22,52,0),"ping","5",1,"flightPlan");
    //storage_flight_plan_set(6000,"get_mem","",1,"flightPlan");
    //storage_flight_plan_set(8000,"help","",1,"flightPlan");
    //storage_flight_plan_set(15000,"reset","",1,"flightPlan");
    LOGD(tag, "insertions ready");
    storage_show_table(table);
    LOGD(tag, "showed table");

    while(1)
    {
        osTaskDelayUntil(&xLastWakeTime, delay_ms); //Suspend task

        elapsed_sec = time(NULL);


        char* command = malloc(sizeof(char)*50);
        char* args = malloc(sizeof(char)*50);
        int* repeat = malloc(sizeof(int));

        storage_flight_plan_get((int)elapsed_sec, &command, &args, &repeat, table);
        //LOGD(tag, "Tiempo: %d", (int)elapsed_sec);
        //LOGD(tag, "Comando: %s", command);
        //LOGD(tag, "Argumentos: %s", args);
        //LOGD(tag, "Repeticiones: %d", *repeat);

        if(command == NULL)
            continue;

        cmd_t *new_cmd = cmd_get_str(command);
        cmd_add_params_str(new_cmd, args);

        for(int i=0; i<*repeat; i++)
        {
            cmd_send(new_cmd);
            LOGD(tag, "Ha enviado comando");
        }

        free(command);
        free(args);
        free(repeat);
    }
}

int date_to_unixtime(int day, int month, int year, int hour, int min, int sec)
{
    struct tm str_time;
    time_t unixtime;

    str_time.tm_mday = day;
    str_time.tm_mon = month-1;
    str_time.tm_year = year-1900;
    str_time.tm_hour = hour;
    str_time.tm_min = min;
    str_time.tm_sec = sec;

    unixtime = mktime(&str_time);

    return (int)unixtime;

}