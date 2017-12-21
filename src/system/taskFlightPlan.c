/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2017, Matias Ramirez Martinez, nicoram.mt@gmail.com
 *      Copyright 2017, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "taskFlightPlan.h"

static const char *tag = "FlightPlan";
char* table = "flightPlan";

void taskFlightPlan(void *param)
{

    LOGD(tag, "Started");

    portTick delay_ms = 1000;          //Task period in [ms]
    time_t elapsed_sec;   // Seconds counter
    portTick xLastWakeTime = osTaskGetTickCount();

    /* FIXME: Move this to data repository dat_repo_init */
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

        /* FIXME: Memory leak detected */
        if(command == NULL)
            continue;

        cmd_t *new_cmd = cmd_get_str(command);
        cmd_add_params_str(new_cmd, args);

        for(int i=0; i< (*repeat); i++)
        {
            LOGD(tag, "Command: %s", command);
            LOGD(tag, "Arguments: %s", args);
            LOGD(tag, "Repetitions: %d", *repeat);
            LOGD(tag, "Periodical: %d", *periodical);
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