//
// Created by carlos on 22-08-17.
//

#include "data_storage.h"

static const char *tag = "data_storage";

static sqlite3 *db = NULL;
PGconn *conn = NULL;
char* fp_table = "flightPlan";

static int dummy_callback(void *data, int argc, char **argv, char **names);

int storage_init(const char *file)
{
    if(db != NULL)
    {
        LOGW(tag, "Database already open, closing it");
        sqlite3_close(db);
    }

    // Open database
#if SCH_STORAGE_MODE == 1
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

    char postgres_conf_s[30];
    sprintf(postgres_conf_s, "user=%s dbname=fs_db", SCH_STORAGE_PGUSER);
    conn = PQconnectdb(postgres_conf_s);

    if (PQstatus(conn) == CONNECTION_BAD) {

        fprintf(stderr, "Connection to database failed: %s\n",
            PQerrorMessage(conn));
        PQfinish(conn);
    }

    int ver = PQserverVersion(conn);

    printf("Server version: %d\n", ver);
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
        return 0;
    }
#elif SCH_STORAGE_MODE == 2

    if (PQstatus(conn) == CONNECTION_BAD) {

        fprintf(stderr, "Connection to database failed: %s\n",
            PQerrorMessage(conn));
    }

    if(drop)
    {

    }

    LOGI(tag, "Creating postgres table");



    char create_table_string[100];
    sprintf(create_table_string, "CREATE TABLE IF NOT EXISTS %s("
     "idx INTEGER PRIMARY KEY, "
     "name TEXT UNIQUE, "
     "value INT);", table);
    LOGI(tag, "Creating postgres table");
    LOGI(tag, "SQL command: %s", create_table_string);

    PGresult *res = PQexec(conn, create_table_string);
    PQclear(res);
    LOGI(tag, "Finish Creating postgres table");

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
        sqlite3_free(sql);
        return -1;
    }
    else
    {
        LOGD(tag, "Table %s created successfully", fp_table);
        sqlite3_free(sql);
        return 0;
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

    char get_value_query[100];
    sprintf(get_value_query, "SELECT value FROM %s WHERE idx=\"%d\";", table, index);
    LOGI(tag, "%s",  get_value_query);
    PGresult *res = PQexec(conn, get_value_query);
    LOGI(tag, "%s\n", PQgetvalue(res, 0, 0));
    PQclear(res);
    value = 0;
#endif
    return value;
}

int storage_repo_get_value_str(char *name, char *table)
{
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
    int value = -1;
    if(rc == SQLITE_ROW)
        value = sqlite3_column_int(stmt, 0);
    else
    LOGE(tag, "Some error encountered (rc=%d)", rc);

    sqlite3_finalize(stmt);
    sqlite3_free(sql);
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
    char set_value_query[200];
    sprintf(set_value_query, "INSERT INTO %s (idx, value) "
                             "VALUES ("
                             "%d, "
                             "%d) "
                             "ON CONFLICT (idx) DO UPDATE "
                             "SET value = %d; "
                             , table, index, value, value);
    LOGI(tag, "%s",  set_value_query);
    PGresult *res = PQexec(conn, set_value_query);
    PQclear(res);

#endif
    return 0;
}

int storage_repo_set_value_str(char *name, int value, char *table)
{
    char *err_msg;
    char *sql = sqlite3_mprintf("INSERT OR REPLACE INTO %s (idx, name, value) "
                                "VALUES ("
                                "(SELECT idx FROM %s WHERE name = \"%s\"), "
                                "%s, "
                                "%d);",
                                table, table, name, name, value);

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
        LOGV(tag, "Inserted %d to %s in %s", value, name, table);
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        return 0;
    }
}

int storage_flight_plan_set(int timetodo, char* command, char* args, int executions, int periodical)
{
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
}

int storage_flight_plan_get(int timetodo, char* command, char* args, int* executions, int* periodical)
{
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
}

int storage_flight_plan_erase(int timetodo)
{

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
}

int storage_flight_plan_reset(void)
{
    return storage_table_flight_plan_init(1);
}

int storage_show_table (void) {
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
    else
    {
        LOGI(tag, "Flight plan table")
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
    }
    return 0;
}

int storage_close(void)
{
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
}

static int dummy_callback(void *data, int argc, char **argv, char **names)
{
    return 0;
}
