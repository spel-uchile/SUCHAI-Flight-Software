/**
 * @file  cmdRepoitory.h
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @author Carlos Gonzalez C - carlgonz@ug.uchile.cl
 * @date 2016
 * @copyright GNU GPL v3
 *
 * This header have definitions related to command repository
 */

#ifndef CMD_REPO_H
#define CMD_REPO_H

#include "cmdIncludes.h"

/* Add files with commands */
#include "cmdOBC.h"
#include "cmdDRP.h"
#include "cmdConsole.h"

/* Command definitions */
/**
 *  Defines the prototype of a command
 */
typedef int (*cmdFunction)( int, void * );

/**
 * Structure to store a command sent to
 * execution
 */
typedef struct cmd_type{
    int id;                     ///< Command id
    int nparams;                 ///< Number of parameters
    void *params;               ///< List of parameters (use malloc)
    cmdFunction function;       ///< Command function
} cmd_t;

/**
 * Structure to store the list of
 * available commands by name
 */
typedef struct cmd_list_type{
    int nparams;                ///< Number of parameters
    char *name;                 ///< Command name (use malloc)
    cmdFunction function;       ///< Command function
} cmd_list_t;

/* Function definitions */
/**
 * Registers a command in the system
 *
 * @param name Str. Name of the comand, max CMD_NAME_LEN characters
 * @param function Pointer to command function
 * @param params Str. to define command parameters
 * @return None
 */
void cmd_add(char *name, cmdFunction function, int nparam);

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
int cmd_null(int nparam, void *param);

#endif /* CMD_REPO_H */