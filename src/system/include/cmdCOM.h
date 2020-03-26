/**
 * @file  cmdCOM.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2020
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

#include "drivers.h"
#include "repoCommand.h"

/**
 * Fixed size of one CSP frame is 200. A frame contains not only data but also
 * information about data @see com_frame_t. COM_FRAME_MAX_LEN is the space
 * available for payload data, so we have do substract 2 bytes for the frame
 * number, 2 bytes fot the telemetry type and 4 bytes for the number of data
 * samples inside a frame.
 * COM_FRAME_MAX_LEN = 200-2*2-4 = 192 bytes max
 */
#define COM_FRAME_MAX_LEN (200 - 2*sizeof(uint16_t) - sizeof(uint32_t))

/**
 * A SCP frame structure. It contains data buffer and information about the data
 * such as the frame number, the telemetry type and the nubmer of data samples
 * inside a frame.
 */
typedef struct __attribute__((__packed__)) com_frame{
    uint16_t nframe;         ///< Frame number
    uint16_t type;          ///< Telemetry type
    uint32_t ndata;         ///< Number of data samples (structs) in the frame
    /**
     * De data buffer containing @ndata structs of payload data. The structs
     * inside the buffer depends on the telemetry @type.
     */
    union{
        uint8_t data8[COM_FRAME_MAX_LEN];
        uint16_t data16[COM_FRAME_MAX_LEN / sizeof(uint16_t)];
        uint32_t data32[COM_FRAME_MAX_LEN / sizeof(uint32_t)];
    }data;
}com_frame_t;

/**
 * Parameter to com_send_data. Stores the destination node and binary data.
 */
typedef struct __attribute__((__packed__)) com_data{
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
 * Send a Telecommand (TC) frame to node. A TC frame contains several <command>
 * [parameters] pairs separated by ";" (semicolon), for example:
 *
 *      "help;send_cmd 10 help;ping 1;print_vars"
 *
 * The list of command will be parsed and queue in TaskCommunications @seealso
 * com_receive_tc
 *
 * @param fmt Str. Parameters format: "%d %n"
 * @param param Str. Parameters as string:
 *      "<node> <command> [parameters];<command> [parameters]".
 *      Ex: "10 help;ping 1"
 * @param nparams Int. Number of parameters: 1 (assumes that %n return the next
 *                parameter pointer).
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 *
 * @code
 *      // Create the TC frame for node 1 with 4 commands
 *      char *tc_frame = "1 ping 10;print_vars;send_status 10"
 *
 *      // Case 1: Call the command directly
 *      com_send_data("%d %s", tc_frame, 2);
 *
 *      // Case 2: Call the command from repoCommand
 *      cmd_t *send_cmd = cmd_get_str("send_tc");          // Get the command
 *      cmd_add_params(send_cmd, tc_frame); // Add params as binary data
 *      cmd_send(send_cmd);
 * @endcode
 *
 */
int com_send_tc_frame(char *fmt, char *params, int nparams);

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
 * Set module global variable trx_node. Future command calls will use this node
 *
 * @param fmt Str. Parameters format: "%d"
 * @param params Str. Parameters: <node>, the TRX node number
 * @param nparams Str. Number of parameters: 1
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors.
 */
int com_set_node(char *fmt, char *params, int nparams);

/**
 * Show current trx_node setting. @see com_set_node
 * @param fmt Not used
 * @param params Not used
 * @param nparams Not used
 * @return CMD_OK
 */
int com_get_node(char *fmt, char *params, int nparams);

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
 * Get TRX settings values. The TRX has a list of parameters to set and
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
 * @endcode
 *
 */
int com_get_config(char *fmt, char *params, int nparams);

/**
 * Set TRX settings values. The TRX has a list of parameters to set and
 * get (@see ax100_param.h and ax100_param_radio.h). Use this command to set
 * any parameter value by name. The special argument 'help 0' can be
 * used to print the list of available parameters.
 *
 * @param fmt Str. Parameters format: "%s %s"
 * @param params Str. Parameters: <param_name> <param_value>, the parameter name
 * and value as strings.
 * @param nparams Str. Number of parameters: 2
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors.
 *
 * @code
 *      // Function usage
 *      com_set_config("%s %s", "help 0", 1);     // Print the parameter list
 *      com_set_config("%s %s", "csp_node 5", 1); // Set and print the TRX node
 *
 *      // Command usage to set a TRX parameter
 *      cmd_t *send_cmd = cmd_get_str("com_set_config"); // Get the command
 *      cmd_add_params(send_cmd, "tx_pwr 0")  // Set param "tx_pwr" to 0
 *      cmd_send(send_cmd);
 * @endcode
 *
 */
int com_set_config(char *fmt, char *params, int nparams);

/* TODO: Add documentation */
int com_update_status_vars(char *fmt, char *params, int nparams);

#endif /* CMD_COM_H */
