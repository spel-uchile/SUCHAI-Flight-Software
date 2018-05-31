/**
 * @file  cmdCOM.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2018
 * @copyright GNU Public License.
 *
 * This header contains commands related with the communication system
 */

#ifndef CMD_COM_H
#define CMD_COM_H

#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "config.h"

#include "repoCommand.h"

#if SCH_COMM_ENABLE
#include "csp/csp.h"
#endif


/**
 * Parameter to com_send_data. Stores the destination node and binary data.
 */
typedef struct com_data{
    uint8_t node;                       ///< destination node
    uint8_t data[SCH_BUFF_MAX_LEN];     ///< data buffer
}com_data_t;

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
 * Send a message using the digi-repeater port, so its expect to receive the
 * the same message back.
 *
 * @param fmt Str. Parameters format "%d %s"
 * @param param Str. Parameters as string: "<node> <message>". Ex: "10 Hi!"
 * @param nparams Int. Number of parameters 2
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int com_send_rpt(char *fmt, char *param, int nparams);

/**
 * Send a command to node using the port assigned to console commands. It
 * expects the confirmation code: 200.
 *
 * @param fmt Str. Parameters format "%d %s"
 * @param param Str. Parameters as string: "<node> <command> [parameters]". Ex: "10 help"
 * @param nparams Int. Number of parameters 2
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int com_send_cmd(char *fmt, char *param, int nparams);

/**
 * Sends telemetry data using CSP. Data is received in @params as binary, packed
 * in a @com_data_t structure that contains the destination node and the data.
 * Its expect the confirmation code: 200. See the usage example.
 *
 * @param fmt Str. Parameters format: "" (not used)
 * @param params com_data_t *. Pointer to a com_data_t structure.
 * @param nparams int. Number of parameters: 1
 * @return CMD_OK if executed correctly (data was sent and confirmed)
 * or CMD_FAIL in case of errors.
 *
 * @code
 *      // Create the data buffer
 *      com_data_t data;
 *      data.node = 10;                       // Set the destination node
 *      memset(data.data, 0, sizeof(data)-1); // Set the data to send (zeros)
 *
 *      // Case 1: Call the command directly
 *      com_send_data("", &data, 1);
 *
 *      // Case 2: Call the command from repoCommand
 *      cmd_t *send_cmd = cmd_get_str("send_data");         // Get the command
 *      cmd_add_params_raw(send_cmd, &data, sizeof(data));  // Add params as binary data
 *      cmd_send(send_cmd);
 * @endcode
 */
int com_send_data(char *fmt, char *params, int nparams);

#endif /* CMD_COM_H */
