/**
 * @file  cmdTM.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2021
 * @copyright GNU Public License.
 *
 * This header contains commands related with telemetry proccessing and
 * downloading.
 */

#ifndef CMDTM_H
#define CMDTM_H

#include <string.h>
#include "suchai/config.h"
#include "suchai/log_utils.h"

#include "suchai/repoCommand.h"
#include "suchai/repoData.h"
#include "suchai/cmdCOM.h"

#define TM_TYPE_GENERIC 0
#define TM_TYPE_STATUS  1
#define TM_TYPE_HELP    2
#define TM_TYPE_PAYLOAD 10
#define TM_TYPE_FILE_START 100
#define TM_TYPE_FILE_DATA 101
#define TM_TYPE_FILE_END 102
#define TM_TYPE_FILE_PART 103

/**
 * Register TM commands
 */
void cmd_tm_init(void);

/**
 * Helper function to read and send a range of telemetry
 * @param start Starting index
 * @param end Stop index
 * @param payload Payload id
 * @param dest_node Node to send TM
 * @return CMD_OK, CMD_ERROR, or CMD_SYNTAX_ERROR
 */
int tm_send_tel_from_to(int start, int end, int payload, int dest_node);

/**
 * Send status variables as telemetry. This command collects the current value
 * of all status variables, builds a frame and downloads telemetry to the
 * specified node. To parse the data @seealso tm_parse_status
 *
 * @param fmt Str. Parameters format: "%d"
 * @param param Str. Parameters as string, node to send TM: <node>. Ex: "10"
 * @param nparams Int. Number of parameters: 1
 * @return CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int tm_send_status(char *fmt, char *params, int nparams);

/**
 * Send specific status variable as telemetry. This command collects the current value
 * of the status variable, builds a frame and downloads telemetry to the
 * specified node. To parse the data @seealso tm_parse_status
 *
 * @param fmt Str. Parameters format: "%d %s"
 * @param param Str. Parameters as string, node to send TM and variable name: <node> <variable name>. Ex: "10 obc_last_reset"
 * @param nparams Int. Number of parameters: 2
 * @return CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int tm_send_var(char *fmt, char *params, int nparams);

/**
 * Parses a status variables telemetry, @seealso tm_send_status.
 * @warning Avoid using this command from command line, or tele-command
 *
 * @param fmt Str. Not used.
 * @param param char *. Parameters as pointer to raw data. Receives a com_frame_t structure with an array of
 * dat_sys_var_short_t structs in frame->data
 * @param nparams Int. Not used.
 * @return CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int tm_parse_status(char *fmt, char *params, int nparams);

/**
 * Parses a commands list as string, @seealso tm_send_cmds.
 *
 * @param fmt Str. Not used.
 * @param param char *. Parameters as pointer to raw data. Receives a
 * status_t structure
 * @param nparams Int. Not used.
 * @return CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int tm_parse_string(char *fmt, char *params, int nparams);

/**
 * Print last struct data stored from specific payload.
 * @param fmt "%u"
 * @param params "<payload> "
 * @param nparams 1
 * @return CMD_OK, CMD_ERROR, or CMD_ERROR_SYNTAX
 */
int tm_get_last(char *fmt, char *params, int nparams);

/**
 * Send last structs data stored as payload in csp frame.
 * @param fmt "%u %u"
 * @param params "<destination node> <payload> "
 * @param nparams 2
 * @return CMD_OK, CMD_ERROR, or CMD_ERROR_SYNTAX
 */
int tm_send_last(char *fmt, char *params, int nparams);

/**
 * Send all structs data stored as payload in multiple csp frames from last acknowledge.
 * @param fmt "%u %u"
 * @param params "<destination node> <payload>"
 * @param nparams 2
 * @return CMD_OK, CMD_ERROR, or CMD_ERROR_SYNTAX
 */
int tm_send_all(char *fmt, char *params, int nparams);

/**
 * Send k structs data stored as payload in multiple csp frames form last acknowledge.
 * @param fmt "%u %u %u"
 * @param params "<destination node> <payload> <k samples>"
 * @param nparams 3
 * @return CMD_OK, CMD_ERROR, or CMD_ERROR_SYNTAX
 */
int tm_send_from(char *fmt, char *params, int nparams);

/**
 * Parses a payload data telemetry, @seealso tm_send_all.
 * @warning Avoid using this command from command line, or tele-command
 *
 * @param fmt Str. Not used.
 * @param param char *. Parameters as pointer to raw data. Receives a com_frame_t structure with an array of
 * payload structs in frame->data
 * @param nparams Int. Not used.
 * @return CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int tm_parse_payload(char *fmt, char *params, int nparams);

/**
 * Acknowledge k samples of a payload.
 * @param fmt "%u %u"
 * @param params "<payload> <k samples>"
 * @param nparams 2
 * @return CMD_OK, CMD_ERROR, or CMD_ERROR_SYNTAX
 */
int tm_set_ack(char *fmt, char *params, int nparams);


/**
 *
 * @param fmt %u %u
 * @param params "<payload> <index>"
 * @param nparams 2
 * @return CMD_OK, CMD_ERROR, or CMD_ERROR_SYNTAX
 */
int tm_get_single(char *fmt, char *params, int nparams);

int tm_send_cmds(char *fmt, char *params, int nparms);

#ifdef LINUX
/**
 * Send a file using CSP
 *
 * @param fmt %s %u
 * @param params "<filename> <node>"
 * @param nparams 2
 * @return CMD_OK, CMD_ERROR, or CMD_ERROR_SYNTAX
 */
int tm_send_file(char *fmt, char *params, int nparams);

/**
 * Parse a file from CSP packets
 *
 * @param fmt ""
 * @param params "<binary_blob>"
 * @param nparams 0
 * @return CMD_OK, CMD_ERROR, or CMD_ERROR_SYNTAX
 */
int tm_parse_file(char *fmt, char *params, int nparams);

/**
 * Send specific frames of a file using CSP
 * @param fmt %s %d %d %d %u
 * @param params "<filename> <file_id> <start> <end> <node>"
 * @param nparams 4
 * @return CMD_OK, CMD_ERROR, or CMD_ERROR_SYNTAX
 */
int tm_send_file_parts(char *fmt, char *params, int nparams);

/**
 * Merge files transferred with tm_send_file_parts
 * @param fmt %s %d
 * @param params "<filename> <file_id>"
 * @param nparams 2
 * @return CMD_OK, CMD_ERROR, or CMD_ERROR_SYNTAX
 */
int tm_merge_file(char *fmt, char *params, int nparams);

/**
 * List a directory and send that data to a remote node
 * Use path as . to list current working directory
 * Similar to obc_ls command, but this send each filename to a remote node
 * as a CSP frame.
 *
 * @param fmt %s %d
 * @param params "<path> <node>"
 * @param nparams 2
 * @return CMD_OK, CMD_ERROR, or CMD_ERROR_SYNTAX
 */
int tm_list_files(char* fmt, char* params, int nparams);
#endif

#endif //CMDTM_H
