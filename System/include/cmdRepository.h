#ifndef CMD_REPO_H
#define CMD_REPO_H

#include "cmdIncludes.h"

/* Add files with commands */
#include "cmdOBC.h"

int cmdNULL(void *param);

cmdFunction repo_getCmd(int cmdID);
int repo_getsysReq(int cmdID);

int repo_onResetCmdRepo(void);

#endif /* CMD_REPO_H */
