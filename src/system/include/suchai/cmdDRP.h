/**
 * @file  cmdDRP.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related with data repositories
 *
 */

#ifndef CMD_DRP_H
#define CMD_DRP_H

#include "suchai/config.h"

#include "suchai/repoData.h"
#include "suchai/repoCommand.h"

#define SCH_DRP_MAGIC (1010)    ///< Magic number to execute critical commands

/**
 * Register data repository (DRP) commands in the system
 */
void cmd_drp_init(void);

/**
 * Execute before flight. This command reset the satellite status variables to
 * their default settings. The default value depends on the variable; however
 * the value represents the status of the satellite just before the launch.
 * launch.
 * The command is executed with a "Magic Number" as parameter to avoid
 * unintended calls. See @SCH_DRP_MAGIC.
 *
 * @param fmt Str. Parameters format: "%d"
 * @param params Str. Parameters as string: "<MAGIC_NUMBER>"
 * @param nparams Int. Number of parameters: 1
 * @return CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int drp_execute_before_flight(char *fmt, char *params, int nparams);

/**
 * This command resets a specific payload table, deleting all its content.
 * The command is executed with a "Magic Number" as a parameter to avoid
 * unintended calls. See @SCH_DRP_MAGIC.
 * @param fmt "%d %d"
 * @param params <payload> <MAGIC_NUMBER>
 * @param nparams Number of parameters: 2
 * @return CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int drp_reset_payload(char *fmt, char *params, int nparams);

/**
 * This command resets all status variables to their default values
 * The command is executed with a "Magic Number" as a parameter to avoid
 * unintended calls. See @SCH_DRP_MAGIC.
 * @param fmt "%d%
 * @param params <MAGIC_NUMBER>
 * @param nparams Number of parameters: 1
 * @return CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int drp_reset_status(char *fmt, char *params, int nparams);

/**
 * Display system related variables. Variables are read from system variables
 * data repository
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int drp_print_system_vars(char *fmt, char *params, int nparams);

/**
 * Update a system status variable by index (address)
 * Support int, uint and float parameters
 *
 * @param fmt Str. Parameters format "%d %f"
 * @param params Str. Parameters as string "<address> <value>"
 * @param nparams Int. Number of parameters 2
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 *
 * @code
 * //From console
 * drp_set_var 1 123
 * drp_set_var 3 -4.25
 *
 * //From code
 * cmd_t cmd = cmd_get_str("drp_set_var");
 * cmd_add_params(&cmd, 1, 123)
 * cmd_send(&cmd)
 *
 * cmd_t cmd = cmd_get_str("drp_set_var");
 * cmd_add_params(&cmd, 3, -4.25)
 * cmd_send(&cmd)
 * @endcode
 */
int drp_update_sys_var_idx(char *fmt, char *params, int nparams);

/**
 * Update a system status variable by name
 * Support int, uint and float parameters
 *
 * @param fmt Str. Parameters format "%s %f"
 * @param params Str. Parameters as string "<name> <value>"
 * @param nparams Int. Number of parameters 2
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 *
 *  * @code
 * //From console
 * drp_set_var_name obc_op_mode 123
 * drp_set_var_name tgt_omega_x -4.25
 *
 * //From code
 * cmd_t cmd = cmd_get_str("drp_set_var_name");
 * cmd_add_params(&cmd, "obc_op_mode", 123)
 * cmd_send(&cmd)
 *
 * cmd_t cmd = cmd_get_str("drp_set_var_name");
 * cmd_add_params(&cmd, "tgt_omega_x", -4.25)
 * cmd_send(&cmd)
 * @endcode
 *
 */
int drp_update_sys_var_name(char *fmt, char *params, int nparams);

/**
 * Print a system status variable value by name
 * *
 * @param fmt Str. Parameters format "%s"
 * @param params Str. Parameters as string "<name>"
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int drp_get_sys_var_name(char *fmt, char *params, int nparams);

/**
 * Update current hours alive and hours without reset counters adding <value>
 * hours.
 *
 * @param fmt Str. Parameters format "%d"
 * @param params Str. Parameters as string "<value>"
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int drp_update_hours_alive(char *fmt, char *params, int nparams);

/**
 * Clear the GND watchdog timer counter to prevent the system reset. This
 * command should be executed periodically from the ground station.<
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int drp_clear_gnd_wdt(char *fmt, char *params, int nparams);

#endif /* CMD_DRP_H */
