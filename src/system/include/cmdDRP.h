/**
 * @file  cmdPPC.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @date 2018
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related following data repositories:
 *  *dat_CubesatVar     -> dat_
 *
 * Commands are named with the following convention:
 *  * Update variables  -> drp_update_
 *  * Show variables    -> drp_print_
 *
 */

#ifndef CMD_DRP_H
#define CMD_DRP_H

#include "config.h"

#include "repoData.h"
#include "repoCommand.h"


/**
 * Register data repository (DRP) commands in the system
 */
void cmd_drp_init(void);

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

#endif /* CMD_DRP_H */
