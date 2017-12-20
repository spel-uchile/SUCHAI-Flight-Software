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

    storage_table_flight_plan_init(table,1);    //############################################################ alguien debe iniciar esto que no se taskFlightPlan  ######################

    //storage_flight_plan_set(date_to_unixtime(19,12,2017,22,52,0),"ping","5",1,"flightPlan",0);
    //storage_flight_plan_set(date_to_unixtime(20,12,2017,23,52,0),"ping","5",1,"flightPlan",0);
    //storage_flight_plan_set(date_to_unixtime(21,12,2017,19,55,0),"ping","5",1,"flightPlan",0);


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
        int* periodical = malloc(sizeof(int));

        dat_get_fp((int)elapsed_sec, &command, &args, &repeat, table, &periodical);

        if(command == NULL)
            continue;

        cmd_t *new_cmd = cmd_get_str(command);
        cmd_add_params_str(new_cmd, args);

        for(int i=0; i<*repeat; i++)
        {
            LOGD(tag, "Comando: %s", command);
            LOGD(tag, "Argumentos: %s", args);
            LOGD(tag, "Repeticiones: %d", *repeat);
            LOGD(tag, "Periodico: %d", *periodical);
            cmd_send(new_cmd);

        }

        free(command);
        free(args);
        free(repeat);
        free(periodical);
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