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


/* TODO: Move this doc to the header */
/**
 * This function registers the list of command in the system, initializing the
 * functions array. This function must be called at every system start up.
 */
void cmd_fp_init(void)
{
    cmd_add("fp_set_cmd", fp_set, "%d %d %d %d %d %d %s %s %d %s %d", 11);
    cmd_add("fp_del_cmd", fp_delete, "%d %d %d %d %d %d %s", 7);
    cmd_add("fp_show", fp_show, "%s", 1);
    cmd_add("fp_reset", fp_reset,"%s", 1);
}

int fp_set(char *fmt, char *params, int nparams)
{
    struct tm str_time;
    time_t unixtime;
    int day, month, year, hour, min, sec;
    char command[CMD_MAX_STR_PARAMS];
    char args[CMD_MAX_STR_PARAMS];
    int repeat,periodical;
    char table[CMD_MAX_STR_PARAMS];

    if(sscanf(params, fmt, &day, &month, &year, &hour, &min, &sec, &command, &args, &repeat, &table, &periodical) == nparams)
    {
        str_time.tm_mday = day;
        str_time.tm_mon = month-1;
        str_time.tm_year = year-1900;
        str_time.tm_hour = hour;
        str_time.tm_min = min;
        str_time.tm_sec = sec;

        unixtime = mktime(&str_time);

        printf("Tiempo cmd: %d", (int)unixtime);

        int rc = storage_flight_plan_set((int)unixtime, command, args, repeat, table, periodical);

        if (rc == 0)
            return CMD_OK;
        else if (rc == -1)
            return CMD_FAIL;
    }
}

int fp_delete(char* fmt, char* params, int nparams)
{

    struct tm str_time;
    time_t unixtime;
    int day, month, year, hour, min, sec;
    char table[CMD_MAX_STR_PARAMS];

    if(sscanf(params, fmt, &day, &month, &year, &hour, &min, &sec, &table) == nparams)
    {
        str_time.tm_mday = day;
        str_time.tm_mon = month-1;
        str_time.tm_year = year-1900;
        str_time.tm_hour = hour;
        str_time.tm_min = min;
        str_time.tm_sec = sec;

        unixtime = mktime(&str_time);

        int rc = storage_flight_plan_erase((int)unixtime, table);

        if(rc==0)
            return CMD_OK;
        else if(rc == -1)
            return CMD_FAIL;
        else
            return CMD_ERROR;
    }
}

int fp_show(char* fmt, char* params, int nparams)
{
    char table[CMD_MAX_STR_PARAMS];

    if(sscanf(params, fmt, &table) == nparams)
    {
        int rc= storage_show_table(table);

        if(rc==0)
            return CMD_OK;
        else if(rc == -1)
            return CMD_FAIL;
        else
            return CMD_ERROR;
    }
}

int fp_reset(char* fmt, char* params, int nparams)
{
    char table[CMD_MAX_STR_PARAMS];

    if(sscanf(params, fmt, &table) == nparams)
    {
        int rc = storage_flight_plan_reset(table);

        if(rc==0)
            return CMD_OK;
        else if(rc == -1)
            return CMD_FAIL;
        else
            return CMD_ERROR;
    }
}

