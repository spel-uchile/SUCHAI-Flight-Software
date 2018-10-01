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

#include "csp/csp.h"
#include "repoCommand.h"

#include "ax100_param.h"
#include "ax100_param_radio.h"
#include "param/rparam_client.h"
//#include "param/param_string.h"

#define COM_FRAME_MAX_LEN (SCH_BUFF_MAX_LEN - 2 * sizeof(uint16_t))
#define AX100_PORT_RPARAM   7

typedef struct com_frame{
    uint16_t frame;
    uint16_t type;
    union{
        uint8_t data8[COM_FRAME_MAX_LEN];
        uint16_t data16[COM_FRAME_MAX_LEN/2];
        uint32_t data32[COM_FRAME_MAX_LEN/4];
    }data;
}com_frame_t;

/**
 * Parameter to com_send_data. Stores the destination node and binary data.
 */
typedef struct com_data{
    uint8_t node;                       ///< destination node
    com_frame_t frame;
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

/**
 * Show CSP debug information, currently the route table and interfaces
 * @param fmt Not used
 * @param params Not used
 * @param nparams Not used
 * @return CMD_OK
 */
int com_debug(char *fmt, char *params, int nparams);

/**
 * Reset the TRX GND Watchdog timer at @node node by sending a CSP command to the
 * AX100_PORT_GNDWDT_RESET (9) port. This command targets the AX100 TRX.
 * If the <node> param is given, then the message is send to that node, if no
 * parameters given then the message is sent to SCH_TRX_ADDRESS node.
 *
 * @param fmt Str. Parameters format: "%d"
 * @param params Str. Parameters: [node], the TRX node number
 * @param nparams Str. Number of parameters: 0|1
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors.
 *
 * @code
 *      // Function usage
 *      com_reset_gnd_wdt("%d", "5", 1);  // Reset gnd wdt of node 5
 *      com_reset_gnd_wdt("", "", 0);     // Reset gnd wdt of default TRX node
 *
 *      // Send reset gnd wdt to node 5
 *      cmd_t *send_cmd = cmd_get_str("com_reset_wdt"); // Get the command
 *      cmd_add_params_var(send_cmd, 5)  // Set param node to 5
 *      cmd_send(send_cmd);
 *
 *      // Send reset gnd_wdt to default SCH_TRX_ADDRESS node
 *      cmd_t *send_cmd = cmd_get_str("com_reset_wdt"); // Get the command
 *      cmd_send(send_cmd);  // Send command without parameters;
 *
 * @endcode
 */
int com_reset_wdt(char *fmt, char *params, int nparams);

/**
 * Print TRX housekeeping information
 * @warning not implemented yet
 *
 * @param fmt
 * @param params
 * @param nparams
 * @return
 */
int com_get_hk(char *fmt, char *params, int nparams);

/**
 * Get settings values from the TRX. The TRX has a list of parameters to set and
 * get (@see ax100_param.h and ax100_param_radio.h). Use this command to get
 * any parameter value by name. The special argument 'help' can be
 * used to print the list of available parameters.
 *
 * @param fmt Str. Parameters format: "%s"
 * @param params Str. Parameters: <param_name>, the parameter name
 * @param nparams Str. Number of parameters: 1
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors.
 *
 * @code
 *      // Function usage
 *      com_get_config("%s", "help", 1);     // Print the parameter list
 *      com_get_config("%s", "csp_node", 1); // Read and print the TRX node
 *
 *      // Command usage to get a TRX parameter
 *      cmd_t *send_cmd = cmd_get_str("com_get_config"); // Get the command
 *      cmd_add_params(send_cmd, "tx_pwr")  // Read param "tx_pwr"
 *      cmd_send(send_cmd);
 *
 * @endcode
 *
 */
int com_get_config(char *fmt, char *params, int nparams);

#endif /* CMD_COM_H */
