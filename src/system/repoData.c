/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2017, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2017, Tomas Opazo Toro, tomas.opazo.t@gmail.com
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



#if SCH_STATUS_REPO_MODE == 0
    int DAT_SYSTEM_VAR_BUFF[dat_system_last_var];
#endif


void dat_repo_init(void)
{

    int rc;

    // Init repository mutex
    if(osSemaphoreCreate(&repo_data_sem) != CSP_SEMAPHORE_OK)
    {
        LOGE(tag, "Unable to create data repository mutex");
    }

    LOGD(tag, "Initializing data repositories buffers...")
    /* TODO: Setup external memories */
#if (SCH_STATUS_REPO_MODE == 0)
    {
        //Use internal memory
        int index;
        for(index=0; index<dat_system_last_var; index++)
        {
            dat_set_system_var((dat_system_t)index, INT_MAX);
        }
    }
#else
    {
        //Init storage system
        rc = storage_init(SCH_STORAGE_FILE);
        assertf(rc==0, tag, "Unable to create non-volatile data repository");

        //Init system repo
        rc = storage_table_repo_init(DAT_REPO_SYSTEM, 0);
        assertf(rc==0, tag, "Unable to create system variables repository");

        int index;
        for(index=0; index<dat_system_last_var; index++)
        {
            storage_repo_set_value_idx(index, INT_MAX, DAT_REPO_SYSTEM);
        }

        //Init system flight plan table
        rc=storage_table_flight_plan_init(0);
        assertf(rc==0, tag, "Unable to create flight plan table");
    }
#endif

    /* TODO: Initialize custom variables */
    LOGD(tag, "Initializing system variables values...")
    dat_set_system_var(dat_obc_hours_alive, 0);
    dat_set_system_var(dat_obc_hours_without_reset, 0);
    dat_set_system_var(dat_obc_reset_counter, dat_get_system_var(dat_obc_reset_counter) + 1);  //TODO: This is a non-volatile variable
}

void dat_repo_close(void)
{
#if SCH_STATUS_REPO_MODE == 1
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
    #if SCH_STATUS_REPO_MODE == 0
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
    #if SCH_STATUS_REPO_MODE == 0
        value = DAT_SYSTEM_VAR_BUFF[index];
    //Uses external (non-volatile) memory
    #else
        value = storage_repo_get_value_idx(index, DAT_REPO_SYSTEM);
    #endif

    //Exit critical zone
    osSemaphoreGiven(&repo_data_sem);

    return value;
}

int dat_get_fp(int elapsed_sec, char** command, char** args, int** executions, int** periodical)
{
    int rc = storage_flight_plan_get(elapsed_sec, command, args, executions, periodical);

    return rc;
}

