/**
 * @file  cmdCOM.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2017
 * @copyright GNU Public License.
 *
 * This header contains commands related with the communication system
 */

#ifndef CMD_COM_H
#define CMD_COM_H

#include "csp/csp.h"

#include "repoCommand.h"

/**
 * Registers communications commands in the system
 */
void cmd_com_init(void);

/**
 * Send ping to node
 * @param fmt Str. Parameters format "%d"
 * @param param Str. Parameters as string: <node>. Ex: "10"
 * @param nparams Int. Number of parameters 1
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int com_ping(char *fmt, char *param, int nparams);

/**
 * Send debug message to node (using the port assigned to debug messages)
 * @param fmt Str. Parameters format "%d %s"
 * @param param Str. Parameters as string: "<node> <message>". Ex: "10 Hi!"
 * @param nparams Int. Number of parameters 2
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int com_send_dbg(char *fmt, char *param, int nparams);

/**
 * Send debug message to node (using the port assigned to console commands)
 * @param fmt Str. Parameters format "%d %s"
 * @param param Str. Parameters as string: "<node> <command>". Ex: "10 help"
 * @param nparams Int. Number of parameters 2
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int com_send_cmd(char *fmt, char *param, int nparams);

#endif /* CMD_COM_H */
