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
#include "config.h"
#include "utils.h"

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
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int eps_hard_reset(char *fmt, char *params, int nparams);

/**
 * Get and pretty print EPS housekeeping information like voltage, current, etc.
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int eps_get_hk(char *fmt, char *params, int nparams);

/**
 * Get and pretty print EPS settings.
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int eps_get_config(char *fmt, char *params, int nparams);

/**
 * Set the the heater modes. NanoPower P31u can control the on-board heaters
 * or the BPX heater. Set the heater (0: external, 1: on-board, 2: both) and the
 * mode (0: off, 1: on).
 *
 * @param fmt Str. Parameters format "%d %d>"
 * @param params Str. Parameters as string "<heater> <mode>", ex: "1 1"
 * @param nparams Int. Number of parameters 2
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
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
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int eps_update_status_vars(char *fmt, char *params, int nparams);

#endif //CMDEPS_H
