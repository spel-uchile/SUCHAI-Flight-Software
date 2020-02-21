/**
 * @file  repoData.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Camilo Rojas M - camrojas@uchile.cl
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @author Matias Ramirez M  - nicoram.mt@gmail.com
 * @author Diego Ortego P - diortego@dcc.uchile.cl
 * @date 2019
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

#include "config.h"
#include "globals.h"
#include "utils.h"
#include "data_storage.h"
#include "osSemphr.h"
#include <repoDataSchema.h>

typedef enum machine_action {
    ACT_PAUSE= 0,
    ACT_START,
    ACT_STAND_BY,
    ACT_LAST
} machine_action_t;

typedef enum machine_state {
    ST_PAUSE = 0,
    ST_SAMPLING,
    ST_LAST
} machine_state_t;

typedef struct sample_machine{
    machine_state_t state;
    machine_action_t action;
    unsigned int active_payloads;
    unsigned int step;
    int samples_left;
} sample_machine_t;

extern sample_machine_t machine;

/**
 * Change sample machine state.
 *
 * @param machine action to take (ST_PAUSE, ST_SAMPLING)
 * @param step seconds period of sampling measure in seconds
 * @param nsamples maximum samples to take, if value is-1 the machine will take unlimited samples
 */
int set_machine_state(machine_action_t action, unsigned int step, int nsamples);


/**
 * Initializes payload storage helper variables
 */
void initialize_payload_vars(void);

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
void _dat_set_system_var(dat_system_t index, int value);  //Auxiliary function for testing

/**
 * Returns an int field's value inside the status repository.
 *
 * @param index Enum index of the field to get
 * @return The field's value
 */
int dat_get_system_var(dat_system_t index);
int _dat_get_system_var(dat_system_t index);  //Auxiliary function for testing

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
 * @param period Pointer for saving the period of period execution of the command, in unix-time
 * @return 0 if OK, -1 if no command was found
 */
int dat_get_fp(int elapsed_sec, char* command, char* args, int* executions, int* period);

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
 * Print the UTC system time in a given format.
 *
 * 0 for ISO format or 1 for UNIX TIME format, >1 Both formats.
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
 * TODO: Change variable name from delay to offset??
 * Gets a data struct from the payload table.
 *
 * @param data Pointer to the struct where the values will be stored
 * @param payload Payload id to get
 * @param delay Delay of the recent value
 * @return 0 if OK, -1 if and error occurred
 */
int dat_get_recent_payload_sample(void* data, int payload, int offset);

/**
 * Deletes all memory sections in NOR FLASH.
 *
 * @return 0 if OK, -1 if an error occurred
 */
int dat_delete_memory_sections(void);

/**
 * Print struct contained in data with payload schema
 *
 * @param data Pointer to the struct where the values will be stored
 * @param payload Payload id
 * @return 0 if OK, -1 if and error occurred
 */
int dat_print_payload_struct(void* data, unsigned int payload);

/**
 * Helper function to get var results in payload struct
 *
 * @param tok_sym
 * @param tok_var
 * @param order
 * @param var_names
 * @param i
 * @return 0 if OK, -1 if and error occurred
 */
int get_payloads_tokens(char** tok_sym, char** tok_var, char* order, char* var_names, int i);

/**
 * Helper function to get value as string in payload struct
 *
 * @param ret_string
 * @param c_type
 * @param buff
 */
void get_value_string(char* ret_string, char* c_type, char* buff);

/**
 *  Helper function to get size of variable in payload struct
 *
 * @param c_type
 * @return
 */
int get_sizeof_type(char* c_type);

#endif // DATA_REPO_H
