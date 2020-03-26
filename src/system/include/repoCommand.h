/**
 * @file  repoCommand.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @date 2020
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
#if SCH_FP_ENABLED
#include "cmdFP.h"
#endif
#if SCH_COMM_ENABLE
#include "cmdCOM.h"
#include "cmdTM.h"
#endif
#if SCH_TEST_ENABLED
#include "cmdTestCommand.h"
#endif
#ifdef SCH_USE_NANOPOWER
#include "cmdEPS.h"
#endif
#ifdef SCH_SEN_ENABLED
#include "cmdSensors.h"
#endif
#ifdef SCH_USE_GSSB
#include "cmdGSSB.h"
#endif


/* Macros */
/**
 * Send command to execution using dispatcherQueue (must be initialized). Blocks
 * if the queue is full
 *
 * @param cmd *cmd_type, pointer to command
 */
#define cmd_send(cmd) if(cmd != NULL){osQueueSend(dispatcher_queue, &cmd, portMAX_DELAY);}

/* Command definitions */
/**
 * Define return the command
 */
#define CMD_OK 1        ///< Command executed successfully
#define CMD_FAIL 0      ///< Command not executed as expected
#define CMD_ERROR -1    ///< Command returned an error

/**
 *  Defines the prototype of a command
 */
typedef int (*cmdFunction)(char *fmt, char *params, int nparams);

#define IF_PARSE_PARAMS(...) if(sscanf(params, fmt, ##__VA_ARGS) == nparams)

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
 *
 * @code
 *      // Adds command foo with 2 params: integer and string
 *      cmd_add("foo", foo, "%d %s", 2);
 *      // Adds command bar with 3 params: integers and double
 *      cmd_add("bar", bar, "&d %d %f", 3);
 * @endcode
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
 * @return Int. CMD_OK if the command was found, CMD_ERROR otherwise.
 */
char * cmd_get_name(int idx);

/**
 * Fills command parameters as raw data using memcpy.@len bytes will be copied
 * from @params to @cmd->params.
 *
 * @note make sure that @params contains at least @len bytes.
 *
 * @param cmd cmd_t *. Command to fill parameters
 * @param params void *. Pointer to the data buffer
 * @param len int. Length of the data buffer.
 *
 * @code
 *      char *data = {0,1,2,3,4,5};
 *      cmd_t *foo = cmd_get_str("foo");
 *      cmd_add_params_raw(foo, data, 6);
 * @endcode
 */
void cmd_add_params_raw(cmd_t *cmd, void *params, int len);

/**
 * Fills command parameters as string
 * @note does not check the parameters format or if the command requires param.
 *
 * @param cmd cmd_t. Command to fill parameters
 * @param params Str. String with parameters
 *
 * @code
 *      //Fill two parameters to foo command
 *      cmd_t *foo = cmd_get_str("foo"); // foo.fmt is '%d %s'
 *      cmd_add_params(foo, "123 abc");
 *      //Fill three parameters to bar command
 *      cmd_t *bar = cmd_get_str("bar"); // bar.fmt is '%d %d %f'
 *      cmd_add_params(bar, "1 2 3.4");
 * @endcode
 */
void cmd_add_params_str(cmd_t *cmd, char *params);

/**
 * Fills command parameters by variables using the registered parameters format.
 * @note variables are converted to string
 *
 * @param cmd cmd_t. Command to fill parameters
 * @param ... List of variables to fill as parameters
 *
 * @code
 *      //Fill two parameters to foo command
 *      int a = 123; char *b = "abc";
 *      cmd_t *foo = cmd_get_str("foo"); // foo.fmt is '%d %s'
 *      cmd_add_params_var(foo, a, b);
 *      //Fill three parameters to bar command
 *      int c = 1; int d = 2; float e = 3.4;
 *      cmd_t *bar = cmd_get_str("bar"); // bar.fmt is '%d %d %f'
 *      cmd_add_params_var(bar, c, d, e);
 * @endcode
 */
void cmd_add_params_var(cmd_t *cmd, ...);

/**
 * Returns a new command with parameters form a string with the format:
 * <command> [parameters]. The [parameters] field is optional. Returns NULL if
 * the command is not found or in case of errors.
 *
 * @param buff str. A null terminated string with the format <command> [parameters]
 * @return cmd_t. A new command (uses malloc) or NULL in case of errors.
 *
 * @code
 *      char *str_cmd = "obc_debug 1";
 *      cmd_t *cmd = cmd_parse_from_str(str_cmd);
 *      assert(cmd != NULL);
 * @endcode
 */
cmd_t *cmd_parse_from_str(char *buff);

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
 * Cleans the repo buffer. Frees allocated memories
 */
void cmd_repo_close(void);

/**
 * Null command, just print to stdout
 *
 * @params fmt Not used
 * @params param Not used
 * @params nparams Not Used
 * @return 1, allways successful
 */
int cmd_null(char *fmt, char *params, int nparams);

/**
 *
 *Print each value of the structure
 *
 * @param cmd cmd_t. The command that will be printed
 * @return 0 OK, 1 Error
 */
int cmd_print(cmd_t* cmd);

/**
 * Find the format of a command by name. This function allocates memory for the
 * string so the user must free the array.
 *
 * @param name *char. Pointer to char variable. The function allocates memory so
 * make sure to free the array after use.
 * @return char* with the command's format
 */
char* cmd_get_fmt(char* name);

#endif /* CMD_REPO_H */
