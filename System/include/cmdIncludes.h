#ifndef CMD_INCLUDES_H
#define CMD_INCLUDES_H

/**
 *  Defines the prototype that every command must conform
 */
typedef int (*cmdFunction)( void * );

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


#define CMD_CMDNULL     (0xFFFF)    ///< Dummy command id. Represent a null command

#define CMD_SYSREQ_MIN      (1)     ///< Min energy level possible
#define CMD_SYSREQ_MAX      (9)     ///< Max energy level possible

#endif /* CMD_INCLUDES_H */
