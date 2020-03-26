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

#include "utils.h"

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
 * The dat_system_last_var constant serves only for comparison when looping through all
 * system status values. For example:
 *
 * @code
 * for (dat_system_t i = 0; i < dat_system_last_var; i++)
 * {
 * // some code using i.
 * }
 * @endcode
 *
 * @see dat_status_t
 * @seealso dat_set_system_var
 * @seealso dat_get_system_var
 */
typedef enum dat_system {
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
    dat_com_bcn_period,           ///< Number of seconds between beacon packets

    /// FPL: Flight plan related variables
    dat_fpl_last,                 ///< Last executed flight plan (unix time)
    dat_fpl_queue,                ///< Flight plan queue length

    /// ADS: Altitude determination system
    dat_ads_acc_x,                ///< Gyroscope acceleration value along the x axis
    dat_ads_acc_y,                ///< Gyroscope acceleration value along the y axis
    dat_ads_acc_z,                ///< Gyroscope acceleration value along the z axis
    dat_ads_mag_x,                ///< Magnetometer value along the x axis
    dat_ads_mag_y,                ///< Magnetometer value along the y axis
    dat_ads_mag_z,                ///< Magnetometer value along the z axis
    dat_ads_pos_x,                ///< Satellite orbit position x (ECI)
    dat_ads_pos_y,                ///< Satellite orbit position y (ECI)
    dat_ads_pos_z,                ///< Satellite orbit position z (ECI)
    dat_ads_tle_epoch,            ///< Current TLE epoch, 0 if TLE is invalid
    dat_ads_tle_last,             ///< Las time position was propagated

    /// EPS: Energy power system
    dat_eps_vbatt,                ///< Voltage of the battery [mV]
    dat_eps_cur_sun,              ///< Current from boost converters [mA]
    dat_eps_cur_sys,              ///< Current from the battery [mA]
    dat_eps_temp_bat0,            ///< Battery temperature sensor

    /// Memory: Current payload memory addresses
    dat_drp_temp,                 ///< Temperature data index
    dat_drp_ads,                  ///< ADS data index
    dat_drp_eps,                  ///< EPS data index
    dat_drp_lang,                 ///< Langmuir data index

    /// Memory: Current send acknowledge data
    dat_drp_ack_temp,                 ///< Temperature data acknowledge
    dat_drp_ack_ads,                  ///< ADS data index acknowledge
    dat_drp_ack_eps,                  ///< EPS data index acknowledge
    dat_drp_ack_lang,                 ///< Langmuir data index acknowledge

    /// Sample Machine: Current state of sample machine
    dat_drp_mach_action,
    dat_drp_mach_state,
    dat_drp_mach_step,
    dat_drp_mach_payloads,
    dat_drp_mach_left,

    /// Add custom status variables here
    //dat_custom,                 ///< Variable description

    /// LAST ELEMENT: DO NOT EDIT
    dat_system_last_var           ///< Dummy element, the amount of status variables
} dat_system_t;

/**
 * Struct storing all system status variables.
 *
 * After adding a new field, a new enum constant of the same name must be added
 * to dat_system_t above.
 *
 * @see dat_system_t
 */
typedef struct __attribute__((packed)) dat_status_s {
    /// OBC: on board computer related variables.
    int32_t dat_obc_opmode;         ///< General operation mode
    int32_t dat_obc_last_reset;     ///< Last reset source
    int32_t dat_obc_hrs_alive;      ///< Hours since first boot
    int32_t dat_obc_hrs_wo_reset;   ///< Hours since last reset
    int32_t dat_obc_reset_counter;  ///< Number of reset since first boot
    int32_t dat_obc_sw_wdt;         ///< Software watchdog timer counter
    float dat_obc_temp_1;           ///< Temperature value of the first sensor
    float dat_obc_temp_2;           ///< Temperature value of the second sensor
    float dat_obc_temp_3;           ///< Temperature value of the gyroscope

    /// DEP: deployment related variables.
    int32_t dat_dep_deployed;       ///< Was the satellite deployed?
    int32_t dat_dep_ant_deployed;   ///< Was the antenna deployed?
    int32_t dat_dep_date_time;      ///< Antenna deployment unix time

    /// RTC: related variables
    int32_t dat_rtc_date_time;      ///< RTC current unix time

    /// COM: communications system variables.
    int32_t dat_com_count_tm;       ///< Number of Telemetries sent
    int32_t dat_com_count_tc;       ///< Number of received Telecommands
    int32_t dat_com_last_tc;        ///< Unix time of the last received Telecommand
    uint32_t dat_com_freq;          ///< Communications frequency [Hz]
    uint32_t dat_com_tx_pwr;        ///< TX power (0: 25dBm, 1: 27dBm, 2: 28dBm, 3: 30dBm)
    uint32_t dat_com_baud;          ///< Baudrate [bps]
    uint32_t dat_com_mode;          ///< Framing mode (1: RAW, 2: ASM, 3: HDLC, 4: Viterbi, 5: GOLAY, 6: AX25)
    uint32_t dat_com_bcn_period;    ///< Number of seconds between beacon packets

    /// FPL: flight plant related variables
    int32_t dat_fpl_last;           ///< Last executed flight plan (unix time)
    int32_t dat_fpl_queue;          ///< Flight plan queue length

    /// ADS: Attitude determination system
    float dat_ads_acc_x;            ///< Gyroscope acceleration value along the x axis
    float dat_ads_acc_y;            ///< Gyroscope acceleration value along the y axis
    float dat_ads_acc_z;            ///< Gyroscope acceleration value along the z axis
    float dat_ads_mag_x;            ///< Magnetometer value along the x axis
    float dat_ads_mag_y;            ///< Magnetometer value along the y axis
    float dat_ads_mag_z;            ///< Magnetometer value along the z axis
    float dat_ads_pos_x;            ///< Satellite orbit position x (ECI)
    float dat_ads_pos_y;            ///< Satellite orbit position y (ECI)
    float dat_ads_pos_z;            ///< Satellite orbit position z (ECI)
    int32_t dat_ads_tle_epoch;      ///< Current TLE epoch, 0 if TLE is invalid
    int32_t dat_ads_tle_last;       ///< Las time position was propagated

    /// EPS: Energy power system
    uint32_t dat_eps_vbatt;         ///< Voltage of battery [mV]
    uint32_t dat_eps_cur_sun;       ///< Current from boost converters [mA]
    uint32_t dat_eps_cur_sys;       ///< Current out of battery [mA]
    uint32_t dat_eps_temp_bat0;     ///< Battery temperature sensor

    /// Memory: Current payload memory address
    uint32_t dat_drp_temp;          ///< Temperature data index
    uint32_t dat_drp_ads;           ///< ADS data index
    uint32_t dat_drp_eps;           ///< EPS data index
    uint32_t dat_drp_lang;          ///< Langmuir data index

    /// Memory: Current send acknowledge data
    uint32_t dat_drp_ack_temp;      ///< Temperature data acknowledge
    uint32_t dat_drp_ack_ads;       ///< ADS data index acknowledge
    uint32_t dat_drp_ack_eps;       ///< EPS data index acknowledge
    uint32_t dat_drp_ack_lang;      ///< Langmuir data index acknowledge

    /// Sample Machine:
    uint32_t dat_drp_mach_action;
    uint32_t dat_drp_mach_state;
    int32_t dat_drp_mach_step;
    uint32_t dat_drp_mach_payloads;
    int32_t dat_drp_mach_left;

    /// Add custom status variables here
    //uint32_t dat_custom;          ///< Variable description

    /// LAST ELEMENT: DO NOT EDIT
    int32_t dat_system_last_var;    ///< Dummy element
} dat_status_t;

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
    lang_sensors,           ///< Langmuir probe sensors
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
 * Struct for storing data collected by langmuir probe payload.
 */
typedef struct __attribute__((__packed__)) langmuir_data {
    int timestamp;
    float sweep_voltage;
    float plasma_voltage;
    float plasma_temperature;
    int particles_counter;
} langmuir_data_t;


/**
 * Data Map Struct for data schema definition.
 */
extern struct __attribute__((__packed__)) map {
    char table[30];
    uint16_t  size;
    uint32_t sys_index;
    uint32_t sys_ack;
    char data_order[50];
    char var_names[200];
} data_map[last_sensor];

/** Union for easily casting status variable types */
typedef union fvalue{
    float f;
    int32_t i;
} fvalue_t;

#define DAT_OBC_OPMODE_NORMAL   (0) ///< Normal operation
#define DAT_OBC_OPMODE_WARN     (1) ///< Fail safe operation
#define DAT_OBC_OPMODE_FAIL     (2) ///< Generalized fail operation

/** The repository's name */
#define DAT_REPO_SYSTEM "dat_system"    ///< Status variables table name

/** Copy a system @var to a status struct @st */
#define DAT_CPY_SYSTEM_VAR(st, var) st->var = dat_get_system_var(var)

/** Copy a float system @var to a status struct @st */
#define DAT_CPY_SYSTEM_VAR_F(st, var) {fvalue_t v; v.i = (float)dat_get_system_var(var); st->var = v.f;}

/** Print the name and value of a integer system status variable */
#define DAT_PRINT_SYSTEM_VAR(st, var) printf("\t%s: %lu\n", #var, (unsigned long)st->var)

/** Print the name and vale of a float system status variable */
#define DAT_PRINT_SYSTEM_VAR_F(st, var) printf("\t%s: %f\n", #var, st->var)

/**
 * Copies the status repository's field values to another dat_status_t struct.
 *
 * This function can be useful for debugging status fields with @c dat_print_status .
 *
 * And for packing the fields prior to sending them using libcsp in @c tm_send_status .
 *
 * @see dat_print_status
 * @see tm_send_status
 *
 * @param status dat_status_t *. Pointer to destination structure
 */
void dat_status_to_struct(dat_status_t *status);

/**
 * Print the names and values of a system status struct's fields.
 *
 * @seealso dat_status_to_struct
 *
 * @param status Pointer to a status variables struct
 */
void dat_print_status(dat_status_t *status);

#endif //REPO_DATA_SCHEMA_H
