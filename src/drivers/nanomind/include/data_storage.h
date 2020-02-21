//
// Created by carlos on 22-08-17.
//

#ifndef SCH_PERSISTENT_H
#define SCH_PERSISTENT_H

#include <stdio.h>
#include <stdint.h>
#include "drivers.h"
#include "utils.h"
#include "config.h"
#include "globals.h"
#include "repoData.h"

/**
 * This union represents a status variable stored in the FM33256B FRAM. Status
 * variables are casted to uint32_t and wrote as 4 uint8_t values in the FRAM.
 * One status variable index is mapped to 4 addresses in the FRAM.
 */
typedef union _data32{
    uint8_t data8_p[4];     ///< Variable as 4 uint8_t array
    uint32_t data32;        ///< Variable as 1 uint32_t integer
}data32_t;


/**
 * Init data storage system.
 * In this case we use the FM33256B FRAM
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
 * @note: NOT IMPLEMENTED
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
 * Get an INT (integer) value from table by index. In the FM33256B data is
 * stored as uint8_t values, so we read 4 uint8_t values to form a uint32_t
 * integer returned as int. The @index is mapped to @index*4 inside FM33256B
 * FRAM
 *
 * @note: NOT IMPLEMENTED
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
 * @note: NOT IMPLEMENTED
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param name Str. Value name
 * @param table Str. Table name
 * @return 0 OK, -1 Error
 */
int storage_repo_get_value_str(char *name, char *table);

/**
 * Set or update the value of a INT (integer) variable by index. In the FM33256B
 * data is stored as uint8_t values, so we cast @value to a uint32_t integer and
 * write 4 uint8_t values. The @index is mapped to @index*4 inside FM33256B FRAM
 *
 * @note: NOT IMPLEMENTED
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
 * @note: NOT IMPLEMENTED
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param name Str. Variable name
 * @param value Int. Value to set
 * @param table Str. Table name
 * @return 0 OK, -1 Error
 */
int storage_repo_set_value_str(char *name, int value, char *table);

/**
 * Add a new entry to the end of the flight plan table, set to execute at a certain time.
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
 * Get the first entry in the flight plan table that's set to execute at the given time.
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
 * Erase the first entry in the flight plan table that's set to execute at the given time.
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param timetodo Int. time to do the action
 * @return 0 OK, -1 Error
 */
int storage_flight_plan_erase(int timetodo);

/**
 * Reset the flight plan table.
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @return 0 OK, -1 Error
 */
int storage_flight_plan_reset(void);

/**
 * Show the flight plan table, printing all values in the
 * form (time, command, args, repeat).
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @return 0 OK
 */
int storage_flight_plan_show_table(void);

// TODO: Check why this function isn't in Linux/include/data_storage.h
/**
 * Set or update a value in index address for specific payload
 * in NOR FLASH
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param index Int. index address in NOR FLASH
 * @param data Pointer to struct
 * @param payload Int. payload to store
 * @return 0 OK, -1 Error
 */
int storage_set_payload_data(int index, void * data, int payload);

/**
 * Add data struct to payload table
 *
 * @param data Pointer to struct
 * @param payload Int, payload to store
 * @return 0 OK, -1 Error
 */
//int storage_add_payload_data(void * data, int payload, int lastindex);

// TODO: Check why this function isn't in Linux/include/data_storage.h
/**
 * Get a value from index address for specific payload
 * in NOR FLASH
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param index Int. index address in NOR FLASH
 * @param data Pointer to struct
 * @param payload Int. payload to get value
 * @return 0 OK, -1 Error
 */
int storage_get_payload_data(int index, void* data, int payload);

/**
 * Get recent values from for specific payload
 * in NOR FLASH
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @param data Pointer to struct
 * @param payload Int. payload to get value
 * @param delay Int, delay from recent value
 * @return OK 0, Error -1
 */
//int storage_get_recent_payload_data(void* data, int payload, int delay);

/**
 * Delete all memory sections in NOR FLASH
 *
 * @note: non-reentrant function, use mutex to sync access
 * @return OK 0, Error -1
 */
int storage_delete_memory_sections(void);

/**
 * Close the opened database.
 *
 * Free all dynamic variables related to storage.
 *
 * @note: non-reentrant function, use mutex to sync access
 *
 * @return 0 OK, -1 Error
 */
int storage_close(void);

/**
 * FIXME: IMPLEMENT
 * @param drop
 * @return
 */
int storage_table_payload_init(int drop);



#endif //SCH_PERSISTENT_H
