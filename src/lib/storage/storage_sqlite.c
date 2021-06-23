/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2021, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2021, Camilo Rojas Milla, camrojas@uchile.cl
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

#include "storage.h"

///< Status variables buffer
static value32_t *status_db = NULL;
static size_t status_entries = 0;

///< Flight plan buffer
static fp_entry_t *flightplan_db = NULL;
static size_t flightplan_entries = 0;

///< Payloads storage buffer
static uint8_t *payload_db = NULL;
static uint8_t **payloads_sections_addresses = NULL;
static int payloads_entries = 0;

static int storage_is_open = 0;

static sqlite3 *db = NULL;

char* fp_table = "flightplan";
char fs_db_name[15];
char postgres_conf_s[SCH_BUFF_MAX_LEN];

int storage_init(const char *db_name)
{
    if(db != NULL) sqlite3_close(db);
    if(sqlite3_open(db_name, &db) != SQLITE_OK) return SCH_ST_ERROR;
    storage_is_open = 1;
    return SCH_ST_OK;
}

int storage_close(void)
{
    if(db != NULL) {
        sqlite3_close(db);
        db = NULL;
        storage_is_open = 0;
        return SCH_ST_OK;
    }
    return SCH_ST_ERROR;
}

int storage_table_status_init(char *table, int n_variables, int drop)
{
    char *err_msg;
    char *sql;
    int rc;
    /* Drop table if drop variable is true */
    if(drop)
    {
        sql = sqlite3_mprintf("DROP TABLE %s", table);
        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

        if (rc != SQLITE_OK ) {
            sqlite3_free(err_msg);
            sqlite3_free(sql);
            return SCH_ST_ERROR;
        }
        else sqlite3_free(sql);
    }

    sql = sqlite3_mprintf("CREATE TABLE IF NOT EXISTS %s("
                          "idx INTEGER PRIMARY KEY, "
                          "name TEXT UNIQUE, "
                          "value INT);",
                          table);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        return SCH_ST_ERROR;
    }
    else sqlite3_free(sql);
    return SCH_ST_OK;
}

int storage_table_flight_plan_init(char *table, int n_entires, int drop)
{
    char* err_msg;
    char* sql;
    int rc;

    /* Drop table if drop variables is true */
    if (drop) {
        sql = sqlite3_mprintf("DROP TABLE IF EXISTS %s", fp_table);
        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

        if (rc != SQLITE_OK ) {
            sqlite3_free(err_msg);
            sqlite3_free(sql);
            return SCH_ST_ERROR;
        }
        else sqlite3_free(sql);
    }

    sql = sqlite3_mprintf("CREATE TABLE IF NOT EXISTS %s("
                          "time int PRIMARY KEY , "
                          "command text, "
                          "args text , "
                          "executions int , "
                          "periodical int );",
                          fp_table);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        return SCH_ST_ERROR;
    }
    else sqlite3_free(sql);
    return SCH_ST_OK;
}

int storage_table_payload_init(char *table, int n_entries, int drop)
{
    int rc = 0;
    if(drop) {
        char *err_msg;
        char *sql;
        for( int i = 0; i < last_sensor; ++i) {
            sql = sqlite3_mprintf("DROP TABLE IF EXISTS %s",  data_map[i].table);
            rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
            if (rc != SQLITE_OK ) {
                sqlite3_free(err_msg);
                sqlite3_free(sql);
            }
            else sqlite3_free(sql);
        }
    }

    for(int i=0; i < last_sensor; ++i) {
        char create_table[SCH_BUFF_MAX_LEN * 4];
        memset(&create_table, 0, SCH_BUFF_MAX_LEN * 4);
        snprintf(create_table, SCH_BUFF_MAX_LEN * 4, "CREATE TABLE IF NOT EXISTS %s(id INTEGER, tstz TIMESTAMPTZ,",
                 data_map[i].table);
        char *tok_sym[300];
        char *tok_var[300];
        char order[300];
        strcpy(order, data_map[i].data_order);
        char var_names[SCH_BUFF_MAX_LEN * 4];
        memset(&var_names, 0, SCH_BUFF_MAX_LEN * 4);
        strcpy(var_names, data_map[i].var_names);
        int nparams = get_payloads_tokens(tok_sym, tok_var, order, var_names, i);

        for (int j = 0; j < nparams; ++j) {
            char line[100];
            sprintf(line, "%s %s", tok_var[j], get_sql_type(tok_sym[j]));
            strcat(create_table, line);
            if (j != nparams - 1) {
                strcat(create_table, ",");
            }
        }
        strcat(create_table, ")");

        char *err_msg;
        rc = sqlite3_exec(db, create_table, 0, 0, &err_msg);

        if (rc != SQLITE_OK) {
            sqlite3_free(err_msg);
            return SCH_ST_ERROR;
        }
    }
    return SCH_ST_OK;
}

/****** STATUS VARIABLES FUNCTIONS *******/

int storage_status_get_value_idx(uint32_t index, value32_t *value, char *table)
{
    if(!storage_is_open || db == NULL) return SCH_ST_ERROR;

    sqlite3_stmt* stmt = NULL;
    char *sql = sqlite3_mprintf("SELECT value FROM %s WHERE idx=\"%d\";", table, index);

    // execute statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if(rc != SQLITE_OK) {
        return SCH_ST_ERROR;
    }

    // fetch only one row's status
    rc = sqlite3_step(stmt);
    if(rc == SQLITE_ROW) *value = (value32_t) sqlite3_column_int(stmt, 0);
    else {
        sqlite3_finalize(stmt);
        sqlite3_free(sql);
        return SCH_ST_ERROR;
    }

    sqlite3_finalize(stmt);
    sqlite3_free(sql);
    return SCH_ST_OK;
}

int storage_status_set_value_idx(int index, value32_t value, char *table)
{
    if(!storage_is_open || db == NULL) return SCH_ST_ERROR;

    char *err_msg;
    char *sql = sqlite3_mprintf("INSERT OR REPLACE INTO %s (idx, name, value) "
                                "VALUES ("
                                "%d, "
                                "(SELECT name FROM %s WHERE idx = \"%d\"), "
                                "%d);",
                                table, index, table, index, value);

    /* Execute SQL statement */
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if( rc != SQLITE_OK ) {
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        return SCH_ST_ERROR;
    }

    sqlite3_free(err_msg);
    sqlite3_free(sql);
    return SCH_ST_OK;
}

/****** FLIGHT PLAN VARIABLES FUNCTIONS *******/

int storage_flight_plan_set_st(fp_entry_t *row)
{

    int timetodo = row->unixtime;
    int executions = row->executions;
    int period = row->periodical;
    int node = row->node;
    // We do not copy here, they are copied in storage_flight_plan_set_st
    // Note that the pointers are still valid here
    char * command = row->cmd;
    char * args = row->args;

    char *err_msg;
    char *sql = sqlite3_mprintf(
            "INSERT OR REPLACE INTO %s (time, command, args, executions, periodical)\n VALUES (%d, \"%s\", \"%s\", %d, %d);",
            fp_table, timetodo, command, args, executions, period);

    /* Execute SQL statement */
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        return SCH_ST_ERROR;
    }
    else {
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        return SCH_ST_OK;
    }
}

int storage_flight_plan_set(int timetodo, char* command, char* args, int executions, int period, int node)
{
    if(timetodo < 0 || command == NULL || args == NULL)
        return SCH_ST_ERROR;

    fp_entry_t row;
    row.unixtime = timetodo;
    row.executions = executions;
    row.periodical = period;
    row.node = node;
    // We do not copy here, they are copied in storage_flight_plan_set_st
    // Note that the pointers are still valid here
    row.cmd = command;
    row.args = args;

    return storage_flight_plan_set_st(&row);
}

int storage_flight_plan_get_st(int timetodo, fp_entry_t *row)
{
    char **results;
    char *err_msg;
    int row_int;
    int col;

    char* sql = sqlite3_mprintf("SELECT * FROM %s WHERE time = %d", fp_table, timetodo);

    sqlite3_get_table(db, sql, &results, &row_int,&col,&err_msg);

    if(row_int==0 || col==0) {
        sqlite3_free(sql);
        sqlite3_free(err_msg);
        sqlite3_free_table(results);
        return SCH_ST_ERROR;
    }
    else {
        strcpy(row->cmd, results[6]);
        strcpy(row->args, results[7]);
        row->executions = atoi(results[8]);
        row->periodical = atoi(results[9]);

        // TODO: Check if delete is needed
        // storage_flight_plan_erase(timetodo, entries);

        sqlite3_free(sql);
    }
    return SCH_ST_OK;
}

int storage_flight_plan_get_idx(int index, fp_entry_t *row)
{

//    if(!storage_is_open || flightplan_db == NULL || row == NULL || index > flightplan_entries)
//        return SCH_ST_ERROR;
//
//    fp_entry_copy(flightplan_db + index, row);
//    return SCH_ST_OK;
}

int storage_flight_plan_get_args(int timetodo, char* command, char* args, int* executions, int* period, int* node)
{
    if(command == NULL || args == NULL)
        return SCH_ST_ERROR;

    fp_entry_t fp_entry;
    int rc = storage_flight_plan_get_st(timetodo, &fp_entry);
    if(rc != SCH_ST_OK)
        return rc;

    strcpy(command, fp_entry.cmd);
    strcpy(args, fp_entry.args);
    *executions = fp_entry.executions;
    *period = fp_entry.periodical;
    *node = fp_entry.node;

    return SCH_ST_OK;
}

int storage_flight_plan_delete_row(int timetodo)
{
    char *err_msg;
    char *sql = sqlite3_mprintf("DELETE FROM %s\n WHERE time = %d", fp_table, timetodo);

    /* Execute SQL statement */
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        return SCH_ST_ERROR;
    }

    sqlite3_free(err_msg);
    sqlite3_free(sql);
    return SCH_ST_OK;
}

int storage_flight_plan_delete_row_idx(int index)
{
    return SCH_ST_ERROR;
}

int storage_flight_plan_reset(void)
{
    return storage_table_flight_plan_init(1, 0, 1);
}

/****** PAYLOAD STORAGE FUNCTIONS *******/

/**
 * Auxiliary functions
 */

int _get_sample_address(int payload, int index, size_t size, uint8_t **address)
{
    int samples_per_section = SCH_SIZE_PER_SECTION / size;
    int sample_section = index / samples_per_section;
    int index_in_section = index % samples_per_section;
    int section_index = payload*SCH_SECTIONS_PER_PAYLOAD + sample_section;
    if(sample_section > SCH_SECTIONS_PER_PAYLOAD)
        return SCH_ST_ERROR;
    if(section_index > payloads_entries * SCH_SIZE_PER_SECTION)
        return SCH_ST_ERROR;

    *address = payloads_sections_addresses[section_index] + index_in_section * size;
    if(*address > payload_db + payloads_entries * SCH_SECTIONS_PER_PAYLOAD * SCH_SIZE_PER_SECTION)
        return SCH_ST_ERROR;

    return SCH_ST_OK;
}

const char* get_sql_type(char* c_type)
{
    if(strcmp(c_type, "%f") == 0) {
        return "REAL";
    }
    else if(strcmp(c_type, "%d") == 0) {
        return "INTEGER";
    } else if(strcmp(c_type, "%u") == 0) {
        return "BIGINT";
    } else {
        return "TEXT";
    }
}

void get_sqlite_value(char* c_type, void* buff, sqlite3_stmt* stmt, int j)
{
    if(strcmp(c_type, "%f") == 0) {
        float val;
        val =(float) sqlite3_column_double(stmt, j);
        memcpy(buff, &val, sizeof(float));
    }
    else if(strcmp(c_type, "%d") == 0) {
        int val;
        val = sqlite3_column_int(stmt, j);
        memcpy(buff, &val, sizeof(int));
    }
    else if(strcmp(c_type, "%u") == 0) {
        unsigned int val;
        val = (unsigned int) sqlite3_column_int(stmt, j);
        memcpy(buff, &val, sizeof(unsigned int));
    }
}


int storage_payload_set_data(int payload, int index, void *data, size_t size)
{
    if(payload >= last_sensor) return SCH_ST_ERROR;

    char* tok_sym[300];
    char* tok_var[300];
    char *order = (char *)malloc(300);
    strcpy(order, data_map[payload].data_order);
    char *var_names = (char *)malloc(1000);
    strcpy(var_names, data_map[payload].var_names);
    int nparams = get_payloads_tokens(tok_sym, tok_var, order, var_names, payload);

    char *values = (char *)malloc(1000);
    char *names = (char *)malloc(1000);
    strcpy(names, "(id, tstz,");
    sprintf(values, "(%d, current_timestamp,", index);

    for(int j=0; j < nparams; ++j) {
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

    strcat(names, ")");
    strcat(values, ")");
    char*  insert_row = (char *)malloc(2000);
    sprintf(insert_row, "INSERT INTO %s %s VALUES %s",data_map[payload].table, names, values);
    free(order);
    free(var_names);
    free(values);
    free(names);

    char* err_msg;
    int rc;
    rc = sqlite3_exec(db, insert_row, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        sqlite3_free(err_msg);
        return SCH_ST_ERROR;
    }
    return SCH_ST_OK;
}

int storage_payload_get_data(int payload, int index, void *data, size_t size)
{
    if(payload >= last_sensor) return SCH_ST_ERROR;

    char* tok_sym[300];
    char* tok_var[300];
    char order[300];
    strcpy(order, data_map[payload].data_order);
    char var_names[1000];
    strcpy(var_names, data_map[payload].var_names);
    int nparams = get_payloads_tokens(tok_sym, tok_var, order, var_names, payload);

    char values[1000];
    char names[1000];

    strcpy(names, "");
    int j;
    for(j=0; j < nparams; ++j) {

        char name[20];
        sprintf(name, " %s", tok_var[j]);
        strcat(names, name);

        if(j != nparams-1){
            strcat(names, ",");
        }
    }

    char get_value[2000];
    sprintf(get_value,"SELECT %s FROM %s WHERE id=%d LIMIT 1"
            ,names, data_map[payload].table, index);

    char* err_msg;
    int rc;
    sqlite3_stmt* stmt = NULL;

    // execute statement
    rc = sqlite3_prepare_v2(db, get_value, -1, &stmt, 0);
    if(rc != SQLITE_OK) return SCH_ST_ERROR;

    // fetch only one row's status
    rc = sqlite3_step(stmt);
    int val;
    if(rc == SQLITE_ROW) {
        for(j=0; j < nparams; ++j) {
            int param_size = get_sizeof_type(tok_sym[j]);
            get_sqlite_value(tok_sym[j], &val, stmt, j);
            // TODO: sum data pointer with accumulative param sizes
            memcpy(data+(j*4), &val, param_size);
        }
    } else {
        sqlite3_finalize(stmt);
        return SCH_ST_ERROR;
    }
    sqlite3_finalize(stmt);
    return SCH_ST_OK;
}

int storage_payload_reset(void)
{
    return storage_table_payload_init(NULL, payloads_entries, 1);
}

int storage_payload_reset_table(int payload)
{
    return SCH_ST_ERROR;
}