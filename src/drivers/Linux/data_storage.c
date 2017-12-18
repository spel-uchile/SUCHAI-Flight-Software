//
// Created by carlos on 22-08-17.
//

#include "data_storage.h"

static const char *tag = "data_storage";
static sqlite3 *db = NULL;

static int dummy_callback(void *data, int argc, char **argv, char **names);

int storage_init(const char *file)
{
    if(db != NULL)
    {
        LOGW(tag, "Database already open, closing it");
        sqlite3_close(db);
    }

    // Open database
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
}

int storage_table_repo_init(char *table, int drop)
{
    char *err_msg;
    char *sql;
    int rc;

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
}

int storage_table_flight_plan_init(char* table, int drop)
{

    char* err_msg;
    char* sql;
    int rc;

    /* Drop table if selected */
    if (drop)
    {
        sql = sqlite3_mprintf("DROP TABLE IF EXISTS %s", table);
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
                                  "time int PRIMARY KEY , "
                                  "command text, "
                                  "args text , "
                                  "repeat int );",
                          table);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK )
    {
        LOGE(tag, "Failed to crate table %s. Error: %s. SQL: %s", table, err_msg, sql);
        //sqlite3_free(err_msg);
        //sqlite3_free(sql);
        return -1;
    }
    else
    {
        LOGD(tag, "Table %s created successfully", table);
        //sqlite3_free(sql);
        return 0;
    }
}

int storage_repo_get_value_idx(int index, char *table)
{
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
    int value = -1;
    if(rc == SQLITE_ROW)
        value = sqlite3_column_int(stmt, 0);
    else
        LOGE(tag, "Some error encountered (rc=%d)", rc);

    sqlite3_finalize(stmt);
    sqlite3_free(sql);
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
    char *err_msg;
    char *sql = sqlite3_mprintf("INSERT OR REPLACE INTO %s (idx, name, value) "
                                        "VALUES ("
                                        "%d, "
                                        "(SELECT name FROM %s WHERE idx = \"%d\"), "
                                        "%d);",
                                table, index, table, index, value);

    /* Execute SQL statement */
    int rc = sqlite3_exec(db, sql, dummy_callback, 0, &err_msg);

    if( rc != SQLITE_OK ) {
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

    if( rc != SQLITE_OK ) {
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

int storage_flight_plan_set(int timetodo, char* command, char* args, int repeat, char* table) {
    char *err_msg;
    char *sql = sqlite3_mprintf(
            "INSERT OR REPLACE INTO %s (time, command, args, repeat)\n VALUES (%d, \"%s\", \"%s\", %d);",
            table, timetodo, command, args, repeat);

    /* Execute SQL statement */
    int rc = sqlite3_exec(db, sql, dummy_callback, 0, &err_msg);

    if (rc != SQLITE_OK) {
        LOGE(tag, "SQL error: %s", err_msg);
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        return -1;
    }
    else {
        LOGV(tag, "Inserted (%d, %s, %s, %d) in %s", timetodo, command, args, repeat, table);
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        return 0;
    }
}

char* storage_flight_plan_get_command(int timetodo, char* table)
{
    const unsigned char* command;
    sqlite3_stmt* stmt = NULL;
    char *sql = sqlite3_mprintf("SELECT command FROM %s WHERE time = %d;", table, timetodo);

    // execute statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if(rc != 0)
    {
        LOGE(tag, "Selecting data from DB Failed (rc=%d)", rc);
        return NULL;
    }

    // fetch only one row's status
    rc = sqlite3_step(stmt);
    char *var = malloc(sizeof(char)*14);

    if(rc == SQLITE_ROW) {
        command = sqlite3_column_text(stmt, 0);
        strcpy(var, (char*)command);
    }
    else if(rc == SQLITE_DONE) {

        //LOGE(tag, "Some error encountered (rc=%d)", rc);
        return NULL;
    }

    sqlite3_finalize(stmt);
    sqlite3_free(sql);
    return var;

}

char* storage_flight_plan_get_args(int timetodo, char* table)
{
    const unsigned char* args;
    sqlite3_stmt* stmt = NULL;
    char *sql = sqlite3_mprintf("SELECT args FROM %s WHERE time= %d;", table, timetodo);

    // execute statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if(rc != 0)
    {
        LOGE(tag, "Selecting data from DB Failed (rc=%d)", rc);
        return NULL;
    }

    // fetch only one row's status
    rc = sqlite3_step(stmt);

    char *var = malloc(sizeof(char)*14);

    if(rc == SQLITE_ROW) {
        args = sqlite3_column_text(stmt, 0);
        strcpy(var, (char*)args);
    }
    else if(rc == SQLITE_DONE) {

        //LOGE(tag, "Some error encountered (rc=%d)", rc);
        return NULL;
    }

    sqlite3_finalize(stmt);
    sqlite3_free(sql);
    return var;
}

int storage_flight_plan_get_repeat(int timetodo, char* table)
{
    int repeat=0;
    sqlite3_stmt* stmt = NULL;
    char *sql = sqlite3_mprintf("SELECT repeat FROM %s WHERE time = %d;", table, timetodo);

    // execute statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if(rc != 0)
    {
        LOGE(tag, "Selecting data from DB Failed (rc=%d)", rc);
        return NULL;
    }

    // fetch only one row's status
    rc = sqlite3_step(stmt);

    if(rc == SQLITE_ROW)
        repeat  = sqlite3_column_int(stmt, 0);
    else if(rc == SQLITE_DONE) {

        //LOGE(tag, "Some error encountered (rc=%d)", rc);
        return NULL;
    }
    sqlite3_finalize(stmt);
    sqlite3_free(sql);
    return repeat;
}

int storage_flight_plan_erase (int timetodo, char* table)
{

    char *err_msg;
    char *sql = sqlite3_mprintf("DELETE FROM %s\n WHERE time = %d",
            table, timetodo);

    /* Execute SQL statement */
    int rc = sqlite3_exec(db, sql, dummy_callback, 0, &err_msg);

    if (rc != SQLITE_OK) {
        LOGE(tag, "SQL error: %s", err_msg);
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        return -1;
    }
    else {
        LOGV(tag, "Eliminated the command in time: %d, in %s", timetodo, table);
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        return 0;
    }
}

int storage_flight_plan_reset (char* table)
{
    return storage_table_flight_plan_init(table, 1);
}

int storage_show_table (char* table) {
    char **results;
    char *err_msg;
    int row;
    int col;
    char *sql = sqlite3_mprintf("SELECT * FROM %s", table);

    // execute statement
    sqlite3_get_table(db, sql, &results,&row,&col,&err_msg);

    if(row==0 || col==0){
        printf("Empty table !\n");
        return 0;
    }
    else {
        for (int i = 0; i < (col * row) + 4; i++) {
            printf("%s", results[i]);
            if ((i + 1) % col == 0 && i != 0)
                printf("\n");
            else
                printf("\t");
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
        LOGD(tag, "Attempting to close a NULL pointer database");
        return -1;
    }
}

static int dummy_callback(void *data, int argc, char **argv, char **names)
{
    return 0;
}