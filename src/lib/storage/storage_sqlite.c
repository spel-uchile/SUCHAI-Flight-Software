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

#include "suchai/storage.h"
#include <sqlite3.h>

///< Status variables buffer
static size_t status_entries = 0;

///< Flight plan buffer
static char* fp_table = NULL;

///< Payloads storage buffer
static int payloads_entries = 0;
static data_map_t *payloads_schema = NULL;
static char *payloads_table = NULL;

static int storage_is_open = 0;

static sqlite3 *db = NULL;

static void get_sqlite_value(char* c_type, void* buff, sqlite3_stmt* stmt, int j);
static const char* get_sql_type(char* c_type);
static int get_payloads_tokens(char** tok_sym, char** tok_var, char* order, char* var_names);
static void get_value_string(char* ret_string, char* c_type, const char* buff);
static int get_sizeof_type(char* c_type);

int storage_init(const char *db_name)
{
    if(db != NULL) sqlite3_close(db);
    if(sqlite3_open(db_name, &db) != SQLITE_OK)
        return SCH_ST_ERROR;
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

    if(fp_table != NULL) free(fp_table);
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
        sql = sqlite3_mprintf("DROP TABLE IF EXISTS %s", table);
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
                          "periodical int ,"
                          "node int);",
                          table);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        return SCH_ST_ERROR;
    }
    else
        sqlite3_free(sql);

    if(fp_table != NULL) free(fp_table);
    fp_table = strdup(table);
    return SCH_ST_OK;
}

int storage_table_payload_init(char *table, data_map_t *data_map, int n_entries, int drop)
{
    if(table == NULL || data_map == NULL)
        return SCH_ST_ERROR;

    int rc = 0;
    if(drop) {
        char *err_msg;
        char *sql;
        for( int i = 0; i < n_entries; ++i) {
            sql = sqlite3_mprintf("DROP TABLE IF EXISTS %s",  data_map[i].table);
            rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
            if (rc != SQLITE_OK ) {
                sqlite3_free(err_msg);
                sqlite3_free(sql);
            }
            else sqlite3_free(sql);
        }
    }

    for(int i=0; i < n_entries; ++i) {
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
        int nparams = get_payloads_tokens(tok_sym, tok_var, order, var_names);

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

    payloads_entries = n_entries;
    payloads_table = strdup(table);
    payloads_schema = data_map;

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
/* TODO: Use the concept of table */
//int storage_flight_plan_set_st(fp_entry_t *row, char *table)
int storage_flight_plan_set_st(fp_entry_t *row)
{
    if(fp_table == NULL)
        return SCH_ST_ERROR;

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
            "INSERT OR REPLACE INTO %s (time, command, args, executions, periodical, node)\n VALUES (%d, \"%s\", \"%s\", %d, %d, %d);",
            fp_table, timetodo, command, args, executions, period, node);

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
    if(fp_table == NULL)
        return SCH_ST_ERROR;

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
        row->unixtime = atoi(results[col+0]);
        row->cmd = strdup(results[col+1]);
        row->args = strdup(results[col+2]);
        row->executions = atoi(results[col+3]);
        row->periodical = atoi(results[col+4]);
        row->node = atoi(results[col+5]);

        sqlite3_free(sql);
    }
    return SCH_ST_OK;
}

int storage_flight_plan_get_idx(int index, fp_entry_t *row)
{
    if(fp_table == NULL)
        return SCH_ST_ERROR;

    char **results;
    char *err_msg;
    int row_int;
    int col;

    char* sql = sqlite3_mprintf("SELECT * FROM %s WHERE rowid = %d", fp_table, index+1);

    sqlite3_get_table(db, sql, &results, &row_int,&col,&err_msg);

    if(row_int==0 || col==0) {
        sqlite3_free(sql);
        sqlite3_free(err_msg);
        sqlite3_free_table(results);
        return SCH_ST_ERROR;
    }
    else {
        row->unixtime = atoi(results[col+0]);
        row->cmd = strdup(results[col+1]);
        row->args = strdup(results[col+2]);
        row->executions = atoi(results[col+3]);
        row->periodical = atoi(results[col+4]);
        row->node = atoi(results[col+5]);

        sqlite3_free(sql);
    }
    return SCH_ST_OK;
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
    if(fp_table == NULL)
        return SCH_ST_ERROR;

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
    if(fp_table == NULL)
        return SCH_ST_ERROR;

    char *err_msg;
    char *sql = sqlite3_mprintf("DELETE FROM %s\n WHERE rowid = %d", fp_table, index+1);

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

int storage_flight_plan_reset(void)
{
    if(fp_table == NULL)
        return SCH_ST_ERROR;

    char *table = strdup(fp_table);  //flight_plan_init frees fp_table before copy
    int rc = storage_table_flight_plan_init(table, 0, 1);
    free(table);
    return rc;
}

/****** PAYLOAD STORAGE FUNCTIONS *******/

/**
 * Auxiliary functions
 */
const char* get_sql_type(char* c_type)
{
    if(strcmp(c_type, "%f") == 0) {
        return "REAL";
    }
    else if(strcmp(c_type, "%d") == 0) {
        return "INTEGER";
    } else if(strcmp(c_type, "%u") == 0) {
        return "BIGINT";
    } else if(strcmp(c_type, "%h") == 0) {
        return "INTEGER";
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
    else if(strcmp(c_type, "%h") == 0) {
        int val;
        val = sqlite3_column_int(stmt, j);
        memcpy(buff, &val, sizeof(int));
    }
    else{
        const char *val;
        *val = sqlite3_column_text(stmt, j);
        strcpy(buff, val);
    }
}

int get_payloads_tokens(char** tok_sym, char** tok_var, char* order, char* var_names)
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

void get_value_string(char* ret_string, char* c_type, const char* buff)
{
    if(strcmp(c_type, "%f") == 0) {
        if (*((int *)buff) == -1) {
            sprintf(ret_string, " 'nan'");
        } else {
            sprintf(ret_string, " %f", *((float*)buff));
        }
    }
    else if(strcmp(c_type, "%d") == 0) {
        sprintf(ret_string, " %d", *((int32_t *)buff));
    }
    else if(strcmp(c_type, "%u") == 0) {
        sprintf(ret_string, " %u", *((uint32_t *)buff));
    }
    else if(strcmp(c_type, "%i") == 0) {
        sprintf(ret_string," %i", *((int32_t*)buff));
    }
    else if(strcmp(c_type, "%h") == 0) {
        sprintf(ret_string," %hi", *((uint16_t *)buff));
    }
}

int get_sizeof_type(char* c_type)
{
    if(strcmp(c_type, "%f") == 0) {
        return sizeof(float);
    }
    else if(strcmp(c_type, "%d") == 0) {
        return sizeof(int32_t);
    } else if(strcmp(c_type, "%u") == 0) {
        return sizeof(uint32_t);
    } else if(strcmp(c_type, "%i") == 0) {
        return sizeof(int32_t);
    } else if(strcmp(c_type, "%h") == 0) {
        return sizeof(int16_t);
    } else if(strcmp(c_type, "%s") == 0) {
        return SCH_ST_STR_SIZE;
    }
    else {
        return -1;
    }
}


int storage_payload_set_data(int payload, int index, void *data, data_map_t *schema)
{
    if(data == NULL || schema == NULL)
        return SCH_ST_ERROR;

    char* tok_sym[300];
    char* tok_var[300];
    char *order = (char *)malloc(300);
    strcpy(order, schema->data_order);
    char *var_names = (char *)malloc(1000);
    strcpy(var_names, schema->var_names);
    int nparams = get_payloads_tokens(tok_sym, tok_var, order, var_names);

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
    sprintf(insert_row, "INSERT INTO %s %s VALUES %s", schema->table , names, values);
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

int storage_payload_get_data(int payload, int index, void *data, data_map_t *schema)
{
    if(data == NULL || schema == NULL)
        return SCH_ST_ERROR;

    char* tok_sym[300];
    char* tok_var[300];
    char order[300];
    strcpy(order, schema->data_order);
    char var_names[1000];
    strcpy(var_names, schema->var_names);
    int nparams = get_payloads_tokens(tok_sym, tok_var, order, var_names);

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
    sprintf(get_value,"SELECT %s FROM %s WHERE id=%d LIMIT 1", names, schema->table, index);

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
    return storage_table_payload_init(payloads_table, payloads_schema, payloads_entries, 1);
}

int storage_payload_reset_table(int payload)
{
    return SCH_ST_ERROR;
}