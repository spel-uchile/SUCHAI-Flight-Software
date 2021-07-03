/**
 * @file  dataSchema.h
 * @author Camilo Rojas M - camrojas@uchile.cl
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This header contains structs symbolizing the schema of data.
 */


#ifndef REPO_DATA_SCHEMA_H
#define REPO_DATA_SCHEMA_H

#include "suchai/log_utils.h"
#include "suchai/storage.h"

///**
// * Struct for storing a single timed command, set to execute in the future.
// */
//typedef struct __attribute__((packed)) fp_entry {
//    int unixtime;               ///< Unix-time, sets when the command should next execute
//    char* cmd;                  ///< Command to execute
//    char* args;                 ///< Command's arguments
//    int executions;             ///< Amount of times the command will be executed per periodic cycle
//    int periodical;             ///< Period of time between executions
//} fp_entry_t;

/**
 * Enum constants for dynamically identifying system status fields at execution time.
 *
 * Also permits adding new status variables cheaply, by generalizing both the
 * dat_set_system_var and dat_get_system_var functions.
 *
 * The dat_status_last_address constant serves only for comparison when looping through all
 * system status values. For example:
 *
 * @code
 * for (dat_status_address_t i = 0; i < dat_status_last_address; i++)
 * {
 * // some code using i.
 * }
 * @endcode
 *
 * @see dat_status_t
 * @seealso dat_set_system_var
 * @seealso dat_get_system_var
 */
typedef enum dat_status_address_enum {
    /// OBC: On board computer related variables.
    dat_obc_opmode = 0,           ///< General operation mode
    dat_obc_last_reset,           ///< Last reset source
    dat_obc_hrs_alive,            ///< Hours since first boot
    dat_obc_hrs_wo_reset,         ///< Hours since last reset
    dat_obc_reset_counter,        ///< Number of reset since first boot
    dat_obc_sw_wdt,               ///< Software watchdog timer counter
    dat_obc_temp_1,               ///< Temperature value of the first sensor
    dat_obc_executed_cmds,        ///< Total number of executed commands
    dat_obc_failed_cmds,          ///< Total number of failed commands

    /// RTC: Rtc related variables
    dat_rtc_date_time,            ///< RTC current unix time

    /// COM: Communications system variables.
    dat_com_count_tm,             ///< Number of Telemetries sent
    dat_com_count_tc,             ///< Number of received Telecommands
    dat_com_last_tc,              ///< Unix time of the last received Telecommand

    /// FPL: Flight plan related variables
    dat_fpl_last,                 ///< Last executed flight plan (unix time)
    dat_fpl_queue,                ///< Flight plan queue length

    /// Memory: Current payload memory addresses
    dat_drp_temp,                 ///< Temperature data index

    /// Memory: Current send acknowledge data
    dat_drp_ack_temp,             ///< Temperature data acknowledge

    /// Add a new status variables address here
    //dat_custom,                 ///< Variable description

    /// LAST ELEMENT: DO NOT EDIT
    dat_status_last_address           ///< Dummy element, the amount of status variables
} dat_status_address_t;

///**
// * A 32 bit variable that can be interpreted as int, uint or float
// */
//typedef union value32_u{
//    int32_t i;
//    uint32_t u;
//    float f;
//} value32_t;

///< Define opeartion modes
#define DAT_OBC_OPMODE_NORMAL        (0) ///< Normal operation
#define DAT_OBC_OPMODE_WARN          (1) ///< Fail safe operation
#define DAT_OBC_OPMODE_FAIL          (2) ///< Generalized fail operation
#define DAT_OBC_OPMODE_REF_POINT     (4) ///< Point to vector
#define DAT_OBC_OPMODE_NAD_POINT     (5) ///< Point to nadir
#define DAT_OBC_OPMODE_DETUMB_MAG    (6) ///< Detumbling

///< Define is a variable is config or status
#define DAT_IS_CONFIG 0
#define DAT_IS_STATUS 1

/**
 * A system variable (status or config) with an address, name, type and value
 */
#define MAX_VAR_NAME 24

typedef struct __attribute__((packed)) dat_sys_var {
    uint16_t address;   ///< Variable address or index (in the data storage)
    char name[MAX_VAR_NAME];      ///< Variable name (max 24 chars)
    char type;          ///< Variable type (u: uint, i: int, f: float)
    int8_t status;      ///< Variable is status (1), is config (0), or uninitialized (-1)
    value32_t value;    ///< Variable default value
} dat_sys_var_t;

/**
 * A system variable (status or config) with an address, and value
 * A short version to be sent as telemetry
 */
typedef struct __attribute__((packed)) dat_sys_var_short {
    uint16_t address;   ///< Variable address or index (in the data storage)
    value32_t value;    ///< Variable default value
} dat_sys_var_short_t;

/**
 * List of status variables with address, name, type and default values
 * This list is useful to decide how to store and send the status variables
 */
static const dat_sys_var_t dat_status_list[] = {
        {dat_obc_last_reset,    "obc_last_reset",    'u', DAT_IS_STATUS, 0},         ///< Last reset source
        {dat_obc_hrs_alive,     "obc_hrs_alive",     'u', DAT_IS_STATUS, 0},          ///< Hours since first boot
        {dat_obc_hrs_wo_reset,  "obc_hrs_wo_reset",  'u', DAT_IS_STATUS, 0},          ///< Hours since last reset
        {dat_obc_reset_counter, "obc_reset_counter", 'u', DAT_IS_STATUS, 0},          ///< Number of reset since first boot
        {dat_obc_sw_wdt,        "obc_sw_wdt",        'u', DAT_IS_STATUS, 0},          ///< Software watchdog timer counter
        {dat_obc_temp_1,        "obc_temp_1",        'f', DAT_IS_STATUS, -1},         ///< Temperature value of the first sensor
        {dat_obc_executed_cmds, "obc_executed_cmds", 'u', DAT_IS_STATUS, 0},
        {dat_obc_failed_cmds,   "obc_failed_cmds",   'u', DAT_IS_STATUS, 0},
        {dat_com_count_tm,      "com_count_tm",      'u', DAT_IS_STATUS, 0},          ///< Number of Telemetries sent
        {dat_com_count_tc,      "com_count_tc",      'u', DAT_IS_STATUS, 0},          ///< Number of received Telecommands
        {dat_com_last_tc,       "com_last_tc",       'u', DAT_IS_STATUS, 0},         ///< Unix time of the last received Telecommand
        {dat_fpl_last,          "fpl_last",          'u', DAT_IS_STATUS, 0},          ///< Last executed flight plan (unix time)
        {dat_fpl_queue,         "fpl_queue",         'u', DAT_IS_STATUS, 0},          ///< Flight plan queue length
        {dat_drp_temp,          "drp_temp",          'u', DAT_IS_STATUS, 0},          ///< Temperature data index
        {dat_obc_opmode,        "obc_opmode",        'd', DAT_IS_CONFIG, -1},          ///< General operation mode
        {dat_rtc_date_time,     "rtc_date_time",     'd', DAT_IS_CONFIG, -1},          ///< RTC current unix time
        {dat_drp_ack_temp,      "drp_ack_temp",      'u', DAT_IS_CONFIG, 0},          ///< Temperature data acknowledge
};
///< The dat_status_last_var constant serves for looping through all status variables
static const int dat_status_last_var = sizeof(dat_status_list) / sizeof(dat_status_list[0]);

/**
 * Enum constants for dynamically identifying payload fields at execution time.
 *
 * Also permits adding payload fields cheaply.
 *
 * The last_sensor constant serves only for comparison when looping through all
 * payload values. For example:
 *
 * @code
 * for (payload_id_t i = 0; i < last_sensor; i++)
 * {
 * // some code using i.
 * }
 * @endcode
 */

typedef enum payload_id {
    temp_sensors=0,         ///< Temperature sensors
    last_sensor             ///< Dummy element, the amount of payload variables
} payload_id_t;

/**
 * Struct for storing temperature data.
 */
typedef struct __attribute__((__packed__)) temp_data {
    uint32_t index;
    uint32_t timestamp;
    float obc_temp_1;
} temp_data_t;


/**
 * Struct for storing data collected by status variables.
 */
typedef struct __attribute__((__packed__)) sta_data {
    uint32_t index;
    uint32_t timestamp;
    uint32_t sta_buff[sizeof(dat_status_list) / sizeof(dat_status_list[0])];
} sta_data_t;

static data_map_t data_map[] = {
{"temp_data",      (uint16_t) (sizeof(temp_data_t)),dat_drp_temp,dat_drp_ack_temp, "%u %u %f",                   "sat_index timestamp obc_temp_1"},
};

/** The repository's name */
#define DAT_TABLE_STATUS "dat_status"      ///< Status variables table name
#define DAT_TABLE_DATA   "dat_data"        ///< Data storage table name
#define DAT_TABLE_FP     "dat_flightplan"  ///< Flight plan table name

/**
 * Search and return a status variable definition from dat_status_list by index or by name
 * @param address Variable index
 * @param name Variable name
 * @return dat_sys_var_t or 0 if not found.
 */
dat_sys_var_t dat_get_status_var_def(dat_status_address_t address);
dat_sys_var_t dat_get_status_var_def_name(char *name);

/**
 * Print the names and values of a system status variable list.
 * @param status Pointer to a status variables list
 */
void dat_print_system_var(dat_sys_var_t *status);

#endif //REPO_DATA_SCHEMA_H
