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
#include <libpq-fe.h>
#include <math.h>

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

static PGconn *conn = NULL;

static const char* get_sql_type(char* c_type);
static int get_payloads_tokens(char** tok_sym, char** tok_var, char* order, char* var_names);
static int get_value_string(char* dest, char* c_type, void *data);
static int get_sizeof_type(char* c_type);
static void get_postgres_value(char* c_type, void* buff, PGresult *stmt, int j);

void prepare_statements(PGconn *co){
    /*
     * 2275:    oid for cstring
     * 23:      oid for int4 (32 bit integer)
     */
    int nprepared = 4;
    int nok_prepared = 0;
    Oid params_type_storage_get_value_idx[2] = {2275, 23};
    PGresult *storage_status_get_value_idx_prepared = PQprepare(co,
                                                                "stmt_storage_status_get_value_idx",
                                                                "SELECT value FROM $1 WHERE idx=$2",
                                                                2,
                                                                params_type_storage_get_value_idx);
    if(PQresultStatus(storage_status_get_value_idx_prepared) != PGRES_COMMAND_OK){
        char *err = PQresultErrorMessage(storage_status_get_value_idx_prepared);
        PQclear(storage_status_get_value_idx_prepared);
    }else{
        nok_prepared++;
    }
    char * sql_storage_status_set_value_idx ="WITH mynames AS"
                                             "(SELECT name FROM $1 WHERE idx=$2) "
                                             "INSERT INTO dummy2(idx,name,value) "
                                             "VALUES ($2,(SELECT name FROM mynames,$3) "
                                             "ON CONFLICT (idx) WHERE idx=$2 "
                                             "DO UPDATE SET (idx, name, value) = "
                                             "($2, (SELECT name FROM mynames),$3)";
    Oid params_type_storage_status_set_value_idx[3]={
            2275,
            23,
            23
    };
    PGresult *storage_status_set_value_idx_prepared = PQprepare(co,
                                                                "stmt_storage_set_value_idx",
                                                                sql_storage_status_set_value_idx,
                                                                3,
                                                                params_type_storage_status_set_value_idx);
    if(PQresultStatus(storage_status_set_value_idx_prepared) != PGRES_COMMAND_OK){
        char *err = PQresultErrorMessage(storage_status_set_value_idx_prepared);
        PQclear(storage_status_set_value_idx_prepared);
    }else{
        nok_prepared++;
    }

    char *sql_storage_flight_plan_set_st = "INSERT INTO $1("
                                                "time,"
                                                "command,"
                                                "args,"
                                                "executions,"
                                                "periodical,"
                                                "node) VALUES ("
                                                "$2,"
                                                "$3,"
                                                "$4,"
                                                "$5,"
                                                "$6,"
                                                "$7) "
                                                "ON CONFLICT (time)"
                                                "DO UPDATE SET ("
                                                "time,"
                                                "command,"
                                                "args,"
                                                "execution,"
                                                "periodical,"
                                                "node)=("
                                                "$2,"
                                                "$3,"
                                                "$4,"
                                                "$5,"
                                                "$6,"
                                                "$7)";
    // oid for cstring is 2275
    // oid for int32 is 23
    Oid params_type_storage_flight_plan_set_st[7] ={
            2275,23,2275,2275, 23,23,23
    };
    PGresult *storage_flight_plan_set_st_prepared = PQprepare(co,
                                                              "stmt_storage_flight_plan_set_st",
                                                              sql_storage_flight_plan_set_st,
                                                              7,
                                                              params_type_storage_flight_plan_set_st
            );
    if (PQresultStatus(storage_flight_plan_set_st_prepared) != PGRES_COMMAND_OK){
        char *err = PQresultErrorMessage(storage_flight_plan_set_st_prepared);
        PQclear(storage_flight_plan_set_st_prepared);
    }else{
        nok_prepared++;
    }

    char *sql_storage_flight_plan_get_st = "SELECT * FROM $1 WHERE time=$2";
    Oid params_type_storage_flight_plan_get_st[2] = {
            2275,
            23
    };
    PGresult *storage_flight_plan_get_st_prepared = PQprepare(co,
                                                              "stmt_storage_flight_plan_get_st",
                                                              sql_storage_flight_plan_get_st,
                                                              2,
                                                              params_type_storage_flight_plan_get_st);
    if (PQresultStatus(storage_flight_plan_get_st_prepared) != PGRES_COMMAND_OK){
        char *err = PQresultErrorMessage(storage_flight_plan_get_st_prepared);
        PQclear(storage_flight_plan_get_st_prepared);
    }else{
        nok_prepared++;
    }

    char *sql_storage_flight_plan_get_idx = "SELECT * FROM $1 WHERE idx=$2";
    Oid params_type_storage_flight_plan_get_idx[2] = {2275,23};
    PGresult  *storage_flight_plan_get_idx_prepared = PQprepare(co,
                                                                "stmt_storage_flight_plan_get_idx",
                                                                sql_storage_flight_plan_get_idx,
                                                                2,
                                                                params_type_storage_flight_plan_get_idx);
    if(PQresultStatus(storage_flight_plan_get_idx_prepared) != PGRES_COMMAND_OK){
        char *err = PQresultErrorMessage(storage_flight_plan_get_idx_prepared);
        PQclear(storage_flight_plan_get_idx_prepared);
    }else{
        nok_prepared++;
    }

    char *sql_storage_flight_plan_delete_row = "DELETE FROM $1 WHERE time=$2";
    Oid params_type_storage_flight_plan_delete_row[2] = {2275, 23};
    PGresult *storage_flight_plan_delete_row_prepared = PQprepare(co,
                                                                  "stmt_storage_flight_plan_delete_row",
                                                                  sql_storage_flight_plan_delete_row,
                                                                  2,
                                                                  params_type_storage_flight_plan_delete_row);
    if(PQresultStatus(storage_flight_plan_delete_row_prepared) != PGRES_COMMAND_OK){
        char *err = PQresultErrorMessage(storage_flight_plan_delete_row_prepared);
        PQclear(storage_flight_plan_delete_row_prepared);
    }else{
        nok_prepared++;
    }

    char *sql_storage_flight_plan_delete_row_idx = "DELETE FROM $1 WHERE idx=$2";
    Oid params_type_storage_flight_plan_delete_row_idx[2] = {2275,23};
    PGresult *storage_flight_plan_delete_row_idx_prepared = PQprepare(co,
                                                                     "stmt_storage_flight_plan_delete_row_idx",
                                                                     sql_storage_flight_plan_delete_row_idx,
                                                                     2,
                                                                     params_type_storage_flight_plan_delete_row_idx);
    if(PQresultStatus(storage_flight_plan_delete_row_idx_prepared) != PGRES_COMMAND_OK){
        char *err = PQresultErrorMessage(storage_flight_plan_delete_row_idx_prepared);
        PQclear(storage_flight_plan_delete_row_idx_prepared);
    }else{
        nok_prepared++;
    }
}

int storage_init(const char *db_name)
{
    char *hostaddr = SCH_STORAGE_PGHOST;
    int port = SCH_STORAGE_PGPORT;
    char *dbname;
    char *user = SCH_STORAGE_PGUSER;
    char *password = SCH_STORAGE_PGPASS;

    if (db_name == NULL){
        return SCH_ST_ERROR;
    }
    strcpy(dbname, db_name);
    if(dbname == NULL){
        return SCH_ST_ERROR;
    }
    /* if postgresql eventualy upgrades and
     * changes it params names, you can
     * change here without touching
     * everywhere else
     * */
    char *key_hostaddr = "hostaddr";
    char *key_port = "port";
    char *key_user = "user";
    char *key_password = "password";
    char *key_dbname = "dbname";

    /// Here set the keywords params names array

    const char *keywords[] = {key_hostaddr,
                             key_port,
                             key_user,
                             key_password,
                             key_dbname};

    char *fmt = "%s %u %s %s %s";

    if(sscanf(db_name, fmt, hostaddr, &port, dbname, user, password) != 5){
        return SCH_ST_ERROR;
    }

    char *port_str;
    double port_double = 1.0 * port;

    size_t port_str_size = (size_t) ceil(log10(port_double));
    snprintf(
            port_str,
            port_str_size,
            "%d",
            port
            );

    const char *values[] = {
            hostaddr,
            port_str,
            user,
            password,
            dbname
    };

    /// expand_dbname is zero because postgresql documentation
    conn = PQconnectdbParams(keywords, values,0);

    /// Error handling
    if (PQstatus(conn) == CONNECTION_BAD){
        PQfinish(conn);
        return SCH_ST_ERROR;
    }
    else if (PQstatus(conn) == CONNECTION_OK) {
        storage_is_open = 1;
        prepare_statements(conn);
        return SCH_ST_OK;
    }

}

int storage_close(void)
{
    if (conn != NULL){
        PQfinish(conn);
        conn = NULL;
        storage_is_open = 0;
        return SCH_ST_OK;
    }

    if(fp_table != NULL) free(fp_table);
    return SCH_ST_ERROR;
}

int storage_table_status_init(char *table, int n_variables, int drop)
{
    char err_msg1[SCH_BUFF_MAX_LEN];

    if (table == NULL){
        return SCH_ST_ERROR;
    }
    if (n_variables < 0){
        return SCH_ST_ERROR;
    }
    if (drop < 0 || drop > 1) {
        return SCH_ST_ERROR;
    }
    int32_t str_len = (int32_t) strnlen(table,63);
    /* oid for char* is 2275, according
    * to the documentation, corresponds a cstring.
    * The other value for null terminated arrays is
    * unknown
    */
    Oid param_types = {2275};
    if(drop) {
        PGresult *sql_stmt;
        const char * const param_values[1]= {table};
        sql_stmt = PQexecParams(conn,
                             "DROP TABLE $1",
                             1,
                             &param_types,
                             param_values,
                             &str_len,
                             NULL,
                             0);
        if (PQresultStatus(sql_stmt) != PGRES_COMMAND_OK) {
            char *errorMessage = PQresultErrorMessage(sql_stmt);
            strncpy(err_msg1, errorMessage,strlen(errorMessage));
            PQclear(sql_stmt);
            return SCH_ST_ERROR;
        }
        PQclear(sql_stmt);
    }
    char *create_table = "CREATE TABLE IF NOT EXISTS $1("
                         "idx INTEGER PRIMARY KEY,"
                         "name TEXT UNIQUE"
                         "value INTEGER)";
    char err_msg2[SCH_BUFF_MAX_LEN];
    const char *params_values[1] = {table};
    PGresult *create_table_sql = PQexecParams(conn,
                                                create_table,
                                                1,
                                                &param_types,
                                                params_values,
                                                &str_len,
                                                NULL,
                                                0);
    if(PQresultStatus(create_table_sql) != PGRES_COMMAND_OK){
        char *errorMessage = PQresultErrorMessage(create_table_sql);
        strncpy(err_msg2, errorMessage,strlen(errorMessage));
        PQclear(create_table_sql);
        return SCH_ST_ERROR;
    }

    PQclear(create_table_sql);
    return SCH_ST_OK;
}

int storage_table_flight_plan_init(char *table, int n_entires, int drop)
{
    char err_msg1[SCH_BUFF_MAX_LEN];
    char err_msg2[SCH_BUFF_MAX_LEN];
    char *sql1;
    char *sql2;

    if(table == NULL){
        return SCH_ST_ERROR;
    }
    if (n_entires < 0){
        return SCH_ST_ERROR;
    }
    if (drop < 0 || drop > 1){
        return SCH_ST_ERROR;
    }
    Oid param_types = {2275};
    int32_t str_len = (int32_t) strnlen(table,63);
    const char *param_values[1] = {table};
    /* Drop table if drop variables is true */
    if(drop){
        sql1 = "DROP TABLE IF EXISTS $1";

        PGresult *drop_result = PQexecParams(conn,
                                             sql1,
                                             1,
                                             &param_types,
                                             param_values,
                                             &str_len,
                                             NULL,
                                             0);
        if (PQresultStatus(drop_result) != PGRES_COMMAND_OK){
            char *errorMessage = PQresultErrorMessage(drop_result);
            strncpy(err_msg1,errorMessage,strlen(errorMessage));
            PQclear(drop_result);
            return SCH_ST_ERROR;
        }
        PQclear(drop_result);
    }
    sql2 = "CREATE TABLE IF NOT EXISTS $1("
          "time INTEGER PRIMARY KEY,"
          "command TEXT,"
          "args TEXT,"
          "executions INTEGER,"
          "periodical INTEGER,"
          "node INTEGER)";
    PGresult *create_table_result = PQexecParams(conn,
                                                sql2,
                                                1,
                                                &param_types,
                                                param_values,
                                                &str_len,
                                                NULL,
                                                0);
    if(PQresultStatus(create_table_result) != PGRES_COMMAND_OK){
        char *errorMessage = PQresultErrorMessage(create_table_result);
        strncpy(err_msg2,errorMessage,strlen(errorMessage));
        PQclear(create_table_result);
        return SCH_ST_ERROR;
    }
    PQclear(create_table_result);

    if(fp_table != NULL) free(fp_table);
    fp_table = strdup(table);

    return SCH_ST_OK;
}

int storage_table_payload_init(char *table, data_map_t *data_map, int n_entries, int drop)
{
    if(table == NULL){
        return SCH_ST_ERROR;
    }
    if( data_map == NULL){
        return SCH_ST_ERROR;
    }
    if (n_entries < 0){ // <1 ???
        return SCH_ST_ERROR;
    }
    if (drop < 0  || drop > 1){
        return SCH_ST_ERROR;
    }
    Oid param_types = {2275};
    const char *params_values[1] = {table};
    if(drop) {
        char err_msg[SCH_BUFF_MAX_LEN];
        char *sql;
        for (int i = 0; i < n_entries; i++) {
            sql = "DROP TABLE IF EXISTS $1";
            char *this_table = data_map[i].table;
            // potentially overflow issues
            int32_t str_len = (int32_t) strnlen(this_table,63);
            PGresult *drop_result = PQexecParams(conn,
                                                 sql,
                                                 1,
                                                 &param_types,
                                                 params_values,
                                                 &str_len,
                                                 NULL,
                                                 0);
            if (PQresultStatus(drop_result) != PGRES_COMMAND_OK) {
                char *errorMessage = PQresultErrorMessage(drop_result);
                strncpy(err_msg, errorMessage, strlen(errorMessage));
                PQclear(drop_result);
                return SCH_ST_ERROR;
            }
            PQclear(drop_result);
        }
    }
    for(int i = 0; i < n_entries; i++){
        char create_table[SCH_BUFF_MAX_LEN * 4];
        memset(&create_table,0,SCH_BUFF_MAX_LEN * 4);
        strcpy(create_table,"CREATE TABLE IF NOT EXISTS $1(id SERIAL, tstz TIMESTAMP,");
        /// replace $1 for data_map[i].table
        char *tok_sym[300];
        char *tok_var[300];
        char order[300];
        strcpy(order,data_map[i].data_order);
        char var_names[SCH_BUFF_MAX_LEN * 4];
        memset(var_names, 0, SCH_BUFF_MAX_LEN * 4);
        strcpy(var_names, data_map[i].var_names);
        int nparams = get_payloads_tokens(tok_sym,tok_var, order,var_names);
        // why do not verify n params??

        for (int j = 0; j < nparams; j++){
            char line[100];
            sprintf(line,
                    "%s %s",
                    tok_var[j],
                    get_sql_type(tok_sym[j]));
            strcat(create_table, line);
            if (j != nparams -1){
                strcat(create_table, ",");
            }
        }
        strcat(create_table, ")");

        char err_msg[SCH_BUFF_MAX_LEN];
        Oid params_type = {2275};
        const char * const data_map_table[1]= {data_map[i].table};
        int32_t len_data_map_table = (int32_t) strnlen(data_map[i].table,63);

        PGresult  *create_table_result = PQexecParams(conn,
                                                      create_table,
                                                      1,
                                                      &params_type,
                                                      data_map_table,
                                                      &len_data_map_table,
                                                      NULL,
                                                      0);
        if (PQresultStatus(create_table_result) != PGRES_COMMAND_OK ){
            strcpy(err_msg,PQresultErrorMessage(create_table_result));
            PQclear(create_table_result);
            return SCH_ST_ERROR;
        }
        PQclear(create_table_result);
    }
    payloads_entries = n_entries;
    payloads_table = strdup(table);
    payloads_schema = data_map;
    return SCH_ST_OK;
}

/****** STATUS VARIABLES FUNCTIONS *******/

int storage_status_get_value_idx(uint32_t index, value32_t *value, char *table)
{
    if(!storage_is_open || conn == NULL || PQstatus(conn) != CONNECTION_OK) return SCH_ST_ERROR;

    char index_str[11];
    int index_size_for_string = ceil(log10(index));
    char err[SCH_BUFF_MAX_LEN];
    if(index_size_for_string < 1){
        char *err_msg = "overflow";
        return SCH_ST_ERROR;
    }
    snprintf(index_str,10+1,"%u",index);
    int32_t len_table = (int32_t) strnlen(table,63);

    int param_lengths[2] = {len_table, index_size_for_string};

    const char* const param_values[2] ={table,index_str};
    PGresult *query_result = PQexecPrepared(conn,
                                            "stmt_storage_status_get_value_idx",
                                            2,
                                            param_values,
                                            param_lengths,
                                            NULL,
                                            0);

    if( PQresultStatus(query_result) != PGRES_TUPLES_OK){
        char *err_msg = PQresultErrorMessage(query_result);
        strncpy(err,err_msg, strlen(err_msg)+1);
        PQclear(query_result);
        return SCH_ST_ERROR;
    }else if(PQntuples(query_result) != 1){
        PQclear(query_result);
        return SCH_ST_ERROR;
    }
    char value_from_results[SCH_BUFF_MAX_LEN];
    char *value_from_results2 = PQgetvalue(query_result,0,0);
    if (value_from_results2 == NULL){
        PQclear(query_result);
        return SCH_ST_ERROR;
    }
    /// copying the retuned value form query result because the char*
    /// lives inside the PQresult. So it should be copied
    strncpy(value_from_results, value_from_results2, strlen(value_from_results2)+1);
    char *str_end;

    int32_t val = (int32_t) strtol(value_from_results,&str_end,10);
    if(*str_end != 0){
        PQclear(query_result);
        char *err_msg = "could not convert value from query to integer";
        return SCH_ST_ERROR;
    }
    *value = (value32_t) val;
    PQclear(query_result);
    return SCH_ST_OK;
}

int storage_status_set_value_idx(int index, value32_t value, char *table)
{
    /// Validation
    if(!storage_is_open || conn == NULL || PQstatus(conn) == CONNECTION_BAD){
        return SCH_ST_ERROR;
    }
    if(index < 0){
        return SCH_ST_ERROR;
    }
    if (table == NULL){
        return SCH_ST_ERROR;
    }

    char err_msg[SCH_BUFF_MAX_LEN];
    /// 63 is the longest table name allowed in postgresql
    /// so we can cast it
    int32_t len_table = (int32_t) strnlen(table, 63);

    char str_index[11];
    char str_value[11];
    snprintf(str_index,11,"%d", index);
    snprintf(str_value,11, "%d", value.i);

    int32_t len_str_index = (int32_t) strnlen(str_index,10);
    int32_t len_str_value = (int32_t) strnlen(str_value, 10);

    const char* const param_values[3] ={table,str_index,str_value};

    int32_t param_lengths[3] = {len_table, len_str_index, len_str_value};
    PGresult *query_result = PQexecPrepared(conn,
                                            "stmt_storage_status_set_value_idx",
                                            3,
                                            param_values,
                                            param_lengths,
                                            NULL,
                                            0);
    if(PQresultStatus(query_result) != PGRES_COMMAND_OK) {
        char *err = PQresultErrorMessage(query_result);
        strncpy(err_msg, err,strlen(err));
        PQclear(query_result);
        return SCH_ST_ERROR;
    }

    PQclear(query_result);
    return SCH_ST_OK;
}

/****** FLIGHT PLAN VARIABLES FUNCTIONS *******/
/* TODO: Use the concept of table */
//int storage_flight_plan_set_st(fp_entry_t *row, char *table)
int storage_flight_plan_set_st(fp_entry_t *row)
{
    if(fp_table == NULL){
        return SCH_ST_ERROR;
    }

    if(row == NULL){
        return SCH_ST_ERROR;
    }

    int timetodo = row->unixtime;
    int executions = row->executions;
    int period = row->periodical;
    int node = row->node;
    // We do not copy here, they are copied in storage_flight_plan_set_st
    // Note that the pointers are still valid here
    char * command = row->cmd;
    char * args = row->args;

    char str_timetodo[11];
    char str_executions[11];
    char str_period[11];
    char str_node[11];

    int32_t len_timetodo = ((int32_t) ceil(log10(timetodo))) + 1;
    int32_t len_executions = ((int32_t) ceil(log10(executions))) + 1;
    int32_t len_period = ((int32_t) ceil(log10(period))) + 1;
    int32_t len_node = ((int32_t) ceil(log10(node))) + 1;

    snprintf(str_timetodo,len_timetodo,"%d", timetodo);
    snprintf(str_executions,len_executions,"%d", executions);
    snprintf(str_period,len_period,"%d", period);
    snprintf(str_node,len_node,"%d", node);

    //63 is the max table name length for postgresql
    // no problem casting the result
    int32_t len_fp_table = (int32_t) strnlen(fp_table,63);

    int32_t len_str_timetodo = (int32_t) strnlen(str_timetodo, 10);
    int32_t len_command = (int32_t) strlen(command);
    int32_t len_args = (int32_t) strlen(args);
    int32_t len_str_execution = (int32_t) strnlen(str_executions,10);
    int32_t len_str_period = (int32_t) strnlen(str_period,10);
    int32_t len_str_node = (int32_t) strnlen(str_node,10);

    char err_msg[SCH_BUFF_MAX_LEN];

    const char * const param_values[7] = {fp_table,
                                   str_timetodo,
                                   command,
                                   args,
                                   str_executions,
                                   str_period,
                                   str_node};
    int param_lengths[7]={
            len_fp_table,
            len_str_timetodo,
            len_command,
            len_args,
            len_str_execution,
            len_str_period,
            len_str_node
    };

    PGresult *query_result = PQexecPrepared(conn,
                                            "stmt_storage_flight_plan_set_st",
                                            7,
                                            param_values,
                                            param_lengths,
                                            NULL,
                                            0);
    if(PQresultStatus(query_result) != PGRES_COMMAND_OK){
        char *err = PQresultErrorMessage(query_result);
        strncpy(err_msg,err,strnlen(err, SCH_BUFF_MAX_LEN));
        PQclear(query_result);
        return SCH_ST_ERROR;
    }

    PQclear(query_result);
    return SCH_ST_OK;
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
    if(fp_table == NULL || row == NULL)
        return SCH_ST_ERROR;

    char **results;
    char err_msg1[SCH_BUFF_MAX_LEN];

    int row_int;
    int col;

    char str_timetodo[11];
    snprintf(str_timetodo,11,"%d",timetodo);

    int32_t len_fp_table = (int32_t) strnlen(fp_table,INT32_MAX);
    int32_t len_str_timetodo = (int32_t) strnlen(str_timetodo,10);

    const char * const param_values[2] ={fp_table, str_timetodo};
    int32_t param_lengths[2] = {len_fp_table, len_str_timetodo};
    PGresult *query_result = PQexecPrepared(conn,
                                            "stmt_storage_flight_plan_get_st",
                                            2,
                                            param_values,
                                            param_lengths,
                                            NULL,
                                            0);
    if(PQresultStatus(query_result) != PGRES_TUPLES_OK){
        char *err = PQresultErrorMessage(query_result);
        strncpy(err_msg1,err,strlen(err));
        PQclear(query_result);
        return SCH_ST_ERROR;
    }
    char *end_ptr;
    char str_unixtime[SCH_BUFF_MAX_LEN];
    strncpy(str_unixtime, PQgetvalue(query_result,0,0),11);

    int32_t result_unixtime = (int32_t) strtol(str_unixtime,&end_ptr,10);

    char err_msg2[SCH_BUFF_MAX_LEN];
    if(*end_ptr != 0){
        strcpy(err_msg2,"error converting field unixtime data from database");
        PQclear(query_result);
        return SCH_ST_ERROR;
    }
    char str_cmd[SCH_BUFF_MAX_LEN];
    strncpy(str_cmd, PQgetvalue(query_result,0,1),SCH_BUFF_MAX_LEN-1);

    char str_args[SCH_BUFF_MAX_LEN];
    strncpy(str_args,PQgetvalue(query_result,0,2),SCH_BUFF_MAX_LEN-1);

    char str_executions[11];
    strncpy(str_executions, PQgetvalue(query_result,0,3),11);
    int32_t result_executions = (int32_t) strtol(str_executions,&end_ptr,10);

    char err_msg3[SCH_BUFF_MAX_LEN];
    if(*end_ptr != 0){
        strcpy(err_msg3,"error converting field executions into integer");
        PQclear(query_result);
        return SCH_ST_ERROR;
    }

    char str_periodical[11];
    strncpy(str_periodical, PQgetvalue(query_result,0,4),11);
    int32_t result_periodical =(int32_t) strtol(str_periodical,&end_ptr,10);

    char err_msg4[SCH_BUFF_MAX_LEN];
    if (*end_ptr != 0){
        strcpy(err_msg4,"error converting field periodical into integer");
        PQclear(query_result);
        return SCH_ST_ERROR;
    }

    char str_node[11];
    strncpy(str_node, PQgetvalue(query_result,0,5),11);
    int32_t result_node = (int32_t) strtol(str_node,&end_ptr,10);

    char err_msg5[SCH_BUFF_MAX_LEN];
    if (*end_ptr != 0){
        strcpy(err_msg5,"error converting field node to integer");
        PQclear(query_result);
        return SCH_ST_ERROR;
    }

    row -> unixtime = result_unixtime;
    row -> cmd = strdup(str_cmd);
    row -> args = strdup(str_args);
    row -> executions = result_executions;
    row -> periodical = result_periodical;
    row -> node = result_node;

    PQclear(query_result);
    return SCH_ST_OK;
}

int storage_flight_plan_get_idx(int index, fp_entry_t *row)
{
    if(fp_table == NULL)
        return SCH_ST_ERROR;

    char err_msg1[SCH_BUFF_MAX_LEN];

    int32_t len_fp_table = (int32_t) strlen(fp_table);
    char str_index[11];
    int32_t len_str_index = ceil(log10(index));
    snprintf(str_index,len_str_index,"%d",index);

    const char * const param_values[2] = {fp_table,str_index};
    int params_lengths[2] = {len_fp_table,len_str_index};

    PGresult *query_result = PQexecPrepared(conn,
                                           "stmt_storage_flight_plan_get_idx",
                                           2,
                                           param_values,
                                           params_lengths,
                                           NULL,
                                           0);
    if(PQresultStatus(query_result) != PGRES_TUPLES_OK){
        strcpy(err_msg1, PQresultErrorMessage(query_result));
        PQclear(query_result);
        return SCH_ST_ERROR;
    }
    char *str_unixtime;
    char *end_ptr;
    strcpy(str_unixtime, PQgetvalue(query_result,0,0));
    int32_t result_unixtime = (int32_t) strtol(str_unixtime,&end_ptr,10);

    char err_msg2[SCH_BUFF_MAX_LEN];
    if(*end_ptr != 0){
        strcpy(err_msg2,"error converting field unixtime to integer");
        PQclear(query_result);
        return SCH_ST_ERROR;
    }

    char str_cmd[SCH_BUFF_MAX_LEN];
    strcpy(str_cmd, PQgetvalue(query_result,0,1));

    char str_args[SCH_BUFF_MAX_LEN];
    strcpy(str_args, PQgetvalue(query_result,0,2));

    char str_executions[11];
    strcpy(str_executions, PQgetvalue(query_result,0,3));
    int32_t result_executions = (int32_t) strtol(str_executions,&end_ptr,10);

    char err_msg3[SCH_BUFF_MAX_LEN];
    if(*end_ptr != 0){
        strcpy(err_msg3,"error converting field executions into integer");
        PQclear(query_result);
        return SCH_ST_ERROR;
    }

    char str_periodical[11];
    strncpy(str_periodical, PQgetvalue(query_result,0,4),11);
    int32_t result_periodical = (int32_t) strtol(str_periodical,&end_ptr,10);

    char err_msg4[SCH_BUFF_MAX_LEN];
    if(*end_ptr != 0){
        strcpy(err_msg4,"error converting field periodical into integer");
        PQclear(query_result);
        return SCH_ST_ERROR;
    }

    char str_node[11];
    strncpy(str_node, PQgetvalue(query_result,0,5),11);
    int32_t result_node = (int32_t) strtol(str_node,&end_ptr,10);

    char err_msg5[SCH_BUFF_MAX_LEN];
    if(*end_ptr != 0){
        strcpy(err_msg5, "error converting field node to integer");
        PQclear(query_result);
        return SCH_ST_ERROR;
    }

    row -> unixtime = result_unixtime;
    row -> cmd = strdup(str_cmd);
    row -> args = strdup(str_args);
    row -> executions = result_executions;
    row -> periodical = result_periodical;
    row -> node = result_node;

    PQclear(query_result);
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
    if (fp_table == NULL)
        return SCH_ST_ERROR;
    char err_msg[SCH_BUFF_MAX_LEN];

    char str_timetodo[11];
    snprintf(str_timetodo,10,"%d",timetodo);

    int32_t len_fp_table = (int32_t) strlen(fp_table);
    int32_t len_timetodo = (int32_t) strlen(str_timetodo);

    const char * const param_values[2] ={fp_table, str_timetodo};
    int32_t params_lengths[2] = {len_fp_table, len_timetodo};

    PGresult *query_result = PQexecPrepared(conn,
                                            "stmt_storage_flight_plan_delete_row",
                                            2,
                                            param_values,
                                            params_lengths,
                                            NULL,
                                            0);
    if(PQresultStatus(query_result) != PGRES_COMMAND_OK ){
        strncpy(err_msg, PQresultErrorMessage(query_result), SCH_BUFF_MAX_LEN-1);
        PQclear(query_result);
        return  SCH_ST_ERROR;
    }
    PQclear(query_result);
    return SCH_ST_OK;
}

int storage_flight_plan_delete_row_idx(int index)
{
    if(fp_table == NULL)
        return SCH_ST_ERROR;

    char err_msg[SCH_BUFF_MAX_LEN];

    char str_index[11];
    int32_t len_str_index = ceil(log10(index));
    int32_t len_fp_table = (int32_t) strlen(fp_table);
    snprintf(str_index,len_str_index,"%d", index);

    const char* const param_values[2] = {fp_table, str_index};
    int32_t param_lengths[2] = {len_fp_table,len_str_index};

    PGresult *query_result = PQexecPrepared(conn,
                                            "stmt_storage_flight_plan_delete_row_id",
                                            2,
                                            param_values,
                                            param_lengths,
                                            NULL,
                                            0);
    if(PQresultStatus(query_result) != PGRES_COMMAND_OK) {
        strncpy(err_msg, PQresultErrorMessage(query_result), SCH_BUFF_MAX_LEN-1);
        PQclear(query_result);
        return SCH_ST_ERROR;
    }

    PQclear(query_result);
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

/**
 * Be aware, this only works for only one row returned
 * */
void get_postgres_value(char* c_type, void* buff, PGresult *stmt, int j)
{
    if(c_type[1] == 'f') {
        float val;
        /// this is why, tup_num = 0 in every condition
        char str_val[SCH_BUFF_MAX_LEN];
        strncpy(str_val,PQgetvalue(stmt, 0,j),SCH_BUFF_MAX_LEN-1);
        char *end_ptr;
        val =(float) strtof(str_val,&end_ptr);
        /*if(*end_ptr != 0){

        }*/
        memcpy(buff, &val, sizeof(float));
    }
    else if(c_type[1] == 'd' || c_type[1] == 'i') {
        int32_t val;
        char str_val[SCH_BUFF_MAX_LEN];
        strncpy(str_val,PQgetvalue(stmt, 0,j),SCH_BUFF_MAX_LEN-1);
        char *end_ptr;
        val = (int32_t) strtol(str_val,&end_ptr,10);
        memcpy(buff, &val, sizeof(int32_t));
    }
    else if(c_type[1] == 'u') {
        uint32_t val;
        char str_val[SCH_BUFF_MAX_LEN];
        strncpy(str_val,PQgetvalue(stmt, 0,j),SCH_BUFF_MAX_LEN-1);
        char *end_ptr;
        val = (uint32_t) strtoul(str_val,&end_ptr,10);
        memcpy(buff, &val, sizeof(uint32_t));
    }
    else if(c_type[1] == 'h') {
        uint16_t val;
        char str_val[SCH_BUFF_MAX_LEN];
        strncpy(str_val,PQgetvalue(stmt, 0,j),SCH_BUFF_MAX_LEN-1);
        char *end_ptr;
        val = (uint16_t) strtoul(str_val,&end_ptr,10);
        memcpy(buff, &val, sizeof(uint16_t));
    }
    else{
        strncpy(buff, PQgetvalue(stmt,0,j), SCH_ST_STR_SIZE);
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
    if (data == NULL || schema == NULL)
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

    PGresult *insert_row_result = PQexec(conn,
                                         insert_row);
    char err_msg[SCH_BUFF_MAX_LEN];
    if(PQresultStatus(insert_row_result) != PGRES_COMMAND_OK){
        strncpy(err_msg, PQresultErrorMessage(insert_row_result), SCH_BUFF_MAX_LEN-1);
        PQclear(insert_row_result);
        return SCH_ST_ERROR;
    }
    free(var_types);
    free(var_names);
    free(sql_values_p);
    free(sql_names_p);
    free(insert_row);

    PQclear(insert_row_result);
    return SCH_ST_OK;
}

int storage_payload_get_data(int payload, int index, void *data, data_map_t *schema)
{
    if (data == NULL || schema == NULL)
        return SCH_ST_ERROR;

    /**
     * Prepare names to sql string, ej: "index var1 var2" -> "index,var1,var2"
     */
    char *var_names2 = strdup(schema->var_names);
    for (int i = 0; i < strlen(var_names2); i++)
        if (var_names2[i] == ' ')
            var_names2[i] = ',';

    // Create the SELECT FROM SQL query
    char *get_value_sql = malloc(2 * ST_SQL_MAX_LEN);
    sprintf(get_value_sql, "SELECT %s FROM %s WHERE id=%d LIMIT 1", var_names2, schema->table, index);

    char* err_msg;
    int rc;

    // Execute statement
    PGresult *select_result = PQexec(conn,
                                     get_value_sql);
    if(PQresultErrorMessage(select_result) != PGRES_TUPLES_OK) {
        strncpy(err_msg, PQresultErrorMessage(select_result), SCH_BUFF_MAX_LEN-1);
        PQclear(select_result);
        return SCH_ST_ERROR;
    }else if(PQntuples(select_result) != 1){
        PQclear(select_result);
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
        get_postgres_value(tok_type, value_buffer, select_result, column++);
        memcpy(data, value_buffer, param_size);
        data += param_size;
        tok_type = strtok_r(NULL, sep, &type_tmp);
    }

    PQclear(select_result);
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