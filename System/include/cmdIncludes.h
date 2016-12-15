/**
 * @file  cmdIncludes.h
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @author Carlos Gonzalez C - carlgonz@ug.uchile.cl
 * @date 2012
 * @copyright GNU GPL v3
 *
 * This header have general definitions about commands prototipes and related
 * data structures.
 */
 
#ifndef CMD_INCLUDES_H
#define CMD_INCLUDES_H

/**
 *  Defines the prototype that every command must conform
 */
typedef int (*cmdFunction)( int, void * );

/**
 * Structure that represents a command passed to executer. Contains a pointer of
 * type cmdFunction with the function to execute and one parameter for that
 * function
 */
typedef struct exec_command{
    int param;                  ///< Command parameter
    cmdFunction fnct;           ///< Command function
}ExeCmd;

/**
 * Structure that represent a command passed to dispatcher. Contains only a code
 * that represent the function to call, a paremeter and other command's metadata
 */
typedef struct ctrl_command{
    int cmdId;                  ///< Command id, represent the desired command
    int param;                  ///< Command parameter
    int idOrig;                 ///< Metadata: Id of sender subsystem
    int sysReq;                 ///< Metadata: Level of energy the command requires
}DispCmd;

#define CMD_NAME_LEN 15

typedef struct cmd_type{
    int id;                     ///< Command id
    int nparam;                 ///< Number of parameters
    void *params;               ///< List of parameters (use malloc)
    char *name;                 ///< Command name
    cmdFunction function;       ///< Command function

} cmd_t;

#define CMD_CMDNULL     (0xFFFF)    ///< Dummy command id. Represent a null command

#define CMD_SYSREQ_MIN      (1)     ///< Min energy level possible
#define CMD_SYSREQ_MAX      (9)     ///< Max energy level possible

#endif /* CMD_INCLUDES_H */
