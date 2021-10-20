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

    /// expand dbname is zero because postgresql documentation
    conn = PQconnectdbParams(keywords, values,0);

    /// Error handling
    if (PQstatus(conn) == CONNECTION_BAD){
        PQfinish(conn);
        return SCH_ST_ERROR;
    }
    else if (PQstatus(conn) == CONNECTION_OK) {
        storage_is_open = 1;
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
    char *err_msg1;

    if (table == NULL){
        return SCH_ST_ERROR;
    }
    if (n_variables < 0){
        return SCH_ST_ERROR;
    }
    if (drop < 0 || drop > 1) {
        return SCH_ST_ERROR;
    }
    uint32_t str_len = (uint32_t) strlen(table);
    /* oid for char* is 2275, according
    * to the documentation, corresponds a cstring.
    * The other value for null terminated arrays is
    * unknown
    */
    Oid param_types = {2275};
    if(drop) {
        PGresult *sql_stmt;

        sql_stmt = PQexecParams(conn,
                             "DROP TABLE $1",
                             1,
                             &param_types,
                             &table,
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
                         "idx BIGSERIAL PRIMARY KEY,"
                         "name TEXT UNIQUE"
                         "value INTEGER)";
    char *err_msg2;
    PGresult *create_table_sql = PQexecParams(conn,
                                                create_table,
                                                1,
                                                &param_types,
                                                table,
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
    char *err_msg1;
    char *err_msg2;
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
    int str_len = (int) strnlen(table,100);

    /* Drop table if drop variables is true */
    if(drop){
        sql1 = "DROP TABLE IF EXISTS $1";
        PGresult *drop_result = PQexecParams(conn,
                                             sql1,
                                             1,
                                             &param_types,
                                             &table,
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
                                                &table,
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
    if(drop){
        char *err_msg;
        char *sql;
        for (int i = 0; i < n_entries; i++){
            sql = "DROP TABLE IF EXISTS $1";
            char *this_table = data_map[i].table;
            int str_len = strlen(this_table);
            PGresult *drop_result =PQexecParams(conn,
                                                sql,
                                                1,
                                                &param_types,
                                                &table,
                                                &str_len,
                                                NULL,
                                                0);
            if(PQresultStatus(drop_result) != PGRES_COMMAND_OK){
                char *errorMessage = PQresultErrorMessage(drop_result);
                strncpy(err_msg,errorMessage,strlen(errorMessage));
                PQclear(drop_result);
                return SCH_ST_ERROR;
            }
            PQclear(drop_result);
        }

        for(int i = 0; i < n_entries; i++){
            char create_table[SCH_BUFF_MAX_LEN * 4]="CREATE TABLE IF NOT EXISTS $1(id SERIAL, tstz TIMESTAMP,";
            memset(&create_table,0,SCH_BUFF_MAX_LEN * 4);

        }
    }
}