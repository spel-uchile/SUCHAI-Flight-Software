/**
 * @file storage.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Camilo Rojas M - camrojas@uchile.cl
 * @author Tamara Gutierrez R - tamara.gutierrez@ug.uchile.cl
 * @date 2021
 * @copyright GNU GPL v3
 *
 * Data storage functions
 */

#ifndef SCH_STORAGE_H
#define SCH_STORAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "suchai/config.h"

#define SCH_ST_OK    (0)
#define SCH_ST_ERROR (-1)

#if SCH_STORAGE_MODE == SCH_ST_SQLITE
    #include <sqlite3.h>
    #include "repoDataSchema.h"
#elif SCH_STORAGE_MODE == SCH_ST_POSTGRES
    #include <libpq-fe.h>
#endif


/**
 * A 32 bit variable that can be interpreted as int, uint or float
 */
typedef union value32_u{
    int32_t i;
    uint32_t u;
    float f;
} value32_t;

/**
 * Struct for storing a single timed command, set to execute in the future.
 */
typedef struct __attribute__((packed)) fp_entry {
    int unixtime;               ///< Unix-time, sets when the command should next execute
    int executions;             ///< Amount of times the command will be executed per periodic cycle
    int periodical;             ///< Period of time between executions
    int node;                   ///< Node to execute the command
    char* cmd;                  ///< Command to execute
    char* args;                 ///< Command's arguments
} fp_entry_t;

static inline void fp_entry_copy(fp_entry_t *src, fp_entry_t *dst)
{
    assert(src != NULL);
    assert(dst != NULL);
    dst->unixtime = src->unixtime;
    dst->executions = src->executions;
    dst->periodical = src->periodical;
    dst->node = src->node;

    if(src->cmd != NULL) {
        dst->cmd = malloc(strlen(src->cmd) + 1);
        strcpy(dst->cmd, src->cmd);
    }
    else
        dst->cmd = NULL;

    if(src->args != NULL) {
        dst->args = malloc(strlen(src->args) + 1);
        strcpy(dst->args, src->args);
    }
    else
        dst->args = NULL;
}

static inline void fp_entry_clear(fp_entry_t *fp_entry)
{
    if(fp_entry != NULL)
    {
        if(fp_entry->cmd != NULL) free(fp_entry->cmd);
        if(fp_entry->args != NULL) free(fp_entry->args);
        fp_entry->unixtime = -1;
        fp_entry->executions = 0;
        fp_entry->periodical = 0;
        fp_entry->node = 0;
        fp_entry->cmd = NULL;
        fp_entry->args = NULL;
    }
}


/**
 * Init data storage system.
 * This function opens the database, file, or allocate memory
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param db_name str. DB name
 * @return 0 OK, -1 Error
 */
int storage_init(const char *db_name);

/**
 * Close the opened database
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @return 0 OK, -1 Error
 */
int storage_close(void);

/**
 * Create new status variables table (db, file, or memory area) in the opened database (@relatesalso storage_init).
 * It is a table like (index, name, value). If the table exists do nothing. If drop is set to
 * 1 then drop an existing table and then creates an empty one.
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param table Str. Table name
 * @param n_entries Int. Max number status variables to store
 * @param drop Int. Set to 1 to drop the existing table before create one
 * @return 0 OK, -1 Error
 */
int storage_table_status_init(char *table, int n_entries, int drop);

/**
 * Create new flight plan table in the opened database (@relatesalso storage_init) in the
 * form (time, command, args, repeat, periodic). If the table exists do nothing. If drop is set to
 * 1 then drop an existing table and then creates an empty one.
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param table Str. Table name
 * @param n_entries Int. Max number of flight plan entries
 * @param drop Int. Set to 1 to drop the existing table before create one
 * @return 0 OK, -1 Error
 */
int storage_table_flight_plan_init(char *table,  int n_entries, int drop);

/**
 * Create new payload data table in the opened database (@relatesalso storage_init)
 * for a payload. If the table exists do nothing. If drop is set to
 * 1 then drop an existing table and then creates an empty one.
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param table Str. Table name
 * @param n_entries Int. Number of sensors/payloads to store
 * @param drop Int. Set to 1 to drop the existing table before create one
 * @return 0 OK, -1 Error
 */
int storage_table_payload_init(char *table, int n_entries, int drop);

/****** STATUS VARIABLES FUNCTIONS *******/

/**
 * Get a status variable value by index
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param index Int. Value index
 * @param table Str. Table name
 * @return 0 OK, -1 Error
 */
int storage_status_get_value_idx(uint32_t index, value32_t *value, char *table);

/**
 * Get a status variable value by name
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param name Str. Value name
 * @param table Str. Table name
 * @return 0 OK, -1 Error
 */
int storage_status_get_value_name(char *name, value32_t *value, char *table);

/**
 * Set or update a status variable by index.
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param index Int. Variable index
 * @param value Int. Value to set
 * @param table Str. Table name
 * @return 0 OK, -1 Error
 */
int storage_status_set_value_idx(int index, value32_t value, char *table);

/**
 * Set or update a status variable by name.
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param index Int. Variable index
 * @param value Int. Value to set
 * @param table Str. Table name
 * @return 0 OK, -1 Error
 */
int storage_status_set_value_name(char *name, value32_t value, char *table);

/****** FLIGHT PLAN VARIABLES FUNCTIONS *******/

/**
 * Set or update a flight plan entry
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param timetodo Time to execute the command
 * @param command Command name
 * @param args Command arguments as string
 * @param executions Command max number of executions
 * @param period Period between executions
 * @param node Node to execute the command (for future use)
 * @return 0 OK, -1 Error
 */
int storage_flight_plan_set(int timetodo, char* command, char* args, int executions, int period, int node);
int storage_flight_plan_set_st(fp_entry_t *row);

/**
 * Get a flight plan entry by time (or by table index)
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param timetodo Search a command by time
 * @param command Command name is stored here
 * @param args Command arguments are stored here
 * @param executions Command max number of executions is stored here
 * @param period Period between executions is stored here
 * @param node Node to execute the command is stored here (for future use)
 * @return 0 OK, -1 Error
 */
int storage_flight_plan_get(int timetodo, fp_entry_t *row);
int storage_flight_plan_get_idx(int index, fp_entry_t *row);
int storage_flight_plan_get_args(int timetodo, char* command, char* args, int* executions, int* period, int* node);


/**
 * Erase a flight plan table entry by time
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param timetodo Search the row by time
 * @return 0 OK, -1 Error
 */
int storage_flight_plan_delete_row(int timetodo);
int storage_flight_plan_delete_row_idx(int index);

/**
 * Reset the flight plan table (delete all entries)
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @return 0 OK, -1 Error
 */
int storage_flight_plan_reset(void);


/****** PAYLOAD STORAGE FUNCTIONS *******/

/**
 * Set a value for specific payload with index value
 * in database
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param index Int. index address
 * @param data Pointer to struct
 * @param size Struct size
 * @param payload Int. payload to store
 * @return 0 OK, -1 Error
 */
int storage_payload_set_data(int payload, int index, void *data, size_t size);

/**
 * Get a value for specific payload with index value
 * in database
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param index Int. index address
 * @param data Pointer to struct to store data
 * @param size Struct size
 * @param payload Int. payload to get value
 * @return 0 OK, -1 Error
 */
int storage_payload_get_data(int payload, int index, void* data, size_t size);

/**
 * Delete payload databases
 *
 * @note: non-reentrant function, use mutex to sync access
 * @return OK 0, Error -1
 */
int storage_payload_reset(void);
int storage_payload_reset_table(int payload);

/**
 * Translate to sql format a c format type
 * @param c_type
 * @return string in sql syntax
 */
const char* get_sql_type(char* c_type);

//#if SCH_STORAGE_MODE == SCH_ST_SQLITE
//    void get_sqlite_value(char* c_type, void* buff, sqlite3_stmt* stmt, int j);
//#elif SCH_STORAGE_MODE == SCH_ST_POSTGRES
//    void get_psql_value(char* c_type, void* buff, PGresult *res, int j);
//#endif

#endif //SCH_STORAGE_H
