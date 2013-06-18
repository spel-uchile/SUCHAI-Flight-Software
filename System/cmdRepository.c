#include "cmdRepository.h"

/* Add external cmd arrays */
extern cmdFunction obc_Function[];
extern int obc_sysReq[];

/**
 * Returns the energy level asociated to each command
 *
 * @param cmdID Command intentifier
 * @return Requiered energy level
 */
int repo_getsysReq(int cmdID)
{
    int cmdOwn, cmdNum;
    int result;

    cmdNum = (unsigned char)cmdID;
    cmdOwn = (unsigned char)(cmdID>>8);

    switch (cmdOwn)
    {
        case CMD_OBC:
            if(cmdNum >= OBC_NCMD)
                result = CMD_SYSREQ_MIN;
            else
                result = obc_sysReq[cmdNum];
            break;
        
        default:
            result = CMD_SYSREQ_MIN;
            break;
    }

    return result;
}

/**
 * Returns a pointer with the function asociated to each cmdID. If the id is not
 * registered, returns the standar null command.
 *
 * @param cmdID Command id
 * @return Pointer to function of type cmdFunction
 */
cmdFunction repo_getCmd(int cmdID)
{
    int cmdOwn, cmdNum;
    cmdFunction result;

    cmdNum = (unsigned char)cmdID;
    cmdOwn = (unsigned char)(cmdID>>8);

    switch (cmdOwn)
    {
        case CMD_OBC:
            if(cmdNum >= OBC_NCMD)
                result=cmdNULL;
            else
                result = obc_Function[cmdNum];
            break;
        
        default:
            result = cmdNULL;
            break;
    }

    return result;
}

/**
 * Initializes all cmd arrays
 *
 * @return 1, allways successful
 */
int repo_onResetCmdRepo(void)
{
    obc_onResetCmdOBC();

    return 1;
}

/**
 * Null command, just print to stdout
 *
 * @param param Not used
 * @return 1, allways successful
 */
int cmdNULL(void *param)
{
    int arg=*( (int *)param );
    printf("cmdNULL was used with param %d\n", arg);

    return 1;
}

