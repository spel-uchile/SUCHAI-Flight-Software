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

#define ST_SQL_MAX_LEN  (1000)

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
static int get_value_string(char* dest, char* c_type, void *data);
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
    if(c_type[1] == 'f') {
        float val;
        val =(float) sqlite3_column_double(stmt, j);
        memcpy(buff, &val, sizeof(float));
    }
    else if(c_type[1] == 'd' || c_type[1] == 'i') {
        int32_t val;
        val = sqlite3_column_int(stmt, j);
        memcpy(buff, &val, sizeof(int32_t));
    }
    else if(c_type[1] == 'u') {
        uint32_t val;
        val = (uint32_t) sqlite3_column_int(stmt, j);
        memcpy(buff, &val, sizeof(uint32_t));
    }
    else if(c_type[1] == 'h') {
        uint16_t val;
        val = (uint16_t)sqlite3_column_int(stmt, j);
        memcpy(buff, &val, sizeof(uint16_t));
    }
    else{
        const unsigned char *val;
        val = sqlite3_column_text(stmt, j);
        strncpy(buff, val, SCH_ST_STR_SIZE);
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

int get_value_string(char* dest, char* c_type, void *data)
{
    int size = -1;
    switch (c_type[1]) {
        case 'f':
            size = sprintf(dest, " %f,", *(float*)data);
            break;
        case 'u':
            size = sprintf(dest, " %u,", *(uint32_t*)data);
        case 'i':
        case 'd':
            size = sprintf(dest, " %d,", *(int32_t*)data);
            break;
        case 'h':
            size = sprintf(dest, " %hd,", *(int16_t*)data);
            break;
        case 's':
            size = snprintf(dest, SCH_ST_STR_SIZE+5, " '%s',", (char*)data);
        default:
            break;
    }

    return size;
}

///< Return the size of a type like: "%f"->4, "%d"->4, "%h"->4, etc..
int get_sizeof_type(char* c_type)
{
    int size = -1;
    switch (c_type[1]) {
        case 'f':
            size = sizeof(float);
            break;
        case 'u':
        case 'i':
        case 'd':
            size = sizeof(int32_t);
            break;
        case 'h':
            size = sizeof(int16_t);
            break;
        case 's':
            size = SCH_ST_STR_SIZE;
        default:
            break;
    }

    return size;
}


int storage_payload_set_data(int payload, int index, void *data, data_map_t *schema)
{
    if(data == NULL || schema == NULL)
        return SCH_ST_ERROR;

    char *sql_values = (char *) calloc(ST_SQL_MAX_LEN,1 );
    char *sql_names = (char *) calloc(ST_SQL_MAX_LEN,1 );
    char *sql_values_p = sql_values;
    char *sql_names_p = sql_names;

    char *var_types = strdup(schema->data_order);
    char *var_names = strdup(schema->var_names);

    const char *sep = " ";
    char *tmp_type, *tmp_var;
    char *tok_type = strtok_r(var_types, sep, &tmp_type);
    char *tok_var = strtok_r(var_names, sep, &tmp_var);

    /**
     * Prepare names and values to sql query, ej:
     * "index var1 var2" -> "(id, tstz, index, var1, var2)"
     * {1 31.1212 -125} -> "(0, current_timestamp, 1, 31.1213, -125)"
     */
    int len = sprintf(sql_names, "%s", "(id, tstz,");
    sql_names += len;
    len = sprintf(sql_values, "(%d, current_timestamp,", index);
    sql_values += len;

    while(tok_type != NULL && tok_var != NULL) {
        int name_len = sprintf(sql_names, " %s,", tok_var);
        sql_names += name_len;

        int value_size = get_sizeof_type(tok_type);
        int value_len = get_value_string(sql_values, tok_type, data);
        data += value_size;
        sql_values += value_len;

        tok_type = strtok_r(NULL, sep, &tmp_type);
        tok_var = strtok_r(NULL, sep, &tmp_var);
    }

    // Remove final ',' and replace with ')' to close the statement
    sql_names[-1]=')';
    sql_values[-1]=')';

    assert(sql_values - sql_values_p < ST_SQL_MAX_LEN);
    assert(sql_names - sql_names_p < ST_SQL_MAX_LEN);

    // Create the INSERT VALUES SQL query
    char*  insert_row = (char *)malloc(2*ST_SQL_MAX_LEN);
    snprintf(insert_row, 2*ST_SQL_MAX_LEN, "INSERT INTO %s %s VALUES %s", schema->table , sql_names_p, sql_values_p);

    // Execute
    char* err_msg;
    int rc = sqlite3_exec(db, insert_row, 0, 0, &err_msg);

    if (rc != SQLITE_OK )
        sqlite3_free(err_msg);

    free(var_types);
    free(var_names);
    free(sql_values_p);
    free(sql_names_p);
    free(insert_row);

    return rc != SQLITE_OK ? SCH_ST_ERROR : SCH_ST_OK;
}

int storage_payload_get_data(int payload, int index, void *data, data_map_t *schema)
{
    if(data == NULL || schema == NULL)
        return SCH_ST_ERROR;

    /**
     * Prepare names to sql string, ej: "index var1 var2" -> "index,var1,var2"
     */
    char *var_names2 = strdup(schema->var_names);
    for(int i=0; i< strlen(var_names2); i++)
        if(var_names2[i] == ' ')
            var_names2[i] = ',';

    // Create the SELECT FROM SQL query
    char *get_value_sql = malloc(2*ST_SQL_MAX_LEN);
    sprintf(get_value_sql, "SELECT %s FROM %s WHERE id=%d LIMIT 1", var_names2, schema->table, index);

    char* err_msg;
    int rc;
    sqlite3_stmt* stmt = NULL;

    // Execute statement
    rc = sqlite3_prepare_v2(db, get_value_sql, -1, &stmt, 0);
    free(get_value_sql);
    free(var_names2);
    if(rc != SQLITE_OK) return SCH_ST_ERROR;

    // Fetch only one row's status
    rc = sqlite3_step(stmt);
    if(rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return SCH_ST_ERROR;
    }

    /**
     * Process row, from a string of value to the structure.
     * Convert string to values and copy data to the structure.
     */
    char *types = strdup(schema->data_order);
    const char *sep = " ";
    char *type_tmp;
    char *tok_type = strtok_r(types, sep, &type_tmp);
    int column = 0;
    while(tok_type != NULL) {
        int param_size = get_sizeof_type(tok_type);
        char value_buffer[param_size];
        get_sqlite_value(tok_type, value_buffer, stmt, column++);
        memcpy(data, value_buffer, param_size);
        data += param_size;
        tok_type = strtok_r(NULL, sep, &type_tmp);
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