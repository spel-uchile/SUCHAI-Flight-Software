/**
 * @file  repoData.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Camilo Rojas M - camrojas@uchile.cl
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @author Matias Ramirez M  - nicoram.mt@gmail.com
 * @author Diego Ortego P - diortego@dcc.uchile.cl
 * @date 2018
 * @copyright GNU GPL v3
 *
 * This header is an API to the system's status repository and flight plan repository.
 *
 * The status repository contains the current status of the system.
 *
 * The flight plan repository contains a set of commands the system is set to run.
 *
 * These commands can be timed to execute in the future, execute periodically, and execute a
 * set number of times per periodic cycle.
 */

#ifndef DATA_REPO_H
#define DATA_REPO_H

#include "osSemphr.h"

#include "config.h"
#include "globals.h"
#include "utils.h"

#if SCH_STORAGE_MODE != 0
    #include "data_storage.h"
#endif

#ifdef NANOMIND
    #include "util/clock.h"
    #include "util/timestamp.h"
#endif

/** Union for easily casting status variable types */
typedef union fvalue{
    float f;
    int32_t i;
} fvalue_t;


#define DAT_OBC_OPMODE_NORMAL   (0) ///< Normal operation
#define DAT_OBC_OPMODE_WARN     (1) ///< Fail safe operation
#define DAT_OBC_OPMODE_FAIL     (2) ///< Generalized fail operation

// TODO: [Gedoix] Esto se puede hacer en el .c y usar #undef

/** The repository's name */
#define DAT_REPO_SYSTEM "dat_system"    ///< Status variables table name

#define DAT_GPS_TABLE "gps_table"
#define DAT_PRS_TABLE "pressure_table"
#define DAT_DPL_TABLE "deploy_table"

/** Copy a system @var to a status struct @st */
#define DAT_CPY_SYSTEM_VAR(st, var) st->var = dat_get_system_var(var)

/** Copy a float system @var to a status struct @st */
#define DAT_CPY_SYSTEM_VAR_F(st, var) {fvalue_t v; v.i = (float)dat_get_system_var(var); st->var = v.f;}

/** Print the name and value of a integer system status variable */
#define DAT_PRINT_SYSTEM_VAR(st, var) printf("\t%s: %lu\n", #var, (unsigned long)st->var)

/** Print the name and vale of a float system status variable */
#define DAT_PRINT_SYSTEM_VAR_F(st, var) printf("\t%s: %f\n", #var, st->var)


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

    /// EPS: Energy power system
    dat_eps_vbatt,                ///< Voltage of the battery [mV]
    dat_eps_cur_sun,              ///< Current from boost converters [mA]
    dat_eps_cur_sys,              ///< Current from the battery [mA]
    dat_eps_temp_bat0,            ///< Battery temperature sensor

    /// Memory: Current payload memory addresses
    dat_mem_temp,                 ///< Temperature data
    dat_mem_ads,                  ///< ADS data index
    dat_mem_eps,                  ///< EPS data index
    dat_mem_lang,                 ///< Langmuir data index
    dat_mem_gps,                  ///< GPS data index
    dat_mem_prs,                  ///< PRS data index
    dat_mem_dpl,                  ///< DPL data index

    /// Add custom status variables here
    dat_balloon_phase,          ///< Balloon phase (0: (A0) base, 1: (A) ascend, 2: (B) equilibrium, 3: (B1) deploy1, 4: (B2) deploy2, 5: (C) descend, 6: (C1) landing).
    dat_balloon_deploys,        ///< Number of balloons
    dat_balloon_height,         ///< Current height
    dat_balloon_pressure,       ///< Current pressure

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

    /// EPS: Energy power system
    uint32_t dat_eps_vbatt;         ///< Voltage of battery [mV]
    uint32_t dat_eps_cur_sun;       ///< Current from boost converters [mA]
    uint32_t dat_eps_cur_sys;       ///< Current out of battery [mA]
    uint32_t dat_eps_temp_bat0;     ///< Battery temperature sensor

    /// Memory: Current payload memory address
    uint32_t dat_mem_temp;          ///< Temperature data
    uint32_t dat_mem_ads;           ///< ADS data
    uint32_t dat_mem_eps;           ///< EPS data
    uint32_t dat_mem_lang;

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
    gps_sensors,            ///< GPS sensors
    prs_sensors,            ///< PRS sensors
    dpl_sensors,            ///< DPL sensors
    //custom_sensor,           ///< Add custom sensors here
    last_sensor             ///< Dummy element, the amount of payload variables
} payload_id_t;

/**
 * Struct for storing temperature data.
 */
typedef struct temp_data {
    int timestamp;
    float obc_temp_1;
    float obc_temp_2;
    float obc_temp_3;
} temp_data_t;

/**
 * Struct for storing data collected by ads sensors.
 */
typedef struct ads_data {
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
typedef struct eps_data {
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

typedef struct langmuir_data {
    int timestamp;
    float sweep_voltage;
    float plasma_voltage;
    float plasma_temperature;
    int particles_counter;
} langmuir_data_t;

typedef struct gps_data {
    int timestamp;
    float latitude;
    float longitude;
    float height;
    float velocity_x;
    float velocity_y;
    int satellites_number;
    int mode;
} gps_data_t;

typedef struct prs_data {
    int timestamp;
    float pressure;
    float temperature;
    float height;
} prs_data_t;

typedef struct dpl_data {
    int timestamp;
    int lineal_actuator;
    int servo_motor;
} dpl_data_t;


extern struct map {
    char table[30];
    uint16_t  size;
    int sys_index;
    char data_order[50];
    char var_names[200];
} data_map[last_sensor];

/**
 * Initializes data repositories, including:
 *      Both repo's mutexes.
 *      The storage system (if permanent memory is being used).
 *      The system status repo, on default values.
 *      The flight plan repo, on default values.
 *
 * @see dat_repo_close
 */
void dat_repo_init(void);

/**
 * Performs a cleanup and closes repository resources.
 *
 * Closes the storage system (if permanent memory is being used).
 *
 * @see dat_repo_init
 */
void dat_repo_close(void);

/**
 * Sets an int field in the status repository to a new value.
 *
 * @param index Enum index of the field to set
 * @param value Integer value to set the variable to
 */
void dat_set_system_var(dat_system_t index, int value);

/**
 * Returns an int field's value inside the status repository.
 *
 * @param index Enum index of the field to get
 * @return The field's value
 */
int dat_get_system_var(dat_system_t index);

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

/**
 * Gets an executable command from the flight plan repo.
 *
 * Given an elapsed seconds counter (assumed to be system time), sets the other parameter pointers to the values
 * of the first command found in the repo that is eligible for execution.
 *
 * Deletes the command from the repo before returning. If the command is periodic, the function saves a copy with
 * updated execution time (the period is added to the time of the next execution execution).
 *
 * @param elapsed_sec Time for finding executable commands
 * @param command Pointer for saving the command name
 * @param args Pointer for saving the command arguments
 * @param executions Pointer for saving the amount of executions the command does per periodic cycle
 * @param periodical Pointer for saving the period of periodical execution of the command, in unix-time
 * @return 0 if OK, -1 if no command was found
 */
int dat_get_fp(int elapsed_sec, char* command, char* args, int* executions, int* periodical);

/**
 * Saves a new command into the flight plan repo.
 *
 * @param timetodo Future time when the command should execute
 * @param command Command name
 * @param args Command arguments
 * @param executions Amount of times the command has to execute pero periodic cycle
 * @param periodical Period of periodical execution of the command, in unix-time
 * @return 0 if OK, 1 if no available space was found //
 */
int dat_set_fp(int timetodo, char* command, char* args, int executions, int periodical);

/**
 * Deletes the first command in the flight plan repo that's eligible for execution at the specified time.
 *
 * @param timetodo Time for finding executable commands
 * @return 0 if OK, 1 if no command was found
 */
int dat_del_fp(int timetodo);

/**
 * Deletes all flight plan command entries, resetting the repo.
 *
 * @return 0 if OK, 1 if error
 */
int dat_reset_fp(void);

/**
 * Prints all values in the flight plan repo.
 *
 * An example of the printed data is:
 *
 * When    Command    Arguments    Executions    Periodical
 * 1089    test_cmd   1            10            100
 *
 * @return 0 if OK
 */
int dat_show_fp (void);

/**
 * Gets the current system time in seconds.
 *
 * @return time_t Current system unix-time
 */
time_t dat_get_time(void);

/**
 * Updates the system time, adding one second to it.
 *
 * When in Linux this does nothing.
 *
 * @return 0 if OK
 */
int dat_update_time(void);

/**
 * Sets the current system time to a new value.
 *
 * @param new_time Time value to set as system time
 * @return 0 OK, 1 Error
 */
int dat_set_time(int new_time);

/**
 * Show the system time in a given format.
 *
 * 0 for ISO format or 1 for UNIX TIME format.
 *
 * @param format Format of the print
 * @return 0 if OK, 1 if the format input doesn't exist
 */
int dat_show_time(int format);

/**
 * Adds a data struct to they payload table.
 *
 * @param data Pointer to the struct to add
 * @param payload Payload id to store
 * @return 0 if OK, -1 if an error occurred
 */
int dat_add_payload_sample(void* data, int payload);

/**
 * Gets a data struct from the payload table.
 *
 * @param data Pointer to the struct where the values will be stored
 * @param payload Payload id to get
 * @param delay Delay of the recent value
 * @return 0 if OK, -1 if and error occurred
 */
int dat_get_recent_payload_sample(void* data, int payload, int delay);

/**
 * Deletes all memory sections in NOR FLASH.
 *
 * @return 0 if OK, -1 if an error occurred
 */
int dat_delete_memory_sections(void);



#endif // DATA_REPO_H
