/**
 * @file  cmdEPS.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2020
 * @copyright GNU Public License.
 *
 * This header contains commands related with the EPS subsystem
 */

#ifndef CMDEPS_H
#define CMDEPS_H

#include "drivers.h"
#include "suchai/config.h"
#include "suchai/log_utils.h"

#include "repoCommand.h"
#include "repoData.h"

/**
 * Register EPS commands
 */
void cmd_eps_init(void);

/**
 * Send the hard reset command to the EPS
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int eps_hard_reset(char *fmt, char *params, int nparams);

/**
 * Get and pretty print EPS housekeeping information like voltage, current, etc.
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int eps_get_hk(char *fmt, char *params, int nparams);

/**
 * Get and pretty print EPS settings.
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int eps_get_config(char *fmt, char *params, int nparams);

/**
 * Set the the heater modes. NanoPower P31u can control the on-board heaters
 * or the BPX heater. Set the heater (0: external, 1: on-board, 2: both) and the
 * mode (0: off, 1: on).
 *
 * @param fmt Str. Parameters format <"%d %d>"
 * @param params Str. Parameters as string "<heater> <mode>", ex: "1 1"
 * @param nparams Int. Number of parameters 2
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 *
 * @code
 *      // Function usage
 *      eps_set_heater("%d "%d", "1 1", 2); // Turn on on board heater
 *      eps_set_heater("%d "%d", "2 0", 2); // Turn off both heaters
 *
 *      // Send a command to turn on the on-board heater
 *      cmd_t *send_cmd = cmd_get_str("eps_set_heater"); // Get the command
 *      cmd_add_params_var(send_cmd, 1, 1); // Turn on on board heater
 *      cmd_send(send_cmd);
 *
 *      // Send a command to turn off both heaters
 *      cmd_t *send_cmd = cmd_get_str("eps_set_heater"); // Get the command
 *      cmd_add_params_var(send_cmd, 2, 0); // Turn off both heaters
 *      cmd_send(send_cmd);
 * @endcode
 */
int eps_set_heater(char *fmt, char *params, int nparams);

/**
 * Update EPS related status system vars.
 * Read EPS hk and set status variables
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int eps_update_status_vars(char *fmt, char *params, int nparams);

/**
 * Turn on/off EPS User outputs
 *
 * @param fmt Str. Parameters format "<%d> <%d>"
 * @param params Str. Parameters as string "<output> <on/off>"
 * @param nparams Int. Number of parameters 2
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 *
 * @code
 *      // Function usage
 *      eps_set_output("%d "%d", "1 1", 2); // Turn on output 1
 *      eps_set_output("%d "%d", "5 0", 2); // Turn off output 5
 *
 *      // Send a command to turn on the on-board heater
 *      cmd_t *send_cmd = cmd_get_str("eps_set_output"); // Get the command
 *      cmd_add_params_var(send_cmd, 3, 1); // Turn on output 3
 *      cmd_send(send_cmd);
 * @endcode
 *
 */
int eps_set_output(char *fmt, char *params, int nparams);

/**
 * Turn on/off al user outputs
 * @param fmt Str. Parameters format "<%d>"
 * @param params Str. Parameters as string "<0|1>"
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int eps_set_output_all(char *fmt, char *params, int nparams);

/**
 * Set boost converter fixed voltage (used in ppt manual mode) in mV
 * @param fmt Str. Parameters format "<%d>"
 * @param params Str. Parameters as string "<mV>"
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int eps_set_vboost(char *fmt, char *params, int nparams);

/**
 * Set PPT mode
 *      0 = HW, 1 = MPPT, 2 = FIXED
 * @param fmt Str. Parameters format "<%d>"
 * @param params Str. Parameters as string "<0|1||2>"
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int eps_set_pptmode(char *fmt, char *params, int nparams);

/**
 * Kick EPS ground WDT
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int eps_reset_wdt(char *fmt, char *params, int nparams);

#endif //CMDEPS_H
