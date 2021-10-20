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
    char *err_msg2;


    char *stmt_name = "drop_on";

    if (table == NULL){
        return SCH_ST_ERROR;
    }
    if (n_variables < 0){
        return SCH_ST_ERROR;
    }
    if (drop < 0 ) {
        return SCH_ST_ERROR;
    }
    /* oid for char* is 2275, according
    * to the documentation, corresponds a cstring.
    * The other value for null terminated arrays is
    * unknown
    */
    Oid param_types = {2275};
    if(drop) {
        PGresult *sql_stmt;
        PGresult *result;
        sql_stmt = PQprepare(conn,
                             stmt_name,
                             "DROP TABLE $1",
                             1,
                             &param_types);
        if (PQresultStatus(sql_stmt) != PGRES_COMMAND_OK) {
            strcpy(err_msg1, PQerrorMessage(sql_stmt));
            PQclear(sql_stmt);
            return SCH_ST_ERROR;
        }
        result = PQexecPrepared(conn,
                                stmt_name,
                                1,
                                table,
                                strlen(table),
                                NULL,
                                0);
        if (PQresultStatus(result) != PGRES_COMMAND_OK) {
            strcpy(err_msg2, PQerrorMessage(result));
            PQclear(result);
            return SCH_ST_ERROR;
        }
        PQclear(sql_stmt);
        PQclear(result);
    }
    char *create_table = "CREATE TABLE IF NOT EXISTS$1("
                         "idx BIGSERIAL PRIMARY KEY,"
                         "name TEXT UNIQUE"
                         "value INTEGER)";
    char *err_msg3;
    PGresult *create_table_prepared = PQprepare(conn,
                                                "create_table_status",
                                                create_table,
                                                1,
                                                &param_types);
    if(PQresultStatus(create_table_prepared) != PGRES_COMMAND_OK){
        strcpy(err_msg3, PQerrorMessage(create_table_prepared));
        PQclear(create_table_prepared);
        return SCH_ST_ERROR;
    }
    char *err_msg4;
    PGresult *create_table_executed = PQexecPrepared(conn,
                                                     "create_table_status",
                                                     1,
                                                     table,
                                                     strlen(table),
                                                     NULL,
                                                     0);
    if(PQresultStatus(create_table_executed) != PGRES_COMMAND_OK){
        strcpy(err_msg4, PQresultErrorMessage(create_table_executed));
        PQclear(create_table_executed);
        return SCH_ST_ERROR;
    }
    PQclear(create_table_prepared);
    PQclear(create_table_executed);
    return SCH_ST_OK;
}

int storage_table_flight_plan_init(char *table, int n_entires, int drop)
{
    char *err_msg1;
    char *sql;
    int rc;

    /* Drop table if drop variables is true */
    if(drop == 1){
        sql = "DROP TABLE IF EXISTS $1";
        PGresult *drop_result = PQ
    }

}