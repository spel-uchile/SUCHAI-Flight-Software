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

#define TM_TYPE_GENERIC 0
#define TM_TYPE_STATUS  1
#define TM_TYPE_PAYLOAD 10

/**
 * Register TM commands
 */
void cmd_tm_init(void);

/**
 * Send status variables as telemetry. This command collects the current value
 * of all status variables, builds a frame and downloads telemetry to the
 * specified node. To parse the data @seealso tm_parse_status
 *
 * @param fmt Str. Parameters format: "%d"
 * @param param Str. Parameters as string, node to send TM: <node>. Ex: "10"
 * @param nparams Int. Number of parameters: 1
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int tm_send_status(char *fmt, char *params, int nparams);

/**
 * Parses a status variables telemetry, @seealso tm_send_status.
 *
 * @param fmt Str. Not used.
 * @param param char *. Parameters as pointer to raw data. Receives a
 * status_t structure
 * @param nparams Int. Not used.
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int tm_parse_status(char *fmt, char *params, int nparams);

/**
 * Send data stored as payload. TODO: complete docs
 * @param fmt
 * @param params
 * @param nparams
 * @return
 */
int tm_send_pay_data(char *fmt, char *params, int nparams);

#endif //CMDTM_H
