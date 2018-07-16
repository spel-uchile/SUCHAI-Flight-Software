/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2018, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2018, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *      Copyright 2018, Matias Ramirez Martinez, nicoram.mt@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "repoData.h"

static const char *tag = "repoData";
char* table = "flightPlan";
#ifdef AVR32
time_t sec = 0;
#endif



#if SCH_STORAGE_MODE == 0
    int DAT_SYSTEM_VAR_BUFF[dat_system_last_var];
    fp_entry_t data_base [SCH_FP_MAX_ENTRIES];
#endif


void dat_repo_init(void)
{
    // Init repository mutex
    if(osSemaphoreCreate(&repo_data_sem) != CSP_SEMAPHORE_OK)
    {
        LOGE(tag, "Unable to create data repository mutex");
    }

    LOGD(tag, "Initializing data repositories buffers...")
    /* TODO: Setup external memories */
#if (SCH_STORAGE_MODE == 0)
    {
        // Reset variables (we do not have persistent storage here)
        int index;
        for(index=0; index<dat_system_last_var; index++)
        {
            dat_set_system_var((dat_system_t)index, INT_MAX);
        }

        //Init internal flight plan table
        int i;
        for(i=0;i<SCH_FP_MAX_ENTRIES;i++)
        {
            data_base[i].unixtime = 0;
            data_base[i].cmd = NULL;
            data_base[i].args = NULL;
            data_base[i].executions = 0;
            data_base[i].periodical = 0;
        }
    }
#else
    {
        //Init storage system
        int rc;
        rc = storage_init(SCH_STORAGE_FILE);
        assertf(rc==0, tag, "Unable to create non-volatile data repository");

        //Init system repo
        rc = storage_table_repo_init(DAT_REPO_SYSTEM, 0);
        assertf(rc==0, tag, "Unable to create system variables repository");

        //Init system flight plan table
        rc=storage_table_flight_plan_init(0);
        assertf(rc==0, tag, "Unable to create flight plan table");

        //Mission specific initialization data
        //Init gps data table subsystem
        rc=storage_table_gps_init(DAT_GPS_TABLE, 0);
        assertf(rc==0, tag, "Unable to create gps table");

        //Init pressure data table subsystem
        rc=storage_table_pressure_init(DAT_PRS_TABLE, 0);
        assertf(rc==0, tag, "Unable to create gps table");

        //Init deploy data table subsystem
        rc=storage_table_deploy_init(DAT_DPL_TABLE, 0);
        assertf(rc==0, tag, "Unable to create gps table");
    }
#endif

    /* TODO: Initialize custom variables */
    LOGD(tag, "Initializing system variables values...")
//    dat_set_system_var(dat_obc_hrs_alive, 0);
    dat_set_system_var(dat_obc_hrs_wo_reset, 0);
    dat_set_system_var(dat_obc_reset_counter, dat_get_system_var(dat_obc_reset_counter) + 1);  //TODO: This is a non-volatile variable
    dat_set_system_var(dat_obc_sw_wdt, 0);  // Reset the gnd wdt on boot

    if(dat_get_system_var(dat_obc_hrs_alive) ==-1) {
        LOGI(tag, "setting hours alive");
        dat_set_system_var(dat_obc_hrs_alive, 0);
    }

    if(dat_get_system_var(dat_balloon_phase) ==-1) {
        LOGI(tag, "setting baloon phase");
        dat_set_system_var(dat_balloon_phase, 0);
    }

    if(dat_get_system_var(dat_balloon_deploys) ==-1) {
        LOGI(tag, "setting baloon globos");
        dat_set_system_var(dat_balloon_deploys, 2);
    }
}

void dat_repo_close(void)
{
#if SCH_STORAGE_MODE == 1
    {
        storage_close();
    }
#endif
}

void dat_set_system_var(dat_system_t index, int value)
{
    //Enter critical zone
    osSemaphoreTake(&repo_data_sem, portMAX_DELAY);

    //Uses internal memory
    #if SCH_STORAGE_MODE == 0
        DAT_SYSTEM_VAR_BUFF[index] = value;
    //Uses external memory
    #else
        storage_repo_set_value_idx(index, value, DAT_REPO_SYSTEM);
    #endif

    //Exit critical zone
    osSemaphoreGiven(&repo_data_sem);
}

int dat_get_system_var(dat_system_t index)
{
    int value = 0;

    //Enter critical zone
    osSemaphoreTake(&repo_data_sem, portMAX_DELAY);

    //Use internal (volatile) memory
    #if SCH_STORAGE_MODE == 0
        value = DAT_SYSTEM_VAR_BUFF[index];
    //Uses external (non-volatile) memory
    #else
        value = storage_repo_get_value_idx(index, DAT_REPO_SYSTEM);
    #endif

    LOGD(tag, "value obtained from id: %d, is: %d", index, value);

    //Exit critical zone
    osSemaphoreGiven(&repo_data_sem);

    return value;
}

int dat_get_fp(int elapsed_sec, char* command, char* args, int* executions, int* periodical)
{
#if SCH_STORAGE_MODE == 0
    int i;
    for(i = 0;i < SCH_FP_MAX_ENTRIES;i++)
    {
        if(elapsed_sec == data_base[i].unixtime)
        {
            strcpy(command, data_base[i].cmd);
            strcpy(args,data_base[i].args);
            *executions = data_base[i].executions;
            *periodical = data_base[i].periodical;

            if (data_base[i].periodical > 0)
                dat_set_fp(elapsed_sec+*periodical,data_base[i].cmd,data_base[i].args,*executions,*periodical);


            dat_del_fp(elapsed_sec);

            return 0;
        }
    }
    return -1;
#else
     return storage_flight_plan_get(elapsed_sec, command, args, executions, periodical);
#endif
}

int dat_set_fp(int timetodo, char* command, char* args, int executions, int periodical)
{
#if SCH_STORAGE_MODE == 0
    //TODO : agregar signal de segment para responder falla
    int i;
    for(i = 0;i < SCH_FP_MAX_ENTRIES;i++)
    {
        if(data_base[i].unixtime == 0)
        {
            data_base[i].unixtime = timetodo;
            data_base[i].executions = executions;
            data_base[i].periodical = periodical;

            data_base[i].cmd = malloc(sizeof(char)*50);
            data_base[i].args = malloc(sizeof(char)*50);

            strcpy(data_base[i].cmd, command);
            strcpy(data_base[i].args,args);

            return 0;
        }
    }
    return 1;
#else
    return storage_flight_plan_set(timetodo, command, args, executions, periodical);
#endif
}

int dat_del_fp(int timetodo)
{
#if SCH_STORAGE_MODE ==0
    int i;
    for(i = 0;i < SCH_FP_MAX_ENTRIES;i++)
    {
        if(timetodo == data_base[i].unixtime)
        {
            data_base[i].unixtime = 0;
            data_base[i].executions = 0;
            data_base[i].periodical = 0;
            free(data_base[i].args);
            free(data_base[i].cmd);
            return 0;
        }
    }
    return 1;
#else
    return storage_flight_plan_erase(timetodo);
#endif
}

int dat_reset_fp(void)
{
#if SCH_STORAGE_MODE == 0
    int i;
    for(i=0;i<SCH_FP_MAX_ENTRIES;i++)
    {
        data_base[i].unixtime = 0;
        data_base[i].cmd = NULL;
        data_base[i].args = NULL;
        data_base[i].executions = 0;
        data_base[i].periodical = 0;
    }
    return 0;
#else
    return storage_table_flight_plan_init(1);
#endif
}

int dat_show_fp (void)
{
#if SCH_STORAGE_MODE ==0
    int cont = 0;
    int i;
    for(i = 0;i < SCH_FP_MAX_ENTRIES; i++)
    {
        if(data_base[i].unixtime!=0)
        {
            if(cont == 0)
            {
                printf("When\tCommand\tArguments\tExecutions\tPeriodical\n");
                cont++;
            }
            time_t time_to_show = data_base[i].unixtime;
            printf("%s\t%s\t%s\t%d\t%d\n",ctime(&time_to_show),data_base[i].cmd,data_base[i].args,data_base[i].executions,data_base[i].periodical);
        }
    }
    if(cont == 0)
    {
        LOGI(tag, "Flight plan table empty");
    }

    return 0;
#else
    return storage_show_table();
#endif
}

time_t dat_get_time(void)
{
#ifdef AVR32
    return sec;
#else
    return time(NULL);
#endif
}

int dat_update_time(void)
{
#ifdef AVR32
    sec++;
    return 0;
#else
    return 0;
#endif
}

int dat_set_time(int new_time)
{
#ifdef AVR32
    sec = (time_t)new_time;
    return 0;
#else
    return 0;
#endif
}

int dat_show_time(int format)
{
#ifdef AVR32
    time_t time_to_show = dat_get_time();
    if(format == 0)
    {
        printf("%s\n",ctime(&time_to_show));
        return 0;
    }
    else if(format == 1)
    {
        printf("%d\n", (int)time_to_show);
        return 0;
    }
    else
    {
        return 1;
    }
#else
    time_t time_to_show = time(NULL);
    if(format == 0)
    {
        printf("%s\n",ctime(&time_to_show));
        return 0;
    }
    else if(format == 1)
    {
        printf("%d\n", (int)time_to_show);
        return 0;
    }
    else
    {
        return 1;
    }
#endif
}
