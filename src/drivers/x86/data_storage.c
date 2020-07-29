/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2020, Camilo Rojas Milla, camrojas@uchile.cl
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

#include "data_storage.h"

static const char *tag = "data_storage";

#if SCH_STORAGE_MODE == 1
    static sqlite3 *db = NULL;
#elif SCH_STORAGE_MODE == 2
    PGconn *conn = NULL;
#endif

char* fp_table = "flightPlan";
char fs_db_name[15];
char postgres_conf_s[SCH_BUFF_MAX_LEN];

static int dummy_callback(void *data, int argc, char **argv, char **names);

int storage_init(const char *file)
{
    // Open database
#if SCH_STORAGE_MODE == 1
    if(db != NULL)
    {
        LOGW(tag, "Database already open, closing it");
        sqlite3_close(db);
    }

    if(sqlite3_open(file, &db) != SQLITE_OK)
    {
        LOGE(tag, "Can't open database: %s", sqlite3_errmsg(db));
        return -1;
    }
    else
    {
        LOGD(tag, "Opened database successfully");
        return 0;
    }
#elif SCH_STORAGE_MODE == 2
    sprintf(fs_db_name, "fs_db_%u", SCH_COMM_ADDRESS);
    // Check if database exist by connecting to its own db
    snprintf(postgres_conf_s, SCH_BUFF_MAX_LEN, "host=%s user=%s dbname=%s password=%s", SCH_STORAGE_PGHOST, SCH_STORAGE_PGUSER, SCH_STORAGE_PGUSER, SCH_STORAGE_PGPASS);
    conn = PQconnectdb(postgres_conf_s);

    if (PQstatus(conn) == CONNECTION_BAD) {
        LOGE(tag, "Connection to database %s failed: %s", SCH_STORAGE_PGUSER, PQerrorMessage(conn));
        PQfinish(conn);
        return -1;
    }

    char select_exist_db[SCH_BUFF_MAX_LEN];
    memset(&select_exist_db, 0, SCH_BUFF_MAX_LEN);
    snprintf(select_exist_db, SCH_BUFF_MAX_LEN,"SELECT datname FROM pg_database "
                             "WHERE datname = '%s';", fs_db_name);

    LOGD(tag, "SQL command: %s", select_exist_db);
    PGresult *res = PQexec(conn, select_exist_db);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        LOGE(tag, "command failed: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return -1;
    }

    char* value_str;
    if ((value_str = PQgetvalue(res, 0, 0)) == NULL)
    {
        //Create database
        char create_db[SCH_BUFF_MAX_LEN];
        memset(&create_db, 0, SCH_BUFF_MAX_LEN);
        snprintf(create_db, SCH_BUFF_MAX_LEN, "CREATE DATABASE %s;", fs_db_name);
        PQclear(res);
        LOGD(tag, "SQL command: %s", create_db);
        res = PQexec(conn, create_db);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            LOGE(tag, "command failed: %s", PQerrorMessage(conn));
            PQclear(res);
            PQfinish(conn);
            return -1;
        }
    }
    else
    {
        LOGD(tag, "Database %s already created", fs_db_name);
    }

    PQclear(res);
    PQfinish(conn);

    sprintf(postgres_conf_s, " host=%s user=%s dbname=%s password=%s", SCH_STORAGE_PGHOST, SCH_STORAGE_PGUSER, fs_db_name, SCH_STORAGE_PGPASS);
    conn = PQconnectdb(postgres_conf_s);

    if (PQstatus(conn) == CONNECTION_BAD) {
        LOGE(tag, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return -1;
    }

    int ver = PQserverVersion(conn);
    LOGI(tag, "Server version: %d", ver);

#endif
    return 0;
}

int storage_table_repo_init(char* table, int drop)
{
    char *err_msg;
    char *sql;
    int rc;

#if SCH_STORAGE_MODE == 1

    /* Drop table if selected */
    if(drop)
    {
        sql = sqlite3_mprintf("DROP TABLE %s", table);
        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

        if (rc != SQLITE_OK )
        {
            LOGE(tag, "Failed to drop table %s. Error: %s. SQL: %s", table, err_msg, sql);
            sqlite3_free(err_msg);
            sqlite3_free(sql);
            return -1;
        }
        else
        {
            LOGD(tag, "Table %s drop successfully", table);
            sqlite3_free(sql);
        }
    }

    sql = sqlite3_mprintf("CREATE TABLE IF NOT EXISTS %s("
                          "idx INTEGER PRIMARY KEY, "
                          "name TEXT UNIQUE, "
                          "value INT);",
                          table);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK )
    {
        LOGE(tag, "Failed to crate table %s. Error: %s. SQL: %s", table, err_msg, sql);
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        return -1;
    }
    else
    {
        LOGD(tag, "Table %s created successfully", table);
        sqlite3_free(sql);
    }
    return 0;

#elif SCH_STORAGE_MODE == 2

    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n",
            PQerrorMessage(conn));
        PQfinish(conn);
        return -1;
    }

    if(drop)
    {

    }

    char create_table_string[SCH_BUFF_MAX_LEN];
    memset(&create_table_string, 0, SCH_BUFF_MAX_LEN);
    snprintf(create_table_string, SCH_BUFF_MAX_LEN, "CREATE TABLE IF NOT EXISTS %s("
     "idx INTEGER PRIMARY KEY, "
     "name TEXT UNIQUE, "
     "value INT);", table);
    LOGD(tag, "SQL command: %s", create_table_string);
    // TODO: manage connection error in res
    PGresult *res = PQexec(conn, create_table_string);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        LOGE(tag, "command CREATE failed: %s", PQerrorMessage(conn));
        PQclear(res);
    }
    PQclear(res);
    return 0;
#endif
}

int storage_table_flight_plan_init(int drop)
{
    char* err_msg;
    char* sql;
    int rc;

#if SCH_STORAGE_MODE == 1

    /* Drop table if selected */
    if (drop)
    {
        sql = sqlite3_mprintf("DROP TABLE IF EXISTS %s", fp_table);
        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

        if (rc != SQLITE_OK )
        {
            LOGE(tag, "Failed to drop table %s. Error: %s. SQL: %s", fp_table, err_msg, sql);
            sqlite3_free(err_msg);
            sqlite3_free(sql);
            return -1;
        }
        else
        {
            LOGD(tag, "Table %s drop successfully", fp_table);
            sqlite3_free(sql);
        }
    }

    sql = sqlite3_mprintf("CREATE TABLE IF NOT EXISTS %s("
                          "time int PRIMARY KEY , "
                          "command text, "
                          "args text , "
                          "executions int , "
                          "periodical int );",
                          fp_table);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK )
    {
        LOGE(tag, "Failed to crate table %s. Error: %s. SQL: %s", fp_table, err_msg, sql);
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        return -1;
    }
    else
    {
        LOGD(tag, "Table %s created successfully", fp_table);
        sqlite3_free(sql);
    }
#elif  SCH_STORAGE_MODE == 2
    if (drop) {
        char drop_query[SCH_BUFF_MAX_LEN];
        memset(&drop_query, 0, SCH_BUFF_MAX_LEN);
        snprintf(drop_query, SCH_BUFF_MAX_LEN, "DROP TABLE IF EXISTS %s", fp_table);
        PGresult *res = PQexec(conn, drop_query);
        if ( PQresultStatus(res) != PGRES_COMMAND_OK ) {
            LOGE(tag, "Drop fp postgres command: %s failed", drop_query)
            PQclear(res);
        }
    }

    char * create_fp_query = "CREATE TABLE IF NOT EXISTS flightPlan("
                              "time int PRIMARY KEY , "
                              "command text, args text , "
                              "executions int , "
                              "periodical int );";

    PGresult *res = PQexec(conn, create_fp_query);
    if ( PQresultStatus(res) != PGRES_COMMAND_OK ) {
        LOGE(tag, "create fp postgres command: %s failed", create_fp_query)
        PQclear(res);
        return -1;
    }

    PQclear(res);
    return 0;
#endif
    return 0;
}

int storage_table_payload_init(int drop)
{

#if SCH_STORAGE_MODE > 0
    if(drop)
    {

    }

    int i = 0;
    for(i=0; i< last_sensor; ++i)
    {
        char create_table[SCH_BUFF_MAX_LEN*2];
        memset(&create_table, 0, SCH_BUFF_MAX_LEN*2);
        snprintf(create_table, SCH_BUFF_MAX_LEN*2, "CREATE TABLE IF NOT EXISTS %s(id INTEGER, tstz TIMESTAMPTZ,", data_map[i].table);
        char* tok_sym[30];
        char* tok_var[30];
        char order[50];
        strcpy(order, data_map[i].data_order);
        char var_names[SCH_BUFF_MAX_LEN];
        memset(&var_names, 0, SCH_BUFF_MAX_LEN);
        strcpy(var_names, data_map[i].var_names);
        int nparams = get_payloads_tokens(tok_sym, tok_var, order, var_names, i);

        int j=0;
        for(j=0; j < nparams; ++j)
        {
            char line[100];
            sprintf(line, "%s %s", tok_var[j], get_sql_type(tok_sym[j]));
            strcat(create_table, line);
            if(j != nparams-1) {
                strcat(create_table, ",");
            }
        }
        strcat(create_table, ")");
        LOGD(tag, "SQL command: %s", create_table);

#if SCH_STORAGE_MODE ==1
        char* err_msg;
        int rc;
        rc = sqlite3_exec(db, create_table, 0, 0, &err_msg);

        if (rc != SQLITE_OK )
        {
            LOGE(tag, "Failed to crate table %s. Error: %s. SQL: %s", data_map[i].table, err_msg, create_table);
            sqlite3_free(err_msg);
        }
        else
        {
            LOGD(tag, "Table %s created successfully", data_map[i].table);
        }
#elif SCH_STORAGE_MODE==2
        // TODO: manage connection error in res
        PGresult *res = PQexec(conn, create_table);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            LOGE(tag, "command CREATE PAYLOAD failed: %s", PQerrorMessage(conn));
            PQclear(res);
            continue;
        }
        PQclear(res);
#endif
    }
#endif
    return 0;
}


int storage_repo_get_value_idx(int index, char *table)
{
    int value = -1;
#if SCH_STORAGE_MODE == 1
    sqlite3_stmt* stmt = NULL;
    char *sql = sqlite3_mprintf("SELECT value FROM %s WHERE idx=\"%d\";", table, index);

    // execute statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if(rc != SQLITE_OK)
    {
        LOGE(tag, "Selecting data from DB Failed (rc=%d)", rc);
        return -1;
    }

    // fetch only one row's status
    rc = sqlite3_step(stmt);
    value = -1;
    if(rc == SQLITE_ROW)
        value = sqlite3_column_int(stmt, 0);
    else
    LOGE(tag, "Some error encountered (rc=%d)", rc);

    sqlite3_finalize(stmt);
    sqlite3_free(sql);
#elif SCH_STORAGE_MODE == 2
    char get_value_query[SCH_BUFF_MAX_LEN];
    memset(&get_value_query, 0, SCH_BUFF_MAX_LEN);
    snprintf(get_value_query,SCH_BUFF_MAX_LEN, "SELECT value FROM %s WHERE idx=%d;", table, index);
    LOGD(tag, "%s",  get_value_query);
    PGresult *res = PQexec(conn, get_value_query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        LOGE(tag, "command storage_repo_get_value_idx failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }
    char* value_str;
    if ((value_str = PQgetvalue(res, 0, 0)) != NULL)
        value = atoi(value_str);
    else
    {
        LOGE(tag, "The value wasn't found");
    }
    PQclear(res);
#endif
    return value;
}

int storage_repo_get_value_str(char *name, char *table)
{
    int value = -1;
#if SCH_STORAGE_MODE == 1
    sqlite3_stmt* stmt = NULL;
    char *sql = sqlite3_mprintf("SELECT value FROM %s WHERE name=\"%s\";", table, name);

    // execute statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if(rc != 0)
    {
        LOGE(tag, "Selecting data from DB Failed (rc=%d)", rc);
        return -1;
    }

    // fetch only one row's status
    rc = sqlite3_step(stmt);
    if(rc == SQLITE_ROW)
        value = sqlite3_column_int(stmt, 0);
    else
        LOGE(tag, "Some error encountered (rc=%d)", rc);

    sqlite3_finalize(stmt);
    sqlite3_free(sql);
#elif SCH_STORAGE_MODE == 2
    char get_value_query[SCH_BUFF_MAX_LEN];
    memset(&get_value_query, 0, sizeof(get_value_query));
    snprintf(get_value_query, SCH_BUFF_MAX_LEN, "SELECT value FROM %s WHERE name=\"%s\";", table, name);
    PGresult *res = PQexec(conn, get_value_query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        LOGE(tag, "command storage_repo_get_value_str failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }
    value = atoi(PQgetvalue(res, 0, 0));
#endif
    return value;
}

int storage_repo_set_value_idx(int index, int value, char *table)
{
#if SCH_STORAGE_MODE == 1
    char *err_msg;
    char *sql = sqlite3_mprintf("INSERT OR REPLACE INTO %s (idx, name, value) "
                                "VALUES ("
                                "%d, "
                                "(SELECT name FROM %s WHERE idx = \"%d\"), "
                                "%d);",
                                table, index, table, index, value);

    /* Execute SQL statement */
    int rc = sqlite3_exec(db, sql, dummy_callback, 0, &err_msg);

    if( rc != SQLITE_OK )
    {
        LOGE(tag, "SQL error: %s", err_msg);
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        return -1;
    }
    else
    {
        LOGV(tag, "Inserted %d to %d in %s", value, index, table);
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        return 0;
    }
#elif SCH_STORAGE_MODE == 2
    char set_value_query[SCH_BUFF_MAX_LEN];
    memset(&set_value_query, 0, sizeof(set_value_query));
    snprintf(set_value_query,SCH_BUFF_MAX_LEN,"INSERT INTO %s (idx, value) "
                             "VALUES ("
                             "%d, "
                             "%d) "
                             "ON CONFLICT (idx) DO UPDATE "
                             "SET value = %d; "
                             , table, index, value, value);
    LOGD(tag, "%s",  set_value_query);
    PGresult *res = PQexec(conn, set_value_query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        LOGE(tag, "command INSERT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }
    PQclear(res);
#endif

    return 0;
}

int storage_flight_plan_set(int timetodo, char* command, char* args, int executions, int periodical)
{
    #if SCH_STORAGE_MODE > 0
        char * insert_query_template =  "INSERT INTO %s (time, command, args, executions, periodical) "
                               "VALUES (%d, \'%s\', \'%s\', %d, %d) ON CONFLICT (time) DO UPDATE "
                               "SET command=\'%s\', args=\'%s\', executions=%d, periodical=%d;";

        #if SCH_STORAGE_MODE == 2
            char insert_query[SCH_BUFF_MAX_LEN*2];
            memset(&insert_query, 0, sizeof(insert_query));
            snprintf(insert_query,SCH_BUFF_MAX_LEN*2, insert_query_template, fp_table, timetodo, command, args, executions, periodical,
                    command, args, executions, periodical);
            LOGI(tag, "Flight Plan Postgres Command: %s",  insert_query);
            PGresult *res = PQexec(conn, insert_query);
            if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                LOGE(tag, "Flight Plan Postgres Command INSERT failed: %s", PQerrorMessage(conn));
                PQclear(res);
                return -1;
            }
            PQclear(res);

        #elif SCH_STORAGE_MODE == 1
        char *err_msg;
            char *sql = sqlite3_mprintf(
                    "INSERT OR REPLACE INTO %s (time, command, args, executions, periodical)\n VALUES (%d, \"%s\", \"%s\", %d, %d);",
                    fp_table, timetodo, command, args, executions, periodical);

            /* Execute SQL statement */
            int rc = sqlite3_exec(db, sql, dummy_callback, 0, &err_msg);

            if (rc != SQLITE_OK)
            {
                LOGE(tag, "SQL error: %s", err_msg);
                sqlite3_free(err_msg);
                sqlite3_free(sql);
                return -1;
            }
            else
            {
                LOGV(tag, "Inserted (%d, %s, %s, %d, %d) in %s", timetodo, command, args, executions, periodical, fp_table);
                sqlite3_free(err_msg);
                sqlite3_free(sql);
                return 0;
            }
        #endif
    #endif
    return 0;
}

int storage_flight_plan_get(int timetodo, char* command, char* args, int* executions, int* periodical)
{
    #if SCH_STORAGE_MODE > 0
        #if SCH_STORAGE_MODE == 2
            int row;
            int col;

            char get_value_query[100];
            sprintf(get_value_query, "SELECT * FROM %s WHERE time = %d", fp_table, timetodo);
            PGresult *res = PQexec(conn, get_value_query);
            if (PQresultStatus(res) != PGRES_TUPLES_OK) {
                LOGE(tag, "command storage_repo_get_value_str failed: %s", PQerrorMessage(conn));
                PQclear(res);
                return -1;
            }

            row = PQntuples(res);
            col = PQnfields(res);

            if(row==0 || col==0)
            {
                PQclear(res);
                return -1;
            }

            strcpy(command, PQgetvalue(res, 0, 1));
            strcpy(args, PQgetvalue(res, 0, 2));
            *executions = atoi(PQgetvalue(res, 0, 3));
            *periodical = atoi(PQgetvalue(res, 0, 4));

            storage_flight_plan_erase(timetodo);

            if (*periodical > 0)
                storage_flight_plan_set(timetodo+*periodical, command, args,*executions,*periodical);

            PQclear(res);
            return 0;

        #elif SCH_STORAGE_MODE == 1
            char **results;
            char *err_msg;
            int row;
            int col;

            char* sql = sqlite3_mprintf("SELECT * FROM %s WHERE time = %d", fp_table, timetodo);

            sqlite3_get_table(db, sql, &results,&row,&col,&err_msg);

            if(row==0 || col==0)
            {
                sqlite3_free(sql);
                LOGV(tag, "SQL error: %s", err_msg);
                sqlite3_free(err_msg);
                sqlite3_free_table(results);
                return -1;
            }
            else
            {
                strcpy(command, results[6]);
                strcpy(args,results[7]);
                *executions = atoi(results[8]);
                *periodical = atoi(results[9]);

                storage_flight_plan_erase(timetodo);

                if (atoi(results[9]) > 0)
                    storage_flight_plan_set(timetodo+*periodical,results[6],results[7],*executions,*periodical);

                sqlite3_free(sql);
                return 0;
            }
        #endif
    #endif
    return 0;
}

int storage_flight_plan_erase(int timetodo)
{
    #if SCH_STORAGE_MODE > 0
        #if SCH_STORAGE_MODE == 2
            char del_query[SCH_BUFF_MAX_LEN];
            memset(&del_query, 0, SCH_BUFF_MAX_LEN);
            snprintf(del_query, SCH_BUFF_MAX_LEN, "DELETE FROM %s\n WHERE time = %d", fp_table, timetodo);
            PGresult *res = PQexec(conn, del_query);
            if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                LOGE(tag, "Error in function storage_flight_plan_erase, postgres failed: %s", PQerrorMessage(conn));
                PQclear(res);
                return -1;
            }
            PQclear(res);
            return 0;

        #elif SCH_STORAGE_MODE ==1
            char *err_msg;
            char *sql = sqlite3_mprintf("DELETE FROM %s\n WHERE time = %d", fp_table, timetodo);

            /* Execute SQL statement */
            int rc = sqlite3_exec(db, sql, dummy_callback, 0, &err_msg);

            if (rc != SQLITE_OK)
            {
                LOGE(tag, "SQL error: %s", err_msg);
                sqlite3_free(err_msg);
                sqlite3_free(sql);
                return -1;
            }
            else
            {
                LOGV(tag, "Command in time %d, table %s was deleted", timetodo, fp_table);
                sqlite3_free(err_msg);
                sqlite3_free(sql);
                return 0;
            }
        #endif
    #endif
    return 0;
}

int storage_flight_plan_reset(void)
{
    return storage_table_flight_plan_init(1);
}

int storage_flight_plan_show_table (void) {
    #if SCH_STORAGE_MODE > 0
        #if SCH_STORAGE_MODE == 2
        int row;
        int col;

        char get_value_query[SCH_BUFF_MAX_LEN];
        memset(&get_value_query, 0, SCH_BUFF_MAX_LEN);
        snprintf(get_value_query, SCH_BUFF_MAX_LEN, "SELECT * FROM %s", fp_table);
        PGresult *res = PQexec(conn, get_value_query);
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            LOGE(tag, "command storage_repo_get_value_str failed: %s", PQerrorMessage(conn));
            PQclear(res);
            return -1;
        }

        row = PQntuples(res);
        col = PQnfields(res);

        if(row==0 || col==0)
        {
            LOGI(tag, "Flight plan table empty");
            return 0;
        }

        LOGI(tag, "Flight plan table");
        int i;
        for (i = 0; i < (col*row); i++)
        {
            if (i%col == 0)
            {
                time_t timef = atoi(PQgetvalue(res, i/col, i%col));
                printf("%s\t",ctime(&timef));
                continue;
            }
            printf("%s\t", PQgetvalue(res, i/col, i%col));
            if ((i + 1) % col == 0)
                printf("\n");
        }

        #elif SCH_STORAGE_MODE == 1

            char **results;
            char *err_msg;
            int row;
            int col;
            char *sql = sqlite3_mprintf("SELECT * FROM %s", fp_table);

            // execute statement
            sqlite3_get_table(db, sql, &results,&row,&col,&err_msg);

            if(row==0 || col==0)
            {
                LOGI(tag, "Flight plan table empty");
                return 0;
            }

            LOGI(tag, "Flight plan table");
            int i;
            for (i = 0; i < (col*row + 5); i++)
            {
                if (i%col == 0 && i!=0)
                {
                    time_t timef = atoi(results[i]);
                    printf("%s\t",ctime(&timef));
                    continue;
                }
                printf("%s\t", results[i]);
                if ((i + 1) % col == 0)
                    printf("\n");
            }
        #endif
    #endif
    return 0;
}

int storage_set_payload_data(int index, void* data, int payload)
{
    if(payload >= last_sensor)
    {
        LOGE(tag, "Payload id: %d greater than maximum id: %d", payload, last_sensor);
        return -1;
    }
// TODO: Implement payload data for RAM, STORAGE_MODE 0
#if SCH_STORAGE_MODE > 0
    char* tok_sym[30];
    char* tok_var[30];
    char order[50];
    strcpy(order, data_map[payload].data_order);
    char var_names[200];
    strcpy(var_names, data_map[payload].var_names);
    int nparams = get_payloads_tokens(tok_sym, tok_var, order, var_names, payload);

    char values[500];
    char names[500];
    strcpy(names, "(id, tstz,");
    sprintf(values, "(%d, current_timestamp,", index);

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

    strcat(names, ")");
    strcat(values, ")");
    char insert_row[200];
    sprintf(insert_row, "INSERT INTO %s %s VALUES %s",data_map[payload].table, names, values);
    LOGD(tag, "%s", insert_row);

#if SCH_STORAGE_MODE == 1
    char* err_msg;
    int rc;
    rc = sqlite3_exec(db, insert_row, 0, 0, &err_msg);

    if (rc != SQLITE_OK )
    {
        LOGE(tag, "Failed to add value to table %s. Error: %s. SQL: %s", data_map[payload].table, err_msg, insert_row);
        sqlite3_free(err_msg);
        return -1;
    }
#elif SCH_STORAGE_MODE == 2
    PGresult *res = PQexec(conn, insert_row);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        LOGE(tag, "command INSERT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }
    PQclear(res);
#endif
#endif
    return 0;
}

int storage_get_payload_data(int index, void* data, int payload)
{
// TODO: Implement payload data for RAM, STORAGE_MODE 0
#if SCH_STORAGE_MODE > 0
    char* tok_sym[30];
    char* tok_var[30];
    char order[50];
    strcpy(order, data_map[payload].data_order);
    char var_names[200];
    strcpy(var_names, data_map[payload].var_names);
    int nparams = get_payloads_tokens(tok_sym, tok_var, order, var_names, payload);

    char values[500];
    char names[500];

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

    char get_value[200];
    sprintf(get_value,"SELECT %s FROM %s WHERE id=%d LIMIT 1"
            ,names, data_map[payload].table, index);
    LOGD(tag, "%s",  get_value);

#if SCH_STORAGE_MODE == 1
    char* err_msg;
    int rc;
    sqlite3_stmt* stmt = NULL;

    // execute statement
    rc = sqlite3_prepare_v2(db, get_value, -1, &stmt, 0);
    if(rc != SQLITE_OK)
    {
        LOGE(tag, "Selecting data from DB Failed (rc=%d)", rc);
        return -1;
    }

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
    }
    else {
        LOGE(tag, "Some error encountered (rc=%d)", rc);
    }

    sqlite3_finalize(stmt);
#elif SCH_STORAGE_MODE == 2
    PGresult *res = PQexec(conn, get_value);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        LOGE(tag, "command storage_get_recent_payload_data failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    int val;
    for(j=0; j < nparams; ++j) {
        int param_size = get_sizeof_type(tok_sym[j]);
        get_psql_value(tok_sym[j], &val, res, j);
        // TODO: sum data pointer with accumulative param sizes
        memcpy(data+(j*4), &val, param_size);
    }
    PQclear(res);
#endif

#endif
    return 0;
}

int storage_close(void)
{
    #if SCH_STORAGE_MODE == 1
        if(db != NULL)
        {
            LOGD(tag, "Closing database");
            sqlite3_close(db);
            db = NULL;
            return 0;
        }
        else
        {
            LOGW(tag, "Attempting to close a NULL pointer database");
            return -1;
        }
    #endif
    return 0;
}

static int dummy_callback(void *data, int argc, char **argv, char **names)
{
    return 0;
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

#if SCH_STORAGE_MODE == 1
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
#elif SCH_STORAGE_MODE == 2
    void get_psql_value(char* c_type, void* buff, PGresult *res, int j)
    {
        if(strcmp(c_type, "%f") == 0) {
            float val;
            val =(float) atof(PQgetvalue(res, 0, j));
            memcpy(buff, &val, sizeof(float));
        }
        else if(strcmp(c_type, "%d") == 0) {
            int val;
            val =  atoi(PQgetvalue(res, 0, j));
            memcpy(buff, &val, sizeof(int));
        }
        else if(strcmp(c_type, "%u") == 0) {
            unsigned int val;
            // TODO: Change to  strtoul()
            val = (unsigned int) atol(PQgetvalue(res, 0, j));
            memcpy(buff, &val, sizeof(unsigned int));
        }
    }
#endif

//TODO: Remove not used function?
//int storage_repo_set_value_str(char *name, int value, char *table)
//{
//    char *err_msg;
//    char *sql = sqlite3_mprintf("INSERT OR REPLACE INTO %s (idx, name, value) "
//                                "VALUES ("
//                                "(SELECT idx FROM %s WHERE name = \"%s\"), "
//                                "%s, "
//                                "%d);",
//                                table, table, name, name, value);
//
//    /* Execute SQL statement */
//    int rc = sqlite3_exec(db, sql, dummy_callback, 0, &err_msg);
//
//    if( rc != SQLITE_OK )
//    {
//        LOGE(tag, "SQL error: %s", err_msg);
//        sqlite3_free(err_msg);
//        sqlite3_free(sql);
//        return -1;
//    }
//    else
//    {
//        LOGV(tag, "Inserted %d to %s in %s", value, name, table);
//        sqlite3_free(err_msg);
//        sqlite3_free(sql);
//        return 0;
//    }
//}




