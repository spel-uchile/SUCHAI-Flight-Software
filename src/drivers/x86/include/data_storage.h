/**
 * @file data_storage.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Camilo Rojas M - camrojas@uchile.cl
 * @date 2020
 * @copyright GNU GPL v3
 *
 * Persistent data storage functions.
 */

#ifndef SCH_PERSISTENT_H
#define SCH_PERSISTENT_H

#include "utils.h"
#include <stdio.h>
#include "config.h"
#include "repoData.h"

#if SCH_STORAGE_MODE == 1
    #include <sqlite3.h>
#elif SCH_STORAGE_MODE == 2
    #include <libpq-fe.h>
#endif

/**
 * Init data storage system.
 * In this case we use SQLite, so this function open a database in file
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param file Str. File path to SQLite database
 * @return 0 OK, -1 Error
 */
int storage_init(const char *file);

/**
 * Create new table in the opened database (@relatesalso storage_init) in the
 * form (index, name, value). If the table exists do nothing. If drop is set to
 * 1 then drop an existing table and then creates an empty one.
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param table Str. Table name
 * @param drop Int. Set to 1 to drop the existing table before create one
 * @return 0 OK, -1 Error
 */
int storage_table_repo_init(char *table, int drop);

/**
 * Create new table in the opened database (@relatesalso storage_init) in the
 * form (time, command, args, repeat). If the table exists do nothing. If drop is set to
 * 1 then drop an existing table and then creates an empty one.
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param drop Int. Set to 1 to drop the existing table before create one
 * @return 0 OK, -1 Error
 */
int storage_table_flight_plan_init(int drop);

/**
 * Create new table in the opened database (@relatesalso storage_init)
 * for a payload. If the table exists do nothing. If drop is set to
 * 1 then drop an existing table and then creates an empty one.
 *
 * @note: NOT IMPLEMENTED
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param drop Int. Set to 1 to drop the existing table before create one
 * @return 0 OK, -1 Error
 */
int storage_table_payload_init(int drop);

/**
 * Get an INT (integer) value from table by index
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param index Int. Value index
 * @param table Str. Table name
 * @return 0 OK, -1 Error
 */
int storage_repo_get_value_idx(int index, char *table);

/**
 * Get a INT (integer) value from table by name
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param name Str. Value name
 * @param table Str. Table name
 * @return 0 OK, -1 Error
 */
int storage_repo_get_value_str(char *name, char *table);

/**
 * Set or update the value of a INT (integer) variable by index.
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param index Int. Variable index
 * @param value Int. Value to set
 * @param table Str. Table name
 * @return 0 OK, -1 Error
 */
int storage_repo_set_value_idx(int index, int value, char *table);

/**
 * Set or update the row of a certain time
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param timetodo Int. time to do the action
 * @param command Str. Command to set
 * @param args Str. command's arguments
 * @param repeat Int. Value of time to run the command
 * @return 0 OK, -1 Error
 */
int storage_flight_plan_set(int timetodo, char* command, char* args, int repeat, int periodical);

/**
 * Get the row of a certain time and set the values in the variables committed
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param timetodo Int. time to do the action
 * @param command Str. Command to get
 * @param args Str. command's arguments
 * @param repeat Int. Value of times to run the command
 * @return 0 OK, -1 Error
 */
int storage_flight_plan_get(int timetodo, char* command, char* args, int* repeat, int* periodical);

/**
 * Erase the row in the table in the opened database (@relatesalso storage_init) that
 * have the same timetodo.
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param timetodo Int. time to do the action
 * @return 0 OK, -1 Error
 */
int storage_flight_plan_erase(int timetodo);

/**
 * Reset the table in the opened database (@relatesalso storage_init) in the
 * form (time, command, args, repeat).
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @return 0 OK, -1 Error
 */
int storage_flight_plan_reset(void);

/**
 * Show the table in the opened database (@relatesalso storage_init) in the
 * form (time, command, args, repeat).
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @return 0 OK
 */
int storage_flight_plan_show_table(void);

/**
 * Set a value for specific payload with index value
 * in database
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param index Int. index address
 * @param data Pointer to struct
 * @param payload Int. payload to store
 * @return 0 OK, -1 Error
 */
int storage_set_payload_data(int index, void * data, int payload);

/**
 * Get a value for specific payload with index value
 * in database
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param index Int. index address
 * @param data Pointer to struct
 * @param payload Int. payload to get value
 * @return 0 OK, -1 Error
 */
int storage_get_payload_data(int index, void* data, int payload);

/**
 * Close the opened database
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @return 0 OK, -1 Error
 */
int storage_close(void);

/**
 * Translate to sql format a c format type
 * @param c_type
 * @return string in sql syntax
 */
const char* get_sql_type(char* c_type);

#if SCH_STORAGE_MODE == 1
    void get_sqlite_value(char* c_type, void* buff, sqlite3_stmt* stmt, int j);
#elif SCH_STORAGE_MODE == 2
    void get_psql_value(char* c_type, void* buff, PGresult *res, int j);
#endif

// TODO: Remove not used function?
/**
// * Set or update the value of a INT (integer) variable by name.
// *
// * @note: non-reentrant function, use mutex to sync access
// *
// * @param name Str. Variable name
// * @param value Int. Value to set
// * @param table Str. Table name
// * @return 0 OK, -1 Error
// */
//int storage_repo_set_value_str(char *name, int value, char *table);

#endif //SCH_PERSISTENT_H
