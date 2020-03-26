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

#include "config.h"

#include "repoData.h"
#include "repoCommand.h"

#define SCH_DRP_MAGIC (1010)    ///< Magic number to execute critical commands

typedef union sensors_value{
    float f;
    int32_t i;
} value;

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
 * @param nparams Int. Number of parameters: 0
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int drp_execute_before_flight(char *fmt, char *params, int nparams);

/**
 * Display system related variables. Variables are read from system variables
 * data repository
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int drp_print_system_vars(char *fmt, char *params, int nparams);

/**
 * Update a system status variable <value> by <index>
 *
 * @param fmt Str. Parameters format "%d %d"
 * @param params Str. Parameters as string "<index> <value>"
 * @param nparams Int. Number of parameters 2
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int drp_update_sys_var_idx(char *fmt, char *params, int nparams);

/**
 * Update current hours alive and hours without reset counters adding <value>
 * hours.
 *
 * @param fmt Str. Parameters format "%d"
 * @param params Str. Parameters as string "<value>"
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int drp_update_hours_alive(char *fmt, char *params, int nparams);

/**
 * Clear the GND watchdog timer counter to prevent the system reset. This
 * command should be executed periodically from the ground station.<
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int drp_clear_gnd_wdt(char *fmt, char *params, int nparams);

/**
 * Tests the data repository functioning, sets a value for every system status
 * variable and reads it back, comparing it with the set value.
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int drp_test_system_vars(char *fmt, char *params, int nparams);

/**
 * Set the variable `dat_dep_deployed` to a given value. This variable is used
 * to determine if the satellite was deployed. If not, then the satellite
 * must execute deployment routines. If deployed, deployment routines are
 * skipped and the satellite is working nominally. Setting this variable to
 * zero can cause the satellite to re-execute initialization activities (e.g.
 * al settings to default values) after a reset.
 *
 * @param fmt Str. Parameters format "%d"
 * @param params Str. Parameters as string "<deployed>"
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int drp_set_deployed(char *fmt, char *params, int nparams);

#endif /* CMD_DRP_H */
