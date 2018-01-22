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

void taskFlightPlan(void *param)
{

    LOGD(tag, "Started");

//funciona

    portTick delay_ms = 1000;          //Task period in [ms]
    portTick xLastWakeTime = osTaskGetTickCount();
#ifdef AVR32
    unsigned long test_unix_t = 1516629600;
    volatile avr32_rtc_t *rtc;
    rtc_init(&rtc,RTC_OSC_32KHZ,RTC_PSEL_32KHZ_1HZ);
    //rtc_set_top_value(&rtc, 0);
    rtc_enable_interrupt(&rtc);
    //rtc_set_value(&rtc,test_unix_t);
    rtc_enable(&rtc);
    unsigned long elapsed_sec;
#else
    time_t elapsed_sec;   // Seconds counter
#endif

    while(1)
    {
        osTaskDelayUntil(&xLastWakeTime, delay_ms); //Suspend task

#ifdef AVR32
        unsigned long elapsed_sec = rtc_get_value(&rtc);
        printf("%lu\n", test_unix_t+elapsed_sec/1000);
#else
        elapsed_sec = time(NULL);
#endif

        char* command = malloc(sizeof(char)*50);
        char* args = malloc(sizeof(char)*50);
        int* executions = malloc(sizeof(int));
        int* periodical = malloc(sizeof(int));

        int rc = dat_get_fp((int)elapsed_sec, &command, &args, &executions, &periodical);

        if(rc == -1)
            continue;

        int i;
        for(i=0; i < (*executions); i++)
        {
            cmd_t *new_cmd = cmd_get_str(command);
            cmd_add_params_str(new_cmd, args);

            LOGD(tag, "Command: %s", command);
            LOGD(tag, "Arguments: %s", args);
            LOGD(tag, "Executions: %d", *executions);
            LOGD(tag, "Periodical: %d", *periodical);
            cmd_send(new_cmd);
        }

        free(command);
        free(args);
        free(executions);
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