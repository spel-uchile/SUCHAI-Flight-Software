/**
 * @file  repoData.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Camilo Rojas M - camrojas@uchile.cl
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @author Matias Ramirez M  - nicoram.mt@gmail.com
 * @date 2018
 * @copyright GNU GPL v3
 *
 * This header is an API to system status repository and data repository:
 */

#ifndef DATA_REPO_H
#define DATA_REPO_H

#include "osSemphr.h"

#include "config.h"
#include "globals.h"
#include "utils.h"

#if SCH_STORAGE_MODE
    #include "data_storage.h"
#endif

typedef union fvalue{
    float f;
    int32_t i;
} fvalue_t;


#define DAT_OBC_OPMODE_NORMAL   (0) ///< Normal operation
#define DAT_OBC_OPMODE_WARN     (1) ///< Fail safe operation
#define DAT_OBC_OPMODE_FAIL     (2) ///< Generalized fail operation

#define DAT_REPO_SYSTEM "dat_system"    ///< Status variables table name

/** Copy a system @var to a status strcture @st */
#define DAT_CPY_SYSTEM_VAR(st, var) st->var = dat_get_system_var(var)
#define DAT_CPY_SYSTEM_VAR_F(st, var) {fvalue_t v; v.i = (float)dat_get_system_var(var); st->var = v.f;}
/** Print the name and vale of a integer system status variable */
#define DAT_PRINT_SYSTEM_VAR(st, var) printf("\t%s: %d\n", #var, st->var)
/** Print the name and vale of a float system status variable */
#define DAT_PRINT_SYSTEM_VAR_F(st, var) printf("\t%s: %f\n", #var, st->var)

/**
 * System level status variables
 */
typedef struct __attribute__((packed)) fp_entry {
    int unixtime;               ///< time to execute in unixtime
    char* cmd;                  ///< command to execute
    char* args;                 ///< command arguments
    int executions;             ///< times to be executed
    int periodical;             ///< time to be executed again
} fp_entry_t;

/**
 * System level status variables
 */
typedef enum dat_system {
    /// OBC: on board computer related variables.
    dat_obc_opmode = 0,        ///< General operation mode
    dat_obc_last_reset,        ///< Last reset source
    dat_obc_hrs_alive,         ///< Hours since first boot
    dat_obc_hrs_wo_reset,      ///< Hours since last reset
    dat_obc_reset_counter,     ///< Number of reset since first boot
    dat_obc_sw_wdt,            ///< Software watchdog timer counter
    dat_obc_temp_1,            ///< Temperature value of the first sensor
    dat_obc_temp_2,            ///< Temperature value of the second sensor
    dat_obc_temp_3,            ///< Temperature value of the gyroscope

    /// DEP: deployment related variables.
    dat_dep_ant_deployed,      ///< Was the antenna deployed?
    dat_dep_date_time,         ///< Deployment unix time

    /// RTC: related variables
    dat_rtc_date_time,         /// RTC current unix time

    /// COM: communications system variables.
    dat_com_count_tm,          ///< Number of TM sent
    dat_com_count_tc,          ///< Number of received TC
    dat_com_last_tc,           ///< Unix time of the last received tc
    dat_com_freq,              ///< Frequency [Hz]
    dat_com_tx_pwr,            ///< TX power (0: 25dBm, 1: 27dBm, 2: 28dBm, 3: 30dBm)
    dat_com_baud,              ///< Baudrate [bps]
    dat_com_mode,              ///< Framing mode (1: RAW, 2: ASM, 3: HDLC, 4: Viterbi, 5: GOLAY, 6: AX25)
    dat_com_bcn_period,        ///< Number of seconds between beacon packets

    /// FPL: flight plant related variables
    dat_fpl_last,              ///< Last executed flight plan (unix time)
    dat_fpl_queue,             ///< Flight plan queue length

    /// ADS: Attitude determination system
    dat_ads_acc_x,            ///< Gyroscope acceleration value along the x axis
    dat_ads_acc_y,            ///< Gyroscope acceleration value along the y axis
    dat_ads_acc_z,            ///< Gyroscope acceleration value along the z axis
    dat_ads_mag_x,            ///< Magnetometer x axis
    dat_ads_mag_y,            ///< Magnetometer y axis
    dat_ads_mag_z,            ///< Magnetometer z axis

    /// EPS: Energy power system
    dat_eps_vbatt,            ///< Voltage of battery [mV]
    dat_eps_cur_sun,          ///< Current from boost converters [mA]
    dat_eps_cur_sys,          ///< Current out of battery [mA]
    dat_eps_temp_bat0,        ///< Battery temperature sensor

    /// Memory: Current payload memory address
    dat_mem_temp,             ///< Temperature data index
    dat_mem_ads,              ///< ADS data index

    /// Add custom status variables here
    //dat_custom              ///< Variable description

    /// LAST ELEMENT: DO NOT EDIT
    dat_system_last_var       ///< Dummy element, the number of status variables
} dat_system_t;

/**
 * System level status variables
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
    int32_t dat_dep_ant_deployed;   ///< Was the antenna deployed?
    int32_t dat_dep_date_time;      ///< Deployment unix time

    /// RTC: related variables
    int32_t dat_rtc_date_time;      /// RTC current unix time

    /// COM: communications system variables.
    int32_t dat_com_count_tm;       ///< number of TM sent
    int32_t dat_com_count_tc;       ///< number of received TC
    int32_t dat_com_last_tc;        ///< Unix time of the last received tc
    uint32_t dat_com_freq;          ///< Frequency [Hz]
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
    float dat_ads_mag_x;            ///< Magnetometer x axis
    float dat_ads_mag_y;            ///< Magnetometer y axis
    float dat_ads_mag_z;            ///< Magnetometer z axis

    /// EPS: Energy power system
    uint32_t dat_eps_vbatt;         ///< Voltage of battery [mV]
    uint32_t dat_eps_cur_sun;       ///< Current from boost converters [mA]
    uint32_t dat_eps_cur_sys;       ///< Current out of battery [mA]
    uint32_t dat_eps_temp_bat0;     ///< Battery temperature sensor

    /// Memory: Current payload memory address
    uint32_t dat_mem_temp;
    uint32_t dat_mem_ads;

    /// LAST ELEMENT: DO NOT EDIT
    int32_t dat_system_last_var;    ///< Dummy element, the number of status variables
} dat_status_t;

/**
 * Payloads structures
 */
typedef enum payload_id {
    temp_sensors=0,
    ads_sensors,
    last_sensor // Dummy value
} payload_id_t;

typedef struct temp_data {
    float obc_temp_1;
    float obc_temp_2;
    float obc_temp_3;
} temp_data_t;

typedef struct ads_data {
    float acc_x;            ///< Gyroscope acceleration value along the x axis
    float acc_y;            ///< Gyroscope acceleration value along the y axis
    float acc_z;            ///< Gyroscope acceleration value along the z axis
    float mag_x;            ///< Magnetometer x axis
    float mag_y;            ///< Magnetometer y axis
    float mag_z;            ///< Magnetometer z axis
} ads_data_t;

/**
 * Initializes data repositories including buffers and mutexes
 */
void dat_repo_init(void);

/**
 * Cleanup and close data repository resources
 */
void dat_repo_close(void);

/**
 * Sets a status variable value
 *
 * @param index dat_system_t. Variable to set @sa dat_system_t
 * @param value Int. Value to set
 */
void dat_set_system_var(dat_system_t index, int value);

/**
 * Returns a status variable's value
 *
 * @param index Variable to set @sa DAT_CubesatVar
 * @return Variable value
 */
int dat_get_system_var(dat_system_t index);

/**
 * Copy the list of status variables to a dat_status_t struct.
 * This function can be useful to debug status variables @seealso dat_print_status
 * and to pack the variables prior to send it using libcsp @seealso tm_send_status.
 *
 * @param status dat_status_t *. Pointer to destination structure
 */
void dat_status_to_struct(dat_status_t *status);

/**
 * Print the name a values of the system status variables.
 * @seealso dat_status_to_struct.
 *
 * @param status dat_status_t *. Pointer to the status variables struct
 */
void dat_print_status(dat_status_t *status);

/**
 * Get the necessary parameters to send a command and set the values in
 * the variables command, args, repeat and periodical
 *
 * @param elapsed_sec Int. time in unixtime
 * @param command Save the command name
 * @param args Save the command arguments
 * @param executions Save the times to execute the command
 * @param periodical Save the periodical value (1 is periodical, 0 is not periodical)
 * @return 0 OK, -1 Error
 */
int dat_get_fp(int elapsed_sec, char* command, char* args, int* executions, int* periodical);

/**
 * Set a command with its args, executions (how many times will be executed)
 * and periodical (seconds to be executed again) to be executed at a certain time
 *
 * @param timetodo Int. time to execute the command
 * @param command Str. command name
 * @param args Str. command arguments
 * @param executions Int. times to execute the command
 * @param periodical Int. periodical value (in seconds)
 * @return 0 OK, -1 Error
 */
int dat_set_fp(int timetodo, char* command, char* args, int executions, int periodical);

/**
 * Detelete a flight plan command that will be executed at a certain time
 *
 * @param timetodo Int. time to execute the command
 * @return 0 OK, 1 Error
 */
int dat_del_fp(int timetodo);

/**
 *
 *Restart the flight plan
 *
 * @return 0 OK, 1 Error
 */
int dat_reset_fp(void);

/**
 *Show the flight plan
 *
 * @return 0 OK, 1 Error
 */
int dat_show_fp (void);

/**
 * Get the system time
 *
 * @return time_t actual system time (in seconds)
 */
time_t dat_get_time(void);

/**
 * Without Linux this function update a second in the system time
 *
 * @return 0 OK, 1 Error
 */
int dat_update_time(void);

/**
 * Set the system time (only if not Linux)
 *
 * @param new_time Int. time to set as system time
 * @return 0 OK, 1 Error
 */
int dat_set_time(int new_time);

/**
 * Show the system time in the format given
 *
 * @param format Int. Date time format: 0 for ISO, 1 for UNIX TIME
 * @return 0 OK, 1 Error
 */
int dat_show_time(int format);

/**
 * Add data struct to payload table
 *
 * @param data Pointer to struct
 * @param payload Int, payload to store
 * @return 0 OK, -1 Error
 */
int dat_add_payload_sample(void* data, int payload);

/**
 * Add data struct to payload table
 *
 * @param data Pointer to struct
 * @param payload Int, payload to store
 * @param delay Int, delay from recent value
 * @return 0 OK, -1 Error
 */
int dat_get_recent_payload_sample(void* data, int payload, int delay);

/**
 * Delete all memory sections in NOR FLASH
 *
 * @return OK 0, Error -1
 */
int dat_delete_memory_sections(void);



#endif // DATA_REPO_H
