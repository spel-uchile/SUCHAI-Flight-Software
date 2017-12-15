//
// Created by carlos on 22-08-17.
//

#ifndef SCH_PERSISTENT_H
#define SCH_PERSISTENT_H

#include "utils.h"
#include <stdio.h>
#include <sqlite3.h>



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
 * @param table Str. Table name
 * @param drop Int. Set to 1 to drop the existing table before create one
 * @return 0 OK, -1 Error
 */
int storage_table_flight_plan_init(char* table, int drop);

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
 * Set or update the value of a INT (integer) variable by name.
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param name Str. Variable name
 * @param value Int. Value to set
 * @param table Str. Table name
 * @return 0 OK, -1 Error
 */
int storage_repo_set_value_str(char *name, int value, char *table);

/**
 * Set or update the row of certain time
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param timetodo Int. time to do the action
 * @param command Str. Command to set
 * @param args Str. command's arguments
 * @param repeat Int. Value of time to run the command
 * @param table Str. Table name
 * @return 0 OK, -1 Error
 */
int storage_flight_plan_set(int timetodo, char* command, char* args, int repeat, char* table);

int storage_flight_plan_set_time(int timetodo, char* table);

/**
 * Get a String that contains the command from table flight plan by timetodo
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param timetodo Int. the time of action
 * @param table Str. Table name
 * @return String, NULL Error
 */
const unsigned char* storage_flight_plan_get_command(int timetodo, char* table);

/**
 * Get a String that constains the command's arguments from table flight plan by timetodo
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param timetodo Int. the time of action
 * @param table Str. Table name
 * @return String, NULL Error
 */
const unsigned char* storage_flight_plan_get_args(int timetodo, char* table);

/**
 * Get a Integer that constains the times to execute the command from table flight plan by timetodo
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param timetodo Int. the time of action
 * @param table Str. Table name
 * @return Integer, NULL Error
 */
int storage_flight_plan_get_repeat(int timetodo, char* table);


/**
 * Erase the row in the table in the opened database (@relatesalso storage_init) that
 * have the same timetodo.
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param timetodo Int. time to do the action
 * @param table Str. Table name
 * @return 0 OK, -1 Error
 */
int storage_flight_plan_erase (int timetodo, char* table);

/**
 * Reset the table in the opened database (@relatesalso storage_init) in the
 * form (time, command, args, repeat).
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param table Str. Table name
 * @return 0 OK, -1 Error
 */
int storage_flight_plan_reset (char* table);

/**
 * Create the table in the opened database (@relatesalso storage_init) in the
 * form (time, command, args, repeat) and insert all the times with NULL atributes
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param table Str. Table name
 */
void storage_table_flight_plan_init_times(int timeinit, char* table);

/**
 * Close the opened database
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @return 0 OK, -1 Error
 */
int storage_close(void);

#endif //SCH_PERSISTENT_H
