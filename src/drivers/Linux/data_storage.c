//
// Created by carlos on 22-08-17.
//

#include "data_storage.h"

static const char *tag = "data_storage";

static sqlite3 *db = NULL;
PGconn *conn = NULL;
char* fp_table = "flightPlan";
char postgres_conf_s[30];

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
    sprintf(postgres_conf_s, "user=%s dbname=fs_db", SCH_STORAGE_PGUSER);
    conn = PQconnectdb(postgres_conf_s);

    if (PQstatus(conn) == CONNECTION_BAD) {
        LOGE(tag, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return -1;
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
        PQfinish(conn);
        return -1;
    }

    if(drop)
    {

    }

    char create_table_string[100];
    sprintf(create_table_string, "CREATE TABLE IF NOT EXISTS %s("
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
    storage_table_payload_init(0);

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

int get_sizeof_type(char* c_type)
{
    if(strcmp(c_type, "%f") == 0) {
        return sizeof(float);
    }
    else if(strcmp(c_type, "%d") == 0) {
        return sizeof(int);
    } else if(strcmp(c_type, "%u") == 0) {
        return sizeof(int);
    } else {
        return -1;
    }
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

int storage_table_payload_init(int drop)
{
#if SCH_STORAGE_MODE == 2
    if(drop)
    {

    }
    int i = 0;
    for(i=0; i< last_sensor; ++i)
    {
        char create_table[300];
        sprintf(create_table, "CREATE TABLE IF NOT EXISTS %s(tstz TIMESTAMPTZ,", data_map[i].table);
        char* tok_sym[30];
        char* tok_var[30];
        char order[50];
        strcpy(order, data_map[i].data_order);
        char var_names[200];
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
        // TODO: manage connection error in res
        PGresult *res = PQexec(conn, create_table);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            LOGE(tag, "command CREATE PAYLOAD failed: %s", PQerrorMessage(conn));
            PQclear(res);
            continue;
        }
        PQclear(res);
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
    sprintf(get_value_query, "SELECT value FROM %s WHERE idx=%d;", table, index);
    LOGD(tag, "%s",  get_value_query);
    // TODO: manage connection error in res
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
    char get_value_query[100];
    sprintf(get_value_query, "SELECT value FROM %s WHERE name=\"%s\";", table, name);
    // TODO: manage connection error in res
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
    char set_value_query[200];
    sprintf(set_value_query, "INSERT INTO %s (idx, value) "
                             "VALUES ("
                             "%d, "
                             "%d) "
                             "ON CONFLICT (idx) DO UPDATE "
                             "SET value = %d; "
                             , table, index, value, value);
    LOGD(tag, "%s",  set_value_query);
    // TODO: manage connection error in res
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
    return 0;
}

void get_value_string(char* ret_string, char* c_type, char* buff)
{
    if(strcmp(c_type, "%f") == 0) {
        sprintf(ret_string, " %f", *((float*)buff));
    }
    else if(strcmp(c_type, "%d") == 0) {
        sprintf(ret_string, " %d", *((int*)buff));
    }
    else if(strcmp(c_type, "%u") == 0) {
        sprintf(ret_string, " %u", *((unsigned int*)buff));
    }
}


int storage_add_payload_data(void* data, int payload)
{
#if SCH_STORAGE_MODE == 2
    char* tok_sym[30];
    char* tok_var[30];
    char order[50];
    strcpy(order, data_map[payload].data_order);
    char var_names[200];
    strcpy(var_names, data_map[payload].var_names);
    int nparams = get_payloads_tokens(tok_sym, tok_var, order, var_names, payload);

    char values[500];
    char names[500];
    strcpy(names, "(tstz,");
    strcpy(values, "(current_timestamp,");

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
    LOGI(tag, "%s", insert_row);
    // TODO: manage connection error in res
    PGresult *res = PQexec(conn, insert_row);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        LOGE(tag, "command INSERT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }
    PQclear(res);
    int ret = 0;
#endif
    return 0;
}

int storage_get_recent_payload_data(void * data, int payload, int delay)
{
#if SCH_STORAGE_MODE == 2
    LOGV(tag, "Obtaining data of payload %d", payload);

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
    sprintf(get_value,"SELECT %s FROM %s ORDER BY tstz DESC LIMIT 1"
                      ,names, data_map[payload].table);


    LOGD(tag, "%s",  get_value);
    PGresult *res = PQexec(conn, get_value);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        LOGE(tag, "command storage_get_recent_payload_data failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }
    // TODO: manage connection error in res

    for(j=0; j < nparams; ++j) {
        int param_size = get_sizeof_type(tok_sym[j]);
        // TODO: switch between data types
        float val = atof(PQgetvalue(res, 0, j));
        memcpy(data+(j*param_size), (char*)&val, param_size);
    }

    PQclear(res);
#endif
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


//////////////////////////////////////////////////////////////
/* Second Mission specific data functions */
/* TODO: refactor next functions for use new data framework */


int storage_table_gps_init(char* table, int drop)
{
    char * init_sql;
    init_sql= "CREATE TABLE IF NOT EXISTS %s("
            "idx INTEGER PRIMARY KEY, "
            "date_time TEXT, "
            "timestamp TEXT, "
            "latitude REAL, "
            "longitude REAL, "
            "height REAL, "
            "velocity_x REAL, "
            "velocity_y REAL, "
            "satellites_number INTEGER, "
            "mode INTEGER, "
            "phase INTEGER);";
    return storage_table_generic_init(table, init_sql, drop);
}

int storage_table_pressure_init(char* table, int drop)
{
    char * init_sql;
    init_sql= "CREATE TABLE IF NOT EXISTS %s("
            "idx INTEGER PRIMARY KEY, "
            "date_time TEXT, "
            "pressure REAL, "
            "temperature REAL, "
            "height REAL);";
    return storage_table_generic_init(table, init_sql, drop);
}

int storage_table_deploy_init(char* table, int drop)
{
    char * init_sql;
    init_sql= "CREATE TABLE IF NOT EXISTS %s("
            "idx INTEGER PRIMARY KEY, "
            "date_time TEXT, "
            "lineal_actuator INTEGER, "
            "servo_motor INTEGER);";
    return storage_table_generic_init(table, init_sql, drop);
}

int storage_table_generic_init(char* table, char* init_sql, int drop)
{
    char *err_msg;
    char *sql;
    int rc;

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
    else
    {
        sql = sqlite3_mprintf(init_sql, table);
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
}

int storage_table_gps_set(const char* table, void* data)
{
//    char *err_msg;
//    int rc;
//
//    char *sql = sqlite3_mprintf(
//            "INSERT OR REPLACE INTO %s "
//                    "(date_time, timestamp, latitude, longitude, height, velocity_x, velocity_y, satellites_number, mode, phase)\n "
//                    "VALUES (datetime(\"now\"), \"%s\", %f, %f, %f, %f, %f, %d, %d, %d);",
//            table, data->timestamp, data->latitude, data->longitude, data->height, data->velocity_x, data->velocity_y, data->satellites_number, data->mode, data->phase);
//
//    rc = sqlite3_exec(db, sql, dummy_callback, 0, &err_msg);
//
//    if (rc != SQLITE_OK)
//    {
//        LOGE(tag, "SQL error: %s", err_msg);
//        sqlite3_free(err_msg);
//        sqlite3_free(sql);
//        return -1;
//    }
//    else
//    {
//        LOGI(tag, "Inserted  gps data");
//        sqlite3_free(err_msg);
//        sqlite3_free(sql);
//        return 0;
//    }

    return 0;
}

int storage_table_gps_get(const char* table, void* data, int n)
{
//    char **results;
//    char *err_msg;
//
//    char *sql = sqlite3_mprintf("SELECT * FROM %s ORDER BY idx DESC LIMIT %d", table, n);
//
//    int row;
//    int col;
//
//    // execute statement
//    sqlite3_get_table(db, sql, &results, &row, &col, &err_msg);
//
//    if(row==0 || col==0)
//    {
//        LOGI(tag, "GPS table empty");
//        return 0;
//    }
//    else
//    {
//        LOGI(tag, "GPS table")
//        int i;
//        for (i = 0; i < (col*row)+col; i++)
//        {
//            printf("%s\t", results[i]);
//            if ((i + 1) % col == 0)
//                printf("\n");
//        }
//
//        for (i = 0; i < row; i++)
//        {
//            // maybe memcpy?
//            strcpy(data[i].timestamp, results[(i*col)+col+2]);
//            data[i].latitude =  atof(results[(i*col)+col+3]);
//            data[i].longitude = atof(results[(i*col)+col+4]);
//            data[i].height = atof(results[(i*col)+col+5]);
//            data[i].velocity_x = atof(results[(i*col)+col+6]);
//            data[i].velocity_y = atof(results[(i*col)+col+7]);
//            data[i].satellites_number = atoi(results[(i*col)+col+8]);
//            data[i].mode = atoi(results[(i*col)+col+9]);
//            data[i].phase = atoi(results[(i*col)+col+10]);
//        }
//    }
    return 0;
}

int storage_table_prs_set(const char* table, void* data)
{
//    char *err_msg;
//    int rc;
//
//    char *sql = sqlite3_mprintf(
//            "INSERT OR REPLACE INTO %s "
//                    "(date_time, pressure, temperature, height)\n "
//                    "VALUES (datetime(\"now\"), %f, %f, %f);",
//            table, data->pressure, data->temperature, data->height);
//
//    rc = sqlite3_exec(db, sql, dummy_callback, 0, &err_msg);
//
//    if (rc != SQLITE_OK)
//    {
//        LOGE(tag, "SQL error: %s", err_msg);
//        sqlite3_free(err_msg);
//        sqlite3_free(sql);
//        return -1;
//    }
//    else
//    {
//        LOGI(tag, "Inserted  prs data");
//        sqlite3_free(err_msg);
//        sqlite3_free(sql);
//        return 0;
//    }

    return 0;
}

int storage_table_prs_get(const char* table, void* data, int n)
{
//    char **results;
//    char *err_msg;
//
//    char *sql = sqlite3_mprintf("SELECT * FROM %s ORDER BY idx DESC LIMIT %d", table, n);
//
//    int row;
//    int col;
//
//    // execute statement
//    sqlite3_get_table(db, sql, &results, &row, &col, &err_msg);
//
//    if(row==0 || col==0)
//    {
//        LOGI(tag, "PRS table empty");
//        return 0;
//    }
//    else
//    {
//        LOGI(tag, "PRS table")
//        int i;
//        for (i = 0; i < (col*row)+col; i++)
//        {
//            printf("%s\t", results[i]);
//            if ((i + 1) % col == 0)
//                printf("\n");
//        }
//
//        for (i = 0; i < row; i++)
//        {
//            data[i].pressure =  atof(results[(i*col)+col+2]);
//            data[i].temperature = atof(results[(i*col)+col+3]);
//            data[i].height = atof(results[(i*col)+col+4]);
//        }
//    }
    return 0;
}

int storage_table_dpl_set(const char* table, void* data)
{
//    char *err_msg;
//    int rc;
//
//    char *sql = sqlite3_mprintf(
//            "INSERT OR REPLACE INTO %s "
//                    "(date_time, lineal_actuator, servo_motor)\n "
//                    "VALUES (datetime(\"now\"), %d, %d);",
//            table, data->lineal_actuator, data->servo_motor);
//
//    rc = sqlite3_exec(db, sql, dummy_callback, 0, &err_msg);
//
//    if (rc != SQLITE_OK)
//    {
//        LOGE(tag, "SQL error: %s", err_msg);
//        sqlite3_free(err_msg);
//        sqlite3_free(sql);
//        return -1;
//    }
//    else
//    {
//        LOGI(tag, "Inserted  dpl data");
//        sqlite3_free(err_msg);
//        sqlite3_free(sql);
//        return 0;
//    }
    return 0;
}

int storage_table_dpl_get(const char* table, void* data, int n)
{
//    char **results;
//    char *err_msg;
//
//    char *sql = sqlite3_mprintf("SELECT * FROM %s ORDER BY idx DESC LIMIT %d", table, n);
//
//    int row;
//    int col;
//
//    // execute statement
//    sqlite3_get_table(db, sql, &results, &row, &col, &err_msg);
//
//    if(row==0 || col==0)
//    {
//        LOGI(tag, "DPL table empty");
//        return 0;
//    }
//    else
//    {
//        LOGI(tag, "DPL table")
//        int i;
//        for (i = 0; i < (col*row)+col; i++)
//        {
//            printf("%s\t", results[i]);
//            if ((i + 1) % col == 0)
//                printf("\n");
//        }
//
//        for (i = 0; i < row; i++)
//        {
//            data[i].lineal_actuator =  atof(results[(i*col)+col+2]);
//            data[i].servo_motor= atof(results[(i*col)+col+3]);
//        }
//    }
    return 0;
}

//////////////////////////////////////////////////////////////





