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
#include "cmdFP.h"

static const char* tag = "cmdFlightPlan";

void cmd_fp_init(void)
{
    cmd_add("fp_set_cmd", fp_set, "%d %d %d %d %d %d %s %s %d %d", 10);
    cmd_add("fp_del_cmd", fp_delete, "%d %d %d %d %d %d", 6);
    cmd_add("fp_show", fp_show, "", 0);
    cmd_add("fp_reset", fp_reset,"", 0);
}

int fp_set(char *fmt, char *params, int nparams)
{
    struct tm str_time;
    time_t unixtime;
    int day, month, year, hour, min, sec;
    char command[CMD_MAX_STR_PARAMS];
    char args[CMD_MAX_STR_PARAMS];
    int executions,periodical;

    if(sscanf(params, fmt, &day, &month, &year, &hour, &min, &sec, &command, &args, &executions, &periodical) == nparams)
    {
        str_time.tm_mday = day;
        str_time.tm_mon = month-1;
        str_time.tm_year = year-1900;
        str_time.tm_hour = hour;
        str_time.tm_min = min;
        str_time.tm_sec = sec;

        unixtime = mktime(&str_time);

        printf("Tiempo cmd: %d", (int)unixtime);

        int rc = storage_flight_plan_set((int)unixtime, command, args, executions, periodical);

        if (rc == 0)
            return CMD_OK;
        else
            return CMD_FAIL;
    }
    else
    {
        LOGW(tag, "fp_set_cmd used with invalid params: %s", params);
        return CMD_FAIL;
    }
}

int fp_delete(char* fmt, char* params, int nparams)
{

    struct tm str_time;
    time_t unixtime;
    int day, month, year, hour, min, sec;

    if(sscanf(params, fmt, &day, &month, &year, &hour, &min, &sec) == nparams)
    {
        str_time.tm_mday = day;
        str_time.tm_mon = month-1;
        str_time.tm_year = year-1900;
        str_time.tm_hour = hour;
        str_time.tm_min = min;
        str_time.tm_sec = sec;

        unixtime = mktime(&str_time);

        int rc = storage_flight_plan_erase((int)unixtime);

        if(rc==0)
            return CMD_OK;
        else
            return CMD_FAIL;
    }
    else
    {
        LOGW(tag, "fp_del_cmd used with invalid params: %s", params);
        return CMD_FAIL;
    }
}

int fp_show(char* fmt, char* params, int nparams)
{

    int rc= storage_show_table();

    if(rc==0)
        return CMD_OK;
    else
        return CMD_FAIL;
}

int fp_reset(char* fmt, char* params, int nparams)
{

    int rc = storage_flight_plan_reset();

    if(rc==0)
        return CMD_OK;
    else
        return CMD_FAIL;

}

