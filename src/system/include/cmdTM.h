/**
 * @file  cmdTM.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2018
 * @copyright GNU Public License.
 *
 * This header contains commands related with telemetry proccessing and
 * downloading.
 */

#ifndef CMDTM_H
#define CMDTM_H

#include "config.h"
#include "utils.h"

#include "repoCommand.h"
#include "repoData.h"
#include "cmdCOM.h"

/**
 * Register TM commands
 */
void cmd_tm_init(void);

/**
 * Send status variables as telemetry. This command collects the current value
 * of all status variables, builds a frame and downloads telemetry to the
 * specified node.
 *
 * @param fmt Str. Parameters format: "%d"
 * @param param Str. Parameters as string, node to send TM: <node>. Ex: "10"
 * @param nparams Int. Number of parameters: 1
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int cmd_tm_send_status(char *fmt, char *params, int nparams);

#endif //CMDTM_H
