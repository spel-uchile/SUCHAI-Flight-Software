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

#include "log_utils.h"

/**
 * Struct for storing a single timed command, set to execute in the future.
 */
typedef struct __attribute__((packed)) fp_entry {
    int unixtime;               ///< Unix-time, sets when the command should next execute
    char* cmd;                  ///< Command to execute
    char* args;                 ///< Command's arguments
    int executions;             ///< Amount of times the command will be executed per periodic cycle
    int periodical;             ///< Period of time between executions
} fp_entry_t;

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
    dat_obc_temp_2,               ///< Temperature value of the second sensor
    dat_obc_temp_3,               ///< Temperature value of the gyroscope
    dat_obc_executed_cmds,        ///< Total number of executed commands
    dat_obc_failed_cmds,          ///< Total number of failed commands

    /// DEP: Deployment related variables.
    dat_dep_deployed,             ///< Was the satellite deployed?
    dat_dep_ant_deployed,         ///< Was the antenna deployed?
    dat_dep_date_time,            ///< Antenna deployment unix time

    /// RTC: Rtc related variables
    dat_rtc_date_time,            ///< RTC current unix time

    /// COM: Communications system variables.
    dat_com_count_tm,             ///< Number of Telemetries sent
    dat_com_count_tc,             ///< Number of received Telecommands
    dat_com_last_tc,              ///< Unix time of the last received Telecommand
    dat_com_freq,                 ///< Communications frequency [Hz]
    dat_com_tx_pwr,               ///< TX power (0: 25dBm, 1: 27dBm, 2: 28dBm, 3: 30dBm)
    dat_com_baud,                 ///< Baudrate [bps]
    dat_com_mode,                 ///< Framing mode (1: RAW, 2: ASM, 3: HDLC, 4: Viterbi, 5: GOLAY, 6: AX25)
    dat_com_bcn_period,           ///< Number of seconds between trx beacon packets
    dat_obc_bcn_offset,           ///< Number of seconds between obc beacon packets

    /// FPL: Flight plan related variables
    dat_fpl_last,                 ///< Last executed flight plan (unix time)
    dat_fpl_queue,                ///< Flight plan queue length

    /// ADS: Altitude determination system
    dat_ads_omega_x,              ///< Gyroscope acceleration value along the x axis
    dat_ads_omega_y,              ///< Gyroscope acceleration value along the y axis
    dat_ads_omega_z,              ///< Gyroscope acceleration value along the z axis
    dat_tgt_omega_x,              ///< Target acceleration value along the x axis
    dat_tgt_omega_y,              ///< Target acceleration value along the y axis
    dat_tgt_omega_z,              ///< Target acceleration value along the z axis
    dat_ads_mag_x,                ///< Magnetometer value along the x axis
    dat_ads_mag_y,                ///< Magnetometer value along the y axis
    dat_ads_mag_z,                ///< Magnetometer value along the z axis
    dat_ads_pos_x,                ///< Satellite orbit position x (ECI)
    dat_ads_pos_y,                ///< Satellite orbit position y (ECI)
    dat_ads_pos_z,                ///< Satellite orbit position z (ECI)
    dat_ads_tle_epoch,            ///< Current TLE epoch, 0 if TLE is invalid
    dat_ads_tle_last,             ///< Las time position was propagated
    dat_ads_q0,                   ///< Attitude quaternion (Inertial to body)
    dat_ads_q1,                   ///< Attitude quaternion (Inertial to body)
    dat_ads_q2,                   ///< Attitude quaternion (Inertial to body)
    dat_ads_q3,                   ///< Attitude quaternion (Inertial to body)
    dat_tgt_q0,                   ///< Target quaternion (Inertial to body)
    dat_tgt_q1,                   ///< Target quaternion (Inertial to body)
    dat_tgt_q2,                   ///< Target quaternion (Inertial to body)
    dat_tgt_q3,                   ///< Target quaternion (Inertial to body)

    /// EPS: Energy power system
    dat_eps_vbatt,                ///< Voltage of the battery [mV]
    dat_eps_cur_sun,              ///< Current from boost converters [mA]
    dat_eps_cur_sys,              ///< Current from the battery [mA]
    dat_eps_temp_bat0,            ///< Battery temperature sensor

    /// Memory: Current payload memory addresses
    dat_drp_temp,                 ///< Temperature data index
    dat_drp_ads,                  ///< ADS data index
    dat_drp_eps,                  ///< EPS data index
    dat_drp_sta,                 ///< Status data index

    /// Memory: Current send acknowledge data
    dat_drp_ack_temp,             ///< Temperature data acknowledge
    dat_drp_ack_ads,              ///< ADS data index acknowledge
    dat_drp_ack_eps,              ///< EPS data index acknowledge
    dat_drp_ack_sta,             ///< Status data index acknowledge

    /// Sample Machine: Current state of sample status_machine
    dat_drp_mach_action,          ///< Current action of sampling state machine
    dat_drp_mach_state,           ///< Current state of sampling state machine
    dat_drp_mach_step,            ///< Step in seconds of sampling state machine
    dat_drp_mach_payloads,        ///< Binary data storing active payload being sampled
    dat_drp_mach_left,            ///< Samples left for sampling state machine

    /// Add a new status variables address here
    //dat_custom,                 ///< Variable description

    /// LAST ELEMENT: DO NOT EDIT
    dat_status_last_address           ///< Dummy element, the amount of status variables
} dat_status_address_t;

/**
 * A 32 bit variable that can be interpreted as int, uint or float
 */
typedef union value32_u{
    int32_t i;
    uint32_t u;
    float f;
} value32_t;

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
typedef struct __attribute__((packed)) dat_sys_var {
    uint16_t address;   ///< Variable address or index (in the data storage)
    char name[24];      ///< Variable name (max 24 chars)
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
        {dat_obc_last_reset,    "obc_last_reset",    'u', DAT_IS_STATUS, -1},         ///< Last reset source
        {dat_obc_hrs_alive,     "obc_hrs_alive",     'u', DAT_IS_STATUS, 0},          ///< Hours since first boot
        {dat_obc_hrs_wo_reset,  "obc_hrs_wo_reset",  'u', DAT_IS_STATUS, 0},          ///< Hours since last reset
        {dat_obc_reset_counter, "obc_reset_counter", 'u', DAT_IS_STATUS, 0},          ///< Number of reset since first boot
        {dat_obc_sw_wdt,        "obc_sw_wdt",        'u', DAT_IS_STATUS, 0},          ///< Software watchdog timer counter
        {dat_obc_temp_1,        "obc_temp_1",        'f', DAT_IS_STATUS, -1},         ///< Temperature value of the first sensor
        {dat_obc_temp_2,        "obc_temp_2",        'f', DAT_IS_STATUS, -1},         ///< Temperature value of the second sensor
        {dat_obc_temp_3,        "obc_temp_3",        'f', DAT_IS_STATUS, -1},         ///< Temperature value of the gyroscope
        {dat_obc_executed_cmds, "obc_executed_cmds", 'u', DAT_IS_STATUS, 0},
        {dat_obc_failed_cmds,   "obc_failed_cmds",   'u', DAT_IS_STATUS, 0},
        {dat_dep_deployed,      "dep_deployed",      'u', DAT_IS_STATUS, 1},          ///< Was the satellite deployed?
        {dat_dep_ant_deployed,  "dep_ant_deployed",  'u', DAT_IS_STATUS, 1},          ///< Was the antenna deployed?
        {dat_dep_date_time,     "dep_date_time",     'u', DAT_IS_STATUS, -1},         ///< Antenna deployment unix time
        {dat_com_count_tm,      "com_count_tm",      'u', DAT_IS_STATUS, 0},          ///< Number of Telemetries sent
        {dat_com_count_tc,      "com_count_tc",      'u', DAT_IS_STATUS, 0},          ///< Number of received Telecommands
        {dat_com_last_tc,       "com_last_tc",       'u', DAT_IS_STATUS, -1},         ///< Unix time of the last received Telecommand
        {dat_fpl_last,          "fpl_last",          'u', DAT_IS_STATUS, 0},          ///< Last executed flight plan (unix time)
        {dat_fpl_queue,         "fpl_queue",         'u', DAT_IS_STATUS, 0},          ///< Flight plan queue length
        {dat_ads_omega_x,       "ads_omega_x",       'f', DAT_IS_STATUS, -1},         ///< Gyroscope acceleration value along the x axis
        {dat_ads_omega_y,       "ads_omega_y",       'f', DAT_IS_STATUS, -1},         ///< Gyroscope acceleration value along the y axis
        {dat_ads_omega_z,       "ads_omega_z",       'f', DAT_IS_STATUS, -1},         ///< Gyroscope acceleration value along the z axis
        {dat_ads_mag_x,         "ads_mag_x",         'f', DAT_IS_STATUS, -1},         ///< Magnetometer value along the x axis
        {dat_ads_mag_y,         "ads_mag_y",         'f', DAT_IS_STATUS, -1},         ///< Magnetometer value along the y axis
        {dat_ads_mag_z,         "ads_mag_z",         'f', DAT_IS_STATUS, -1},         ///< Magnetometer value along the z axis
        {dat_ads_pos_x,         "ads_pos_x",         'f', DAT_IS_STATUS, -1},         ///< Satellite orbit position x (ECI)
        {dat_ads_pos_y,         "ads_pos_y",         'f', DAT_IS_STATUS, -1},         ///< Satellite orbit position y (ECI)
        {dat_ads_pos_z,         "ads_pos_z",         'f', DAT_IS_STATUS, -1},         ///< Satellite orbit position z (ECI)
        {dat_ads_tle_epoch,     "ads_tle_epoch",     'u', DAT_IS_STATUS, -1},         ///< Current TLE epoch, 0 if TLE is invalid
        {dat_ads_tle_last,      "ads_tle_last",      'u', DAT_IS_STATUS, -1},         ///< Last time position was propagated
        {dat_ads_q0,            "ads_q0",            'f', DAT_IS_STATUS, -1},         ///< Attitude quaternion (Inertial to body)
        {dat_ads_q1,            "ads_q1",            'f', DAT_IS_STATUS, -1},         ///< Attitude quaternion (Inertial to body)
        {dat_ads_q2,            "ads_q2",            'f', DAT_IS_STATUS, -1},         ///< Attitude quaternion (Inertial to body)
        {dat_ads_q3,            "ads_q3",            'f', DAT_IS_STATUS, -1},         ///< Attitude quaternion (Inertial to body)
        {dat_eps_vbatt,         "eps_vbatt",         'u', DAT_IS_STATUS, -1},         ///< Voltage of the battery [mV]
        {dat_eps_cur_sun,       "eps_cur_sun",       'u', DAT_IS_STATUS, -1},         ///< Current from boost converters [mA]
        {dat_eps_cur_sys,       "eps_cur_sys",       'u', DAT_IS_STATUS, -1},         ///< Current from the battery [mA]
        {dat_eps_temp_bat0,     "eps_temp_bat0",     'u', DAT_IS_STATUS, -1},         ///< Battery temperature sensor
        {dat_drp_temp,          "drp_temp",          'u', DAT_IS_STATUS, 0},          ///< Temperature data index
        {dat_drp_ads,           "drp_ads",           'u', DAT_IS_STATUS, 0},          ///< ADS data index
        {dat_drp_eps,           "drp_eps",           'u', DAT_IS_STATUS, 0},          ///< EPS data index
        {dat_drp_sta,           "drp_sta",          'u', DAT_IS_STATUS, 0},          ///< Langmuir data index
        {dat_drp_mach_action,   "drp_mach_action",   'u', DAT_IS_STATUS, 0},          ///<
        {dat_drp_mach_state,    "drp_mach_state",    'u', DAT_IS_STATUS, 0},          ///<
        {dat_drp_mach_left,     "drp_mach_left",     'u', DAT_IS_STATUS, 0},          ///<
        {dat_obc_opmode,        "obc_opmode",        'i', DAT_IS_CONFIG, 0},          ///< General operation mode
        {dat_rtc_date_time,     "rtc_date_time",     'i', DAT_IS_CONFIG, 0},          ///< RTC current unix time
        {dat_com_freq,          "com_freq",          'u', DAT_IS_CONFIG, SCH_TX_FREQ},        ///< Communications frequency [Hz]
        {dat_com_tx_pwr,        "com_tx_pwr",        'u', DAT_IS_CONFIG, SCH_TX_PWR},         ///< TX power (0: 25dBm, 1: 27dBm, 2: 28dBm, 3: 30dBm)
        {dat_com_baud,          "com_baud",          'u', DAT_IS_CONFIG, SCH_TX_BAUD},        ///< Baudrate [bps]
        {dat_com_mode,          "com_mode",          'u', DAT_IS_CONFIG, 0},          ///< Framing mode (1: RAW, 2: ASM, 3: HDLC, 4: Viterbi, 5: GOLAY, 6: AX25)
        {dat_com_bcn_period,    "com_bcn_period",    'u', DAT_IS_CONFIG, SCH_TX_BCN_PERIOD},  ///< Number of seconds between trx beacon packets
        {dat_obc_bcn_offset,    "obc_bcn_offset",    'u', DAT_IS_CONFIG, SCH_OBC_BCN_OFFSET}, ///< Number of seconds between obc beacon packets
        {dat_tgt_omega_x,       "tgt_omega_x",       'f', DAT_IS_CONFIG, 0},          ///< Target acceleration value along the x axis
        {dat_tgt_omega_y,       "tgt_omega_y",       'f', DAT_IS_CONFIG, 0},          ///< Target acceleration value along the y axis
        {dat_tgt_omega_z,       "tgt_omega_z",       'f', DAT_IS_CONFIG, 0},          ///< Target acceleration value along the z axis
        {dat_tgt_q0,            "tgt_q0",            'f', DAT_IS_CONFIG, 0},          ///< Target quaternion (Inertial to body)
        {dat_tgt_q1,            "tgt_q1",            'f', DAT_IS_CONFIG, 0},          ///< Target quaternion (Inertial to body)
        {dat_tgt_q2,            "tgt_q2",            'f', DAT_IS_CONFIG, 0},          ///< Target quaternion (Inertial to body)
        {dat_tgt_q3,            "tgt_q3",            'f', DAT_IS_CONFIG, 0},          ///< Target quaternion (Inertial to body)
        {dat_drp_ack_temp,      "drp_ack_temp",      'u', DAT_IS_CONFIG, 0},          ///< Temperature data acknowledge
        {dat_drp_ack_ads,       "drp_ack_ads",       'u', DAT_IS_CONFIG, 0},          ///< ADS data index acknowledge
        {dat_drp_ack_eps,       "drp_ack_eps",       'u', DAT_IS_CONFIG, 0},          ///< EPS data index acknowledge
        {dat_drp_ack_sta,      "drp_ack_sta",        'u', DAT_IS_CONFIG, 0},          ///< Langmuir data index acknowledge
        {dat_drp_mach_step,     "drp_mach_step",     'i', DAT_IS_CONFIG, 0},          ///<
        {dat_drp_mach_payloads, "drp_mach_payloads", 'u', DAT_IS_CONFIG, 0}           ///<
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
    ads_sensors,            ///< Ads sensors
    eps_sensors,            ///< Eps sensors
    sta_sensors,            ///< Status Variables
    //custom_sensor,           ///< Add custom sensors here
    last_sensor             ///< Dummy element, the amount of payload variables
} payload_id_t;

/**
 * Struct for storing temperature data.
 */
typedef struct __attribute__((__packed__)) temp_data {
    int timestamp;
    float obc_temp_1;
    float obc_temp_2;
    float obc_temp_3;
} temp_data_t;

/**
 * Struct for storing data collected by ads sensors.
 */
typedef struct __attribute__((__packed__)) ads_data {
    int timestamp;
    float acc_x;            ///< Gyroscope acceleration value along the x axis
    float acc_y;            ///< Gyroscope acceleration value along the y axis
    float acc_z;            ///< Gyroscope acceleration value along the z axis
    float mag_x;            ///< Magnetometer x axis
    float mag_y;            ///< Magnetometer y axis
    float mag_z;            ///< Magnetometer z axis
} ads_data_t;

/**
 * Struct for storing data collected by eps housekeeping.
 */
typedef struct __attribute__((__packed__)) eps_data {
    int timestamp;
    uint32_t cursun;            ///< Current from boost converters [mA]
    uint32_t cursys;            ///< Current out of battery [mA]
    uint32_t vbatt;            ///< Voltage of battery [mV]
    int32_t temp1;
    int32_t temp2;
    int32_t temp3;
    int32_t temp4;
    int32_t temp5;
    int32_t temp6;              ///< Temperature sensors [0 = TEMP1, TEMP2, TEMP3, TEMP4, BATT0, BATT1]
} eps_data_t;


/**
 * Struct for storing data collected by status variables.
 */
typedef struct __attribute__((__packed__)) sta_data {
    uint32_t timestamp;
    dat_sys_var_short_t sta_buff[sizeof(dat_status_list) / sizeof(dat_status_list[0])];
} sta_data_t;


/**
 * Data Map Struct for data schema definition.
 */
extern struct __attribute__((__packed__)) map {
    char table[30];
    uint16_t  size;
    uint32_t sys_index;
    uint32_t sys_ack;
    char data_order[200];
    char var_names[1000];
} data_map[last_sensor];

/** The repository's name */
#define DAT_REPO_SYSTEM "dat_system"    ///< Status variables table name

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
