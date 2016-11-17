/**
 * @file  cmdRepoitory.h
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @author Carlos Gonzalez C - carlgonz@ug.uchile.cl
 * @date 2012
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

/* Defines */
#define CMD_IDORIG_THOUSEKEEPING    0x1001
#define CMD_IDORIG_TCONSOLE    0x1002

void cmd_add(char *name, cmdFunction function, int nparam);
cmd_t cmd_get_str(char *name);
cmd_t cmd_get_idx(int idx);
void cmd_print_all(void);
int cmd_init(void);
int cmd_null(int nparam, void *param);

#endif /* CMD_REPO_H */
