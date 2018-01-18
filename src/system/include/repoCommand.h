/**
 * @file  cmdRepoitory.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @date 2017
 * @copyright GNU GPL v3
 *
 * This header have definitions related to command repository
 */

#ifndef CMD_REPO_H
#define CMD_REPO_H

#include <stdarg.h>

#include "utils.h"
#include "globals.h"

/* Add files with commands */
#include "cmdOBC.h"
#include "cmdDRP.h"
#include "cmdConsole.h"
#include "cmdFP.h"

#if SCH_COMM_ENABLE
    #include "cmdCOM.h"
#endif
#if SCH_RUN_TESTS
    #include "cmdTestCommand.h"
#endif

/* Macros */
/**
 * Send command to execution using dispatcherQueue (must be initialized). Blocks
 * if the queue is full
 *
 * @param cmd *cmd_type, pointer to command
 */
#define cmd_send(cmd) osQueueSend(dispatcher_queue, &cmd, portMAX_DELAY);

/* Command definitions */
/**
 * Define return the command
 */
#define CMD_OK 1
#define CMD_FAIL 0
#define CMD_ERROR -1

/**
 * Fixed buffers lengths
 */
#define CMD_MAX_LEN 100
#define CMD_MAX_STR_PARAMS 64

/**
 *  Defines the prototype of a command
 */
typedef int (*cmdFunction)(char *fmt, char *params, int nparams);

/**
 * Structure to store a command sent to
 * execution
 */
typedef struct cmd_type{
    int id;                     ///< Command id
    int nparams;                ///< Number of parameters
    char *fmt;                  ///< Format of parameters
    char *params;               ///< List of parameters (use malloc)
    cmdFunction function;       ///< Command function
} cmd_t;

/**
 * Structure to store the list of
 * available commands by name
 */
typedef struct cmd_list_type{
    int nparams;                ///< Number of parameters
    char *fmt;                  ///< Format of parameters
    char *name;                 ///< Command name (use malloc)
    cmdFunction function;       ///< Command function
} cmd_list_t;

/* Function definitions */
/**
 * Registers a command in the system
 *
 * @param function Pointer to command function
 * @param fparams Str. defines format of parameters, separated by spaces
 * @param nparam Int. number of parameters, according to @fparams
 * @return Int. Length of command list in case of success or CMD_ERROR (-1) if
 * an error occurred.
 * @example:
 *
 *      // Adds command foo with 2 params: integer and string
 *      cmd_add("foo", foo, "%d %s", 2);
 *      // Adds command bar with 3 params: integers and double
 *      cmd_add("bar", bar, "&d %d %f", 3);
 */
int cmd_add(char *name, cmdFunction function, char *fmt, int nparams);

/**
 * Create a new command by name
 *
 * @param name Str. Command name
 * @return cmd_t * Pointer to command structure already initialized. Null if
 *                 command does not exists. Use @cmd_free to free allocated
 *                 memory.
 */
cmd_t * cmd_get_str(char *name);

/**
 * Create a new command by index or id
 *
 * @param idx Int command index or Id.
 * @return cmd_t Command structure. Null if command does not exists.
 */
cmd_t * cmd_get_idx(int idx);

/**
 * Find the name of a command by id. This function allocates memory for the
 * string so the user must free the array.
 *
 * @param idx Int. Command index or id
 * @param name *char. Pointer to char variable. The function allocates memory so
 * make sure to free the array after use.
 * @return Int. CMD_OK if the command was found, CMD_ERROR otherwise.
 */
char * cmd_get_name(int idx);

/**
 * Fills command parameters as string
 * @param cmd cmd_t. Command to fill parameters
 * @param params Str. String with parameters
 * @example
 *
 *      //Fill two parameters to foo command
 *      cmd_t *foo = cmd_get_str("foo"); // foo.fmt is '%d %s'
 *      cmd_add_params(foo, "123 abc");
 *      //Fill three parameters to bar command
 *      cmd_t *bar = cmd_get_str("bar"); // bar.fmt is '%d %d %f'
 *      cmd_add_params(bar, "1 2 3.4");
 */
void cmd_add_params_str(cmd_t *cmd, char *params);

/**
 * Fills command parameters by variables.
 * Note variables are converted to string
 * @param cmd cmd_t. Command to fill parameters
 * @param ... List of variables to fill as parameters
 * @example
 *
 *      //Fill two parameters to foo command
 *      int a = 123; char *b = "abc";
 *      cmd_t *foo = cmd_get_str("foo"); // foo.fmt is '%d %s'
 *      cmd_add_params_fmt(foo, a, b);
 *      //Fill three parameters to bar command
 *      int c = 1; int d = 2; float e = 3.4;
 *      cmd_t *bar = cmd_get_str("bar"); // bar.fmt is '%d %d %f'
 *      cmd_add_params_fmt(bar, c, d, e);
 */
void cmd_add_params_var(cmd_t *cmd, ...);


/**
 * Destroys a command and frees the allocated memory
 */
void cmd_free(cmd_t *cmd);

/**
* Print the list of registered commands
*/
void cmd_print_all(void);

/**
 * Initializes the command buffer adding null_cmd
 *
 * @return 1
 */
int cmd_repo_init(void);

/**
 * Null command, just print to stdout
 *
 * @param param Not used
 * @return 1, allways successful
 */
int cmd_null(char *fmt, char *params, int nparams);

#endif /* CMD_REPO_H */
