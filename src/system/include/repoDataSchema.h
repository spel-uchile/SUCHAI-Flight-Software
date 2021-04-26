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
 * A 32 bit variable that can be interpreted as int, uint or float
 */
typedef union value32_u{
    int32_t i;
    uint32_t u;
    float f;
} value32_t;

/**
 * A system variable (status or config) with a name, type and value
 */
typedef struct __attribute__((packed)) dat_sys_var {
    char name[25];      ///< Variable name (max 25 chars)
    char type;          ///< Variable type (u: uint, i: int, f: float)
    value32_t value;    ///< Variable value
} dat_sys_var_t;

/**
 * List of status variables
 * Status variables are updated by the software itself not by ground operators
 */
#define DAT_OBC_LAST_RESET "obc_last_reset"
#define DAT_OBC_HRS_ALIVE "obc_hrs_alive"
#define DAT_OBC_HRS_WO_RESET "obc_hrs_wo_reset"
#define DAT_OBC_RESET_COUNTER "obc_reset_counter"
#define DAT_OBC_SW_WDT "obc_sw_wdt"
#define DAT_OBC_TEMP_1 "obc_temp_1"
#define DAT_OBC_TEMP_2 "obc_temp_2"
#define DAT_OBC_TEMP_3 "obc_temp_3"
#define DAT_DEP_DEPLOYED "dep_deployed"
#define DAT_DEP_ANT_DEPLOYED "dep_ant_deployed"
#define DAT_DEP_DATE_TIME "dep_date_time"
#define DAT_COM_COUNT_TM "com_count_tm"
#define DAT_COM_COUNT_TC "com_count_tc"
#define DAT_COM_LAST_TC "com_last_tc"
#define DAT_FPL_LAST "fpl_last"
#define DAT_FPL_QUEUE "fpl_queue"
#define DAT_ADS_OMEGA_X "ads_omega_x"
#define DAT_ADS_OMEGA_Y "ads_omega_y"
#define DAT_ADS_OMEGA_Z "ads_omega_z"
#define DAT_ADS_MAG_X "ads_mag_x"
#define DAT_ADS_MAG_Y "ads_mag_y"
#define DAT_ADS_MAG_Z "ads_mag_z"
#define DAT_ADS_POS_X "ads_pos_x"
#define DAT_ADS_POS_Y "ads_pos_y"
#define DAT_ADS_POS_Z "ads_pos_z"
#define DAT_ADS_TLE_EPOCH "ads_tle_epoch"
#define DAT_ADS_TLE_LAST "ads_tle_last"
#define DAT_ADS_Q_0 "ads_q0"
#define DAT_ADS_Q_1 "ads_q1"
#define DAT_ADS_Q_2 "ads_q2"
#define DAT_ADS_Q_3 "ads_q3"
#define DAT_EPS_VBATT "eps_vbatt"
#define DAT_EPS_CUR_SUN "eps_cur_sun"
#define DAT_EPS_CUR_SYS "eps_cur_sys"
#define DAT_EPS_TEMP_BAT_0 "eps_temp_bat0"
#define DAT_DRP_TEMP "drp_temp"
#define DAT_DRP_ADS "drp_ads"
#define DAT_DRP_EPS "drp_eps"
#define DAT_DRP_LANG "drp_lang"
#define DAT_DRP_MACH_ACTION "drp_mach_action"
#define DAT_DRP_MACH_STATE "drp_mach_state"
#define DAT_DRP_MACH_LEFT "drp_mach_left"

static const dat_sys_var_t dat_status_list[] = {
        {DAT_OBC_LAST_RESET, 'u', -1},      ///< Last reset source
        {DAT_OBC_HRS_ALIVE, 'u', 0},       ///< Hours since first boot
        {DAT_OBC_HRS_WO_RESET, 'u', 0},    ///< Hours since last reset
        {DAT_OBC_RESET_COUNTER, 'u', 0},   ///< Number of reset since first boot
        {DAT_OBC_SW_WDT, 'u', 0},          ///< Software watchdog timer counter
        {DAT_OBC_TEMP_1, 'f', -1},          ///< Temperature value of the first sensor
        {DAT_OBC_TEMP_2, 'f', -1},          ///< Temperature value of the second sensor
        {DAT_OBC_TEMP_3, 'f', -1},          ///< Temperature value of the gyroscope
        {DAT_DEP_DEPLOYED, 'u', 1},        ///< Was the satellite deployed?
        {DAT_DEP_ANT_DEPLOYED, 'u', 1},    ///< Was the antenna deployed?
        {DAT_DEP_DATE_TIME, 'u', -1},       ///< Antenna deployment unix time
        {DAT_COM_COUNT_TM, 'u', 0},        ///< Number of Telemetries sent
        {DAT_COM_COUNT_TC, 'u', 0},        ///< Number of received Telecommands
        {DAT_COM_LAST_TC, 'u', -1},         ///< Unix time of the last received Telecommand
        {DAT_FPL_LAST, 'u', 0},            ///< Last executed flight plan (unix time)
        {DAT_FPL_QUEUE, 'u', 0},           ///< Flight plan queue length
        {DAT_ADS_OMEGA_X, 'f', -1},         ///< Gyroscope acceleration value along the x axis
        {DAT_ADS_OMEGA_Y, 'f', -1},         ///< Gyroscope acceleration value along the y axis
        {DAT_ADS_OMEGA_Z, 'f', -1},         ///< Gyroscope acceleration value along the z axis
        {DAT_ADS_MAG_X, 'f', -1},           ///< Magnetometer value along the x axis
        {DAT_ADS_MAG_Y, 'f', -1},           ///< Magnetometer value along the y axis
        {DAT_ADS_MAG_Z, 'f', -1},           ///< Magnetometer value along the z axis
        {DAT_ADS_POS_X, 'f', -1},           ///< Satellite orbit position x (ECI)
        {DAT_ADS_POS_Y, 'f', -1},           ///< Satellite orbit position y (ECI)
        {DAT_ADS_POS_Z, 'f', -1},           ///< Satellite orbit position z (ECI)
        {DAT_ADS_TLE_EPOCH, 'u', -1},       ///< Current TLE epoch, 0 if TLE is invalid
        {DAT_ADS_TLE_LAST, 'u', -1},        ///< Last time position was propagated
        {DAT_ADS_Q_0, 'f', -1},             ///< Attitude quaternion (Inertial to body)
        {DAT_ADS_Q_1, 'f', -1},             ///< Attitude quaternion (Inertial to body)
        {DAT_ADS_Q_2, 'f', -1},             ///< Attitude quaternion (Inertial to body)
        {DAT_ADS_Q_3, 'f', -1},             ///< Attitude quaternion (Inertial to body)
        {DAT_EPS_VBATT, 'u', -1},           ///< Voltage of the battery [mV]
        {DAT_EPS_CUR_SUN, 'u', -1},         ///< Current from boost converters [mA]
        {DAT_EPS_CUR_SYS, 'u', -1},         ///< Current from the battery [mA]
        {DAT_EPS_TEMP_BAT_0, 'u', -1},      ///< Battery temperature sensor
        {DAT_DRP_TEMP, 'u', 0},            ///< Temperature data index
        {DAT_DRP_ADS, 'u', 0},             ///< ADS data index
        {DAT_DRP_EPS, 'u', 0},             ///< EPS data index
        {DAT_DRP_LANG, 'u', 0},            ///< Langmuir data index
        {DAT_DRP_MACH_ACTION, 'u', 0},     ///<
        {DAT_DRP_MACH_STATE, 'u', 0},      ///<
        {DAT_DRP_MACH_LEFT, 'u', 0},       ///<
};
///< The dat_status_last_var constant serves for looping through all status variables
static const int dat_status_last_var = sizeof(dat_status_list) / sizeof(dat_status_list[0]);

/**
 * List of configuration variables
 * Configuration variables are updated by ground operators
 */
#define DAT_OBC_OP_MODE "obc_op_mode"
#define DAT_RTC_DATE_TIME "rtc_date_time"
#define DAT_COM_FREQ "com_freq"
#define DAT_COM_TX_PWR "com_tx_pwr"
#define DAT_COM_BAUD "com_baud"
#define DAT_COM_MODE "com_mode"
#define DAT_COM_BCN_PERIOD "com_bcn_period"
#define DAT_OBC_BCN_OFFSET "obc_bcn_offset"
#define DAT_TGT_OMEGA_X "tgt_omega_x"
#define DAT_TGT_OMEGA_Y "tgt_omega_y"
#define DAT_TGT_OMEGA_Z "tgt_omega_z"
#define DAT_TGT_Q_0 "tgt_q0"
#define DAT_TGT_Q_1 "tgt_q1"
#define DAT_TGT_Q_2 "tgt_q2"
#define DAT_TGT_Q_3 "tgt_q3"
#define DAT_DRP_ACK_TEMP "drp_ack_temp"
#define DAT_DRP_ACK_ADS "drp_ack_ads"
#define DAT_DRP_ACK_EPS "drp_ack_eps"
#define DAT_DRP_ACK_LANG "drp_ack_lang"
#define DAT_DRP_MACH_STEP "drp_mach_step"
#define DAT_DRP_MACH_PAYLOADS "drp_mach_payloads"

static const dat_sys_var_t dat_config_list[] = {
        {DAT_OBC_OP_MODE, 'i', 0},         ///< General operation mode
        {DAT_RTC_DATE_TIME, 'i', 0},       ///< RTC current unix time
        {DAT_COM_FREQ, 'u', SCH_TX_FREQ},            ///< Communications frequency [Hz]
        {DAT_COM_TX_PWR, 'u', SCH_TX_PWR},          ///< TX power (0: 25dBm, 1: 27dBm, 2: 28dBm, 3: 30dBm)
        {DAT_COM_BAUD, 'u', SCH_TX_BAUD},            ///< Baudrate [bps]
        {DAT_COM_MODE, 'u', 0},            ///< Framing mode (1: RAW, 2: ASM, 3: HDLC, 4: Viterbi, 5: GOLAY, 6: AX25)
        {DAT_COM_BCN_PERIOD, 'u', SCH_TX_BCN_PERIOD},      ///< Number of seconds between trx beacon packets
        {DAT_OBC_BCN_OFFSET, 'u', SCH_OBC_BCN_OFFSET},      ///< Number of seconds between obc beacon packets
        {DAT_TGT_OMEGA_X, 'f', 0},         ///< Target acceleration value along the x axis
        {DAT_TGT_OMEGA_Y, 'f', 0},         ///< Target acceleration value along the y axis
        {DAT_TGT_OMEGA_Z, 'f', 0},         ///< Target acceleration value along the z axis
        {DAT_TGT_Q_0, 'f', 0},             ///< Target quaternion (Inertial to body)
        {DAT_TGT_Q_1, 'f', 0},             ///< Target quaternion (Inertial to body)
        {DAT_TGT_Q_2, 'f', 0},             ///< Target quaternion (Inertial to body)
        {DAT_TGT_Q_3, 'f', 0},             ///< Target quaternion (Inertial to body)
        {DAT_DRP_ACK_TEMP, 'u', 0},        ///< Temperature data acknowledge
        {DAT_DRP_ACK_ADS, 'u', 0},         ///< ADS data index acknowledge
        {DAT_DRP_ACK_EPS, 'u', 0},         ///< EPS data index acknowledge
        {DAT_DRP_ACK_LANG, 'u', 0},        ///< Langmuir data index acknowledge
        {DAT_DRP_MACH_STEP, 'i', 0},       ///<
        {DAT_DRP_MACH_PAYLOADS, 'u', 0},   ///<
};
///< The dat_config_last_var constant serves for looping through all status variables
static const int dat_config_last_var = sizeof(dat_config_list) / sizeof(dat_config_list[0]);

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
    dat_com_bcn_period,           ///< Number of seconds between trx beacon packets
    dat_obc_bcn_offset,           ///< Number of seconds between obc beacon packets

    /// FPL: Flight plan related variables
    dat_fpl_last,                 ///< Last executed flight plan (unix time)
    dat_fpl_queue,                ///< Flight plan queue length

    /// ADS: Altitude determination system
    dat_ads_omega_x,              ///< Gyroscope acceleration value along the x axis
    dat_ads_omega_y,              ///< Gyroscope acceleration value along the y axis
    dat_ads_omega_z,              ///< Gyroscope acceleration value along the z axis
    dat_tgt_omega_x,                ///< Target acceleration value along the x axis
    dat_tgt_omega_y,                ///< Target acceleration value along the y axis
    dat_tgt_omega_z,                ///< Target acceleration value along the z axis
    dat_ads_mag_x,                ///< Magnetometer value along the x axis
    dat_ads_mag_y,                ///< Magnetometer value along the y axis
    dat_ads_mag_z,                ///< Magnetometer value along the z axis
    dat_ads_pos_x,                ///< Satellite orbit position x (ECI)
    dat_ads_pos_y,                ///< Satellite orbit position y (ECI)
    dat_ads_pos_z,                ///< Satellite orbit position z (ECI)
    dat_ads_tle_epoch,            ///< Current TLE epoch, 0 if TLE is invalid
    dat_ads_tle_last,             ///< Las time position was propagated
    dat_ads_q0,                  ///< Attitude quaternion (Inertial to body)
    dat_ads_q1,                  ///< Attitude quaternion (Inertial to body)
    dat_ads_q2,                  ///< Attitude quaternion (Inertial to body)
    dat_ads_q3,                  ///< Attitude quaternion (Inertial to body)
    dat_tgt_q0,                  ///< Target quaternion (Inertial to body)
    dat_tgt_q1,                  ///< Target quaternion (Inertial to body)
    dat_tgt_q2,                  ///< Target quaternion (Inertial to body)
    dat_tgt_q3,                  ///< Target quaternion (Inertial to body)

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
    uint32_t dat_obc_bcn_offset;    ///< Number of seconds between obc beacon packets

    /// FPL: flight plant related variables
    int32_t dat_fpl_last;           ///< Last executed flight plan (unix time)
    int32_t dat_fpl_queue;          ///< Flight plan queue length

    /// ADS: Attitude determination system
    float dat_ads_omega_x;            ///< Gyroscope acceleration value along the x axis
    float dat_ads_omega_y;            ///< Gyroscope acceleration value along the y axis
    float dat_ads_omega_z;            ///< Gyroscope acceleration value along the z axis
    float dat_tgt_omega_x;            ///< Target acceleration value along the x axis
    float dat_tgt_omega_y;            ///< Target acceleration value along the y axis
    float dat_tgt_omega_z;            ///< Target acceleration value along the z axis
    float dat_ads_mag_x;            ///< Magnetometer value along the x axis
    float dat_ads_mag_y;            ///< Magnetometer value along the y axis
    float dat_ads_mag_z;            ///< Magnetometer value along the z axis
    float dat_ads_pos_x;            ///< Satellite orbit position x (ECI)
    float dat_ads_pos_y;            ///< Satellite orbit position y (ECI)
    float dat_ads_pos_z;            ///< Satellite orbit position z (ECI)
    int32_t dat_ads_tle_epoch;      ///< Current TLE epoch, 0 if TLE is invalid
    int32_t  dat_ads_tle_last;      ///< Las time position was propagated
    float dat_ads_q0;               ///< Attitude quaternion (Inertial to body)
    float dat_ads_q1;               ///< Attitude quaternion (Inertial to body)
    float dat_ads_q2;               ///< Attitude quaternion (Inertial to body)
    float dat_ads_q3;               ///< Attitude quaternion (Inertial to body)
    float dat_tgt_q0;               ///< Target quaternion (Inertial to body)
    float dat_tgt_q1;               ///< Target quaternion (Inertial to body)
    float dat_tgt_q2;               ///< Target quaternion (Inertial to body)
    float dat_tgt_q3;               ///< Target quaternion (Inertial to body)

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

#define DAT_OBC_OPMODE_NORMAL        (0) ///< Normal operation
#define DAT_OBC_OPMODE_WARN          (1) ///< Fail safe operation
#define DAT_OBC_OPMODE_FAIL          (2) ///< Generalized fail operation
#define DAT_OBC_OPMODE_REF_POINT     (4) ///< Point to vector
#define DAT_OBC_OPMODE_NAD_POINT     (5) ///< Point to nadir
#define DAT_OBC_OPMODE_DETUMB_MAG    (6) ///< Detumbling

/** The repository's name */
#define DAT_REPO_SYSTEM "dat_system"    ///< Status variables table name

/** Copy a system @var to a status struct @st */
#define DAT_CPY_SYSTEM_VAR(st, var) st->var = dat_get_system_var(var)

/** Copy a float system @var to a status struct @st */
#define DAT_CPY_SYSTEM_VAR_F(st, var) {fvalue_t v; v.i = (float)dat_get_system_var(var); st->var = v.f;}

/** Print the name and value of a integer system status variable */
#define DAT_PRINT_SYSTEM_VAR(st, var) printf("\t%s: %lu\n\r", #var, (unsigned long)st->var)

/** Print the name and vale of a float system status variable */
#define DAT_PRINT_SYSTEM_VAR_F(st, var) printf("\t%s: %f\n\r", #var, st->var)

/**
 * Copies the status repository's values to list of values.
 * This function can be useful to pack and transmit the status variables
 * @warning @varlist and @status must point to reserved memory spaces
 *
 * @param status dat_status_t *. Pointer to destination structure
 */
void dat_status_to_list(value32_t *varlist, dat_sys_var_t *status, int nvars);

/**
 * Copies a list of status values to a list of status variables.
 * This function can be useful to pack and transmit the status variables
 * @warning @varlist and @status must point to reserved memory spaces
 *
 * @param status dat_status_t *. Pointer to destination structure
 */
void dat_status_from_list(value32_t *varlist, dat_sys_var_t *status, dat_sys_var_t *reference, int nvars);

/**
 * Print the names and values of a system status variable list.
 * @param status Pointer to a status variables list
 */
void dat_print_status(dat_sys_var_t *status, int nvars);

#endif //REPO_DATA_SCHEMA_H
