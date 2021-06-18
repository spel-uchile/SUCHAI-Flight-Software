/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2020, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *      Copyright 2020, Camilo Rojas Milla, camrojas@uchile.cl
 *      Copyright 2020, Matias Ramirez Martinez, nicoram.mt@gmail.com
 *      Copyright 2020, Tamara Gutierrez Rojo tamigr.2293@gmail.com
 *      Copyright 2020, Diego Ortego Prieto, diortego@dcc.uchile.cl
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
    #if SCH_STORAGE_TRIPLE_WR == 1
        static value32_t DAT_SYSTEM_VAR_BUFF[dat_status_last_address * 3];
    #else
        int DAT_SYSTEM_VAR_BUFF[dat_status_last_address];
    #endif
    static fp_entry_t data_base [SCH_FP_MAX_ENTRIES];
#endif

dat_stmachine_t status_machine;

void dat_repo_init(void)
{
    // Init repository mutex
    if(osSemaphoreCreate(&repo_data_sem) != OS_SEMAPHORE_OK)
        LOGE(tag, "Unable to create system status repository mutex");


    LOGD(tag, "Initializing data repositories buffers...")
#if (SCH_STORAGE_MODE == 0)
    {
        // Reset variables (we do not have persistent storage here)
        int index;
        for(index=0; index < dat_status_last_address; index++)
        {
            dat_set_status_var(index, dat_get_status_var_def(index).value);
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

        //Init payloads repo
        int rc = storage_table_payload_init(0);
        assertf(rc==0, tag, "Unable to create payload repo");
    }
#elif (SCH_STORAGE_MODE > 0)
    {
        //Init storage system
        int rc;
        char fs_db_file[sizeof(SCH_STORAGE_FILE) + 10];
        sprintf(fs_db_file, "%s.%u.db",SCH_STORAGE_FILE, SCH_COMM_ADDRESS);
        rc = storage_init(fs_db_file);
        assertf(rc==0, tag, "Unable to create non-volatile data repository");

        //Init system repo
        rc = storage_table_repo_init(DAT_REPO_SYSTEM, 0);
        assertf(rc==0, tag, "Unable to create system variables repository");

        //Init payloads repo
        rc = storage_table_payload_init(0);
        assertf(rc==0, tag, "Unable to create payload repo");

        //Init system flight plan table
        int entries = dat_get_system_var(dat_fpl_queue);
        rc=storage_table_flight_plan_init(0, &entries);
        dat_set_system_var(dat_fpl_queue, entries);
        assertf(rc==0, tag, "Unable to create flight plan table");
    }
#endif
}

void dat_repo_close(void)
{
#if SCH_STORAGE_MODE != 0
    {
        storage_close();
    }
#endif
}

/**
 * Function for testing triple writing.
 *
 * Should do the same as @c dat_set_system_var , but with only one system status repo.
 *
 * @param index Enum index of the field to set
 * @param value Integer value to set the variable to
 */
int _dat_set_system_var(dat_status_address_t index, int value)
{
    int rc = 0;
    //Enter critical zone
    osSemaphoreTake(&repo_data_sem, portMAX_DELAY);

    //Uses internal memory
#if SCH_STORAGE_MODE == 0
    value32_t var = {.i=value};
    DAT_SYSTEM_VAR_BUFF[index] = var;
    //Uses external memory
#else
    rc = storage_repo_set_value_idx(index, value, DAT_REPO_SYSTEM);
#endif

    //Exit critical zone
    osSemaphoreGiven(&repo_data_sem);

    return rc;
}

/**
 * Function for testing triple writing.
 *
 * Should do the same as @c dat_get_system_var , but with only one system status repo.
 *
 * @param index Enum index of the field to get
 * @return The field's value
 */
int _dat_get_system_var(dat_status_address_t index)
{
    value32_t value;

    //Enter critical zone
    osSemaphoreTake(&repo_data_sem, portMAX_DELAY);

    //Use internal (volatile) memory
#if SCH_STORAGE_MODE == 0
    value = DAT_SYSTEM_VAR_BUFF[index];
    //Uses external (non-volatile) memory
#else
    value.i = storage_repo_get_value_idx(index, DAT_REPO_SYSTEM);
#endif

    //Exit critical zone
    osSemaphoreGiven(&repo_data_sem);

    return value.i;
}

///< Compatibility function
int dat_set_system_var(dat_status_address_t index, int value)
{
    value32_t v;
    v.i = value;
    return dat_set_status_var(index, v);
}

int dat_set_status_var(dat_status_address_t index, value32_t value)
{
    int rc = 0;
    //Enter critical zone
    osSemaphoreTake(&repo_data_sem, portMAX_DELAY);

    //Uses internal memory
#if SCH_STORAGE_MODE == 0
    DAT_SYSTEM_VAR_BUFF[index] = value;
    //Uses tripled writing
    #if SCH_STORAGE_TRIPLE_WR == 1
        DAT_SYSTEM_VAR_BUFF[index + dat_status_last_address] = value;
        DAT_SYSTEM_VAR_BUFF[index + dat_status_last_address * 2] = value;
    #endif
    //Uses external memory
#else
    rc = storage_repo_set_value_idx(index, value.i, DAT_REPO_SYSTEM);
    //Uses tripled writing
    #if SCH_STORAGE_TRIPLE_WR == 1
        int rc2 = storage_repo_set_value_idx(index + dat_status_last_address, value.i, DAT_REPO_SYSTEM);
        int rc3 = storage_repo_set_value_idx(index + dat_status_last_address*2, value.i, DAT_REPO_SYSTEM);
        rc = rc & rc2 & rc3;
    #endif
#endif

    //Exit critical zone
    osSemaphoreGiven(&repo_data_sem);

    return rc;
}

int dat_set_status_var_name(char *name, value32_t value)
{
    dat_sys_var_t var = dat_get_status_var_def_name(name);
    if(var.status == -1)
        return -1; // Value not found
    return dat_set_status_var(var.address, value);
}

///< Compatibility function
int dat_get_system_var(dat_status_address_t index)
{
    value32_t var = dat_get_status_var(index);
    return var.i;
}

value32_t dat_get_status_var(dat_status_address_t index)
{
    value32_t value_1;
#if SCH_STORAGE_TRIPLE_WR == 1
    value32_t value_2;
    value32_t value_3;
#endif

    //Enter critical zone
    osSemaphoreTake(&repo_data_sem, portMAX_DELAY);

    //Use internal (volatile) memory
#if SCH_STORAGE_MODE == 0
    value_1 = DAT_SYSTEM_VAR_BUFF[index];
    //Uses tripled writing
    #if SCH_STORAGE_TRIPLE_WR == 1
        value_2 = DAT_SYSTEM_VAR_BUFF[index + dat_status_last_address];
        value_3 = DAT_SYSTEM_VAR_BUFF[index + dat_status_last_address * 2];
    #endif
    //Uses external (non-volatile) memory
#else
    value_1.i = storage_repo_get_value_idx(index, DAT_REPO_SYSTEM);
    //Uses tripled writing
    #if SCH_STORAGE_TRIPLE_WR == 1
        value_2.i = storage_repo_get_value_idx(index + dat_status_last_address, DAT_REPO_SYSTEM);
        value_3.i = storage_repo_get_value_idx(index + dat_status_last_address * 2, DAT_REPO_SYSTEM);
    #endif
#endif

    //Exit critical zone
    osSemaphoreGiven(&repo_data_sem);
    
    // Compare values in tripled reading
#if SCH_STORAGE_TRIPLE_WR == 1
    //Compare value and its copies
    if (value_1.u == value_2.u || value_1.u == value_3.u)
        return value_1;
    else if (value_2.u == value_3.u)
        return value_2;
    else
        LOGE(tag, "Unable to get a correct value for index %d", index);
#endif
    return value_1;
}

value32_t dat_get_status_var_name(char *name)
{
    dat_sys_var_t var = dat_get_status_var_def_name(name);
    return dat_get_status_var(var.address);
}

#if SCH_STORAGE_MODE == 0
static int _dat_set_fp_async(int timetodo, char* command, char* args, int executions, int periodical)
{
    int i;
    for(i = 0;i < SCH_FP_MAX_ENTRIES;i++)
    {
        if(data_base[i].unixtime == -1)
        {
            data_base[i].unixtime = timetodo;
            data_base[i].executions = executions;
            data_base[i].periodical = periodical;

            data_base[i].cmd = malloc(sizeof(char)*SCH_CMD_MAX_STR_NAME);
            data_base[i].args = malloc(sizeof(char)*SCH_CMD_MAX_STR_PARAMS);

            strncpy(data_base[i].cmd, command, SCH_CMD_MAX_STR_NAME);
            strncpy(data_base[i].args,args, SCH_CMD_MAX_STR_FORMAT);

            return 0;
        }
    }

    return 1;
}

static int _dat_del_fp_async(int timetodo)
{
    int i;
    for(i = 0;i < SCH_FP_MAX_ENTRIES;i++)
    {
        if(timetodo == data_base[i].unixtime)
        {
            data_base[i].unixtime = -1;
            data_base[i].executions = 0;
            data_base[i].periodical = 0;
            free(data_base[i].args);
            free(data_base[i].cmd);
            return 0;
        }
    }
    return 1;
}
#endif

int dat_set_fp(int timetodo, char* command, char* args, int executions, int periodical)
{
    int entries = dat_get_system_var(dat_fpl_queue);

    osSemaphoreTake(&repo_data_sem, portMAX_DELAY);
    //Enter critical zone
#if SCH_STORAGE_MODE == 0
    //TODO : agregar signal de segment para responder falla
    int rc = _dat_set_fp_async(timetodo, command, args, executions, periodical);
#else
    int rc = storage_flight_plan_set(timetodo, command, args, executions, periodical, &entries);
#endif
    //Exit critical zone
    osSemaphoreGiven(&repo_data_sem);

    dat_set_system_var(dat_fpl_queue, entries);
    return rc;
}

int dat_get_fp(int elapsed_sec, char* command, char* args, int* executions, int* period)
{
    int rc;
    int entries = dat_get_system_var(dat_fpl_queue);
    osSemaphoreTake(&repo_data_sem, portMAX_DELAY);
    //Enter critical zone
#if SCH_STORAGE_MODE == 0
    int i;
    rc = -1;  // not found by default
    for(i = 0;i < SCH_FP_MAX_ENTRIES;i++)
    {
        if(elapsed_sec == data_base[i].unixtime)
        {
            strcpy(command, data_base[i].cmd);
            strcpy(args,data_base[i].args);
            *executions = data_base[i].executions;
            *period = data_base[i].periodical;

//            if (data_base[i].periodical > 0)
//                _dat_set_fp_async(elapsed_sec+*period, data_base[i].cmd, data_base[i].args, *executions, *period);

            _dat_del_fp_async(elapsed_sec);
            rc = 0;  // found, then break!
            break;
        }
    }
#else
    rc =storage_flight_plan_get(elapsed_sec, command, args, executions, period, &entries);
#endif
    //Exit critical zone
    osSemaphoreGiven(&repo_data_sem);

    dat_set_system_var(dat_fpl_queue, entries);

    return rc;
}

int dat_del_fp(int timetodo)
{
    int entries = dat_get_system_var(dat_fpl_queue);
    osSemaphoreTake(&repo_data_sem, portMAX_DELAY);
    //Enter critical zone
#if SCH_STORAGE_MODE ==0
    int rc = _dat_del_fp_async(timetodo);
#else
    int rc = storage_flight_plan_erase(timetodo, &entries);
#endif
    //Exit critical zone
    osSemaphoreGiven(&repo_data_sem);

    dat_set_system_var(dat_fpl_queue, entries);

    return rc;
}

int dat_reset_fp(void)
{
    int rc;
    int entries = dat_get_system_var(dat_fpl_queue);
    osSemaphoreTake(&repo_data_sem, portMAX_DELAY);
    //Enter critical zone
#if SCH_STORAGE_MODE == 0
    int i;
    for(i=0;i<SCH_FP_MAX_ENTRIES;i++)
    {
        data_base[i].unixtime = -1;
        data_base[i].cmd = NULL;
        data_base[i].args = NULL;
        data_base[i].executions = 0;
        data_base[i].periodical = 0;
    }
    rc = 0;
#else
    rc = storage_table_flight_plan_init(1, &entries);
#endif
    //Exit critical zone
    osSemaphoreGiven(&repo_data_sem);

    dat_set_system_var(dat_fpl_queue, entries);
    return rc;
}

int dat_show_fp (void)
{
    int rc;

    int entries = dat_get_system_var(dat_fpl_queue);
    osSemaphoreTake(&repo_data_sem, portMAX_DELAY);
    //Enter critical zone
#if SCH_STORAGE_MODE ==0
    int cont = 0;
    int i;
    char buffer[80];

    for(i = 0;i < SCH_FP_MAX_ENTRIES; i++)
    {
        if(data_base[i].unixtime!=-1)
        {
            if(cont == 0)
            {
                printf("When\tCommand\tArguments\tExecutions\tPeriodical\n");
                cont++;
            }
            time_t time_to_show = data_base[i].unixtime;
            strftime(buffer, 80, "%Y-%m-%d %H:%M:%S UTC\n", gmtime(&time_to_show));
            printf("%s\t%s\t%s\t%d\t%d\n",buffer,data_base[i].cmd,data_base[i].args,data_base[i].executions,data_base[i].periodical);
        }
    }
    if(cont == 0)
    {
        LOGI(tag, "Flight plan table empty");
    }
    rc = 0;
#else
    rc = storage_flight_plan_show_table(entries);
#endif
    //Exit critical zone
    osSemaphoreGiven(&repo_data_sem);
    return rc;
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
#if defined(AVR32)
    sec = (time_t)new_time;
    return 0;
#elif defined(ESP32)
    //TODO: Implement set time
#elif defined(NANOMIND)
    timestamp_t timestamp = {(uint32_t)new_time, 0};
    clock_set_time(&timestamp);
    return 0;
#elif defined(LINUX)
    // TODO: This needs to be tested on a raspberry LINUX system, to see if the
    //  sudo call asks for permissions or not
    char cmd[64];
    memset(cmd, 0, 64);
    snprintf(cmd, 64, "%s%d", "sudo date +%s -s @", new_time);

    int rc = system(cmd);

    if (rc == -1)
    {
        LOGE(tag, "Failed attempt at creating a child process for setting the status_machine clock");
        return 1;
    }
    else if (rc == 127)
    {
        LOGE(tag, "Failed attempt at starting a shell for setting status_machine clock");
        return 1;
    }
    else
    {
        return rc != 0 ? 1 : 0;
    }
#else
    LOGE(tag, "dat_set_time NOT suported in this platform!")
    return 0;
#endif
}

int dat_show_time(int format)
{
    time_t time_to_show = dat_get_time();

    if(format == 0 || format > 1)
    {
        char buffer[80];
        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S UTC\n", gmtime(&time_to_show));
        LOGR("%s", buffer);
    }
    if(format >= 1)
    {
        LOGR(tag, "%u\n", (unsigned int)time_to_show);
    }

    return 0;
}

int dat_add_payload_sample(void* data, int payload)
{
    int ret;

    int index = dat_get_system_var(data_map[payload].sys_index);
    LOGI(tag, "Adding data for payload %d in index %d", payload, index);

    //Enter critical zone
    osSemaphoreTake(&repo_data_sem, portMAX_DELAY);

//FIXME: use STORAGE_MODE
#if defined(LINUX) || defined(NANOMIND)
    ret = storage_set_payload_data(index, data, payload);
#else
    ret=0;
#endif
    //Exit critical zone
    osSemaphoreGiven(&repo_data_sem);

    // Update address
    if (ret >= 0) {
        dat_set_system_var(data_map[payload].sys_index, index+1+ret);
        return index+1+ret;
    } else {
        LOGE(tag, "Couldn't set data payload %d", payload);
        return -1;
    }
}

int dat_get_payload_sample(void*data, int payload, int index)
{
    int ret;

    osSemaphoreTake(&repo_data_sem, portMAX_DELAY);

    ret = storage_get_payload_data(index, data, payload);
    osSemaphoreGiven(&repo_data_sem);

    return ret;
}


int dat_get_recent_payload_sample(void* data, int payload, int offset)
{
    int ret;

    int index = dat_get_system_var(data_map[payload].sys_index);
    LOGV(tag, "Obtaining data of payload %d, in index %d, sys_var: %d", payload, index,data_map[payload].sys_index );

    //Enter critical zone
    osSemaphoreTake(&repo_data_sem, portMAX_DELAY);
//FIXME: Is this conditional required?
//FIXME: Use STORAGE_MODE
#if defined(LINUX) || defined(NANOMIND)
    if(index-1-offset >= 0) {
        ret = storage_get_payload_data(index-1-offset, data, payload);
    }
    else {
        LOGE(tag, "Asked for too large offset (%d) on payload (%d)", offset, payload);
        ret = -1;
    }
#else
    ret=0;
#endif
    //Exit critical zone
    osSemaphoreGiven(&repo_data_sem);
    return ret;
}

int dat_delete_memory_sections(void)
{
    int ret;
    // Resetting memory system vars
    for(int i = 0; i < last_sensor; ++i)
    {
        // Lock is acquired inside the function
        dat_set_system_var(data_map[i].sys_index, 0);
    }

    //Enter critical zone
    osSemaphoreTake(&repo_data_sem, portMAX_DELAY);
    //Free memory or drop databases
    ret = storage_delete_memory_sections();
    //Exit critical zone
    osSemaphoreGiven(&repo_data_sem);
#if SCH_FP_ENABLED

    int entries = dat_get_system_var(dat_fpl_queue);
    osSemaphoreTake(&repo_data_sem, portMAX_DELAY);
    storage_flight_plan_reset(&entries);
    osSemaphoreGiven(&repo_data_sem);
    dat_set_system_var(dat_fpl_queue, entries >= 0 ? entries : 0);
#endif
    return ret;
}


int get_payloads_tokens(char** tok_sym, char** tok_var, char* order, char* var_names, int i)
{
    const char s[2] = " ";
    tok_sym[0] = strtok(order, s);

    int j=0;
    while(tok_sym[j] != NULL) {
        j++;
        tok_sym[j] = strtok(NULL, s);
    }

    tok_var[0] = strtok(var_names, s);

    j=0;
    while(tok_var[j] != NULL) {
        j++;
        tok_var[j] = strtok(NULL, s);
    }
    return j;
}

void get_value_string(char* ret_string, char* c_type, char* buff)
{
    if(strcmp(c_type, "%f") == 0) {
        if (*((int *)buff) == -1) {
            sprintf(ret_string, " 'nan'");
        } else {
            sprintf(ret_string, " %f", *((float*)buff));
        }
    }
    else if(strcmp(c_type, "%d") == 0) {
        sprintf(ret_string, " %d", *((int*)buff));
    }
    else if(strcmp(c_type, "%u") == 0) {
        sprintf(ret_string, " %u", *((unsigned int*)buff));
    } else if(strcmp(c_type, "%i") == 0) {
        sprintf(ret_string," %i", *((int*)buff));
    }
}

int get_sizeof_type(char* c_type)
{
    if(strcmp(c_type, "%f") == 0) {
        return sizeof(float);
    }
    else if(strcmp(c_type, "%d") == 0) {
        return sizeof(int);
    } else if(strcmp(c_type, "%u") == 0) {
        return sizeof(int);
    } else if(strcmp(c_type, "%i") == 0) {
        return sizeof(int);
    }
    else {
        return -1;
    }
}

int dat_print_payload_struct(void* data, unsigned int payload)
{
    //FIXME: Buffers allocated in stack with magical numbers! Use defined values
    //FIXME: Use malloc to allocate large buffers, here 1280  bytes allocated!
    //FIXME: Task stack size: 5*256 = 1280 bytes!

    char* tok_sym[300];
    char* tok_var[300];
//    char** tok_sym = (char**)malloc(300);
//    char** tok_var = (char**)malloc(300);
    char *order = (char *)malloc(300);
    strcpy(order, data_map[payload].data_order);
    char *var_names = (char *)malloc(1000);
    strcpy(var_names, data_map[payload].var_names);
    int nparams = get_payloads_tokens(tok_sym, tok_var, order, var_names, (int)payload);

    char *values = (char *)malloc(1000);
    char *names = (char *)malloc(1000);
    strcpy(names, "");
    strcpy(values, "");

    int j;
    for(j=0; j < nparams; ++j) {
        int param_size = get_sizeof_type(tok_sym[j]);
        char buff[param_size];
        memcpy(buff, data+(j*param_size), param_size);

        char name[20];
        sprintf(name, " %s", tok_var[j]);
        strcat(names, name);

        char val[20];
        get_value_string(val, tok_sym[j], buff);
        strcat(values, val);

        if(j != nparams-1){
            strcat(names, ",");
            strcat(values, ",");
        }
    }
    strcat(names, ":");
    printf( "%s %s\n", names, values);

    free(order);
    free(var_names);
    free(values);
    free(names);
//    free(tok_sym);
//    free(tok_var);

    return 0;
}

int dat_set_stmachine_state(dat_stmachine_action_t action, unsigned int step, int nsamples)
{
    LOGI(tag, "Changing state to  %d %u %d", action, step, nsamples);
    if (action >= 0 && action < ACT_LAST && step > 0 && nsamples > -2) {
        osSemaphoreTake(&repo_machine_sem, portMAX_DELAY);
        status_machine.action = action;
        status_machine.step = step;
        status_machine.samples_left = nsamples;
        osSemaphoreGiven(&repo_machine_sem);
        return 1;
    }
    return 0;
}

int dat_stmachine_is_sensor_active(int payload, int active_payloads, int n_payloads) {
//    printf("max number of active payload %d\n", (1 << n_payloads));
    if ( active_payloads >= (1 << n_payloads) ) {
        return 0;
    }
    return ( (active_payloads & (1 << payload)) != 0 );
}

void _get_sat_quaterion(quaternion_t *q,  dat_status_address_t index)
{
    assert(index+4 < dat_status_last_address);
    int i;
    for(i=0; i<4; i++)
    {
        value32_t v = dat_get_status_var(index+i);
        q->q[i] = (double)v.f;
    }
}

void _set_sat_quaterion(quaternion_t *q,  dat_status_address_t index)
{
    assert(index+4 < dat_status_last_address);
    int i;
    for(i=0; i<4; i++)
    {
        value32_t v;
        v.f = (float)q->q[i];
        dat_set_status_var(index + i, v);
    }
}

void _get_sat_vector(vector3_t *r, dat_status_address_t index)
{
    assert(index+3 < dat_status_last_address);
    int i;
    for(i=0; i<3; i++)
    {
        value32_t v= dat_get_status_var(index+i);
        r->v[i] = (double)v.f;
    }
}

void _set_sat_vector(vector3_t *r, dat_status_address_t index)
{
    assert(index+3 < dat_status_last_address);
    int i;
    for(i=0; i<3; i++)
    {
        value32_t v;
        v.f = (float)r->v[i];
        dat_set_status_var(index + i, v);
    }
}