/**
 * @file  cmdADCS.h
 * @author Carlos Gonzalez C carlgonz@uchile.cl
 * @author Camilo Rojas M - camrojas@uchile.cl
 * @author Gustavo Diaz H - gustavo.diaz@ing.uchile.cl
 * @author Elias Obreque S - elias.obreque@uchile.cl
 * @author Javier Morales R - javiermoralesr95@gmail.com
 * @author Luis Jimenez V - luis.jimenez@ing.uchile.cl
 * @date 2021
 * @copyright GNU Public License.
 *
 * This header contains commands related with the ADCS system
 */

#ifndef _CMDADCS_H
#define _CMDADCS_H

#include <math.h>

#include "drivers.h"
#include "csp/csp.h"
#include "suchai/math_utils.h"

#include "suchai/repoData.h"
#include "suchai/repoCommand.h"
#include "suchai/cmdCOM.h"
#include "suchai/math_utils.h"
#include "suchai/log_utils.h"

/**
 * Register TLE commands
 */
void cmd_tle_init(void);

/**
 * Log current TLE lines
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int tle_get(char *fmt, char *params, int nparams);

/**
 * Set TLE lines, once a at time. Call this command two times, one for each
 * line to set up the TLE. The command recognizes the line number by the first
 * parameter of each TLE line, the stores the full file.
 * @warning This command do not update the TLE structure. It only saves the TLE
 * lines. Normally it requires to also call the tle_update command.
 *
 * Example 1, from code:
 * @code
 *  cmd_t *tle1 = cmd_get_str("tle_set");
 *  cmd_add_params_str(tle1, "1 42788U 17036Z   20027.14771603  .00000881  00000-0  39896-4 0  9992");
 *  cmd_send(tle1);
 *
 *  cmd_t *tle2 = cmd_get_str("tle_set");
 *  cmd_add_params_str(tle2, "2 42788  97.3234  85.2817 0012095 159.3521 200.8207 15.23399088144212");
 *  cmd_send(tle2);
 *
 *  cmd_t *tle_u = cmd_get_str("tle_update");
    cmd_send(tle_u);
 * @endcode
 *
 * Example 2, from the serial console or telecommand
 * @code
 * tle_set 1 42788U 17036Z   20027.14771603  .00000881  00000-0  39896-4 0  9992
 * tle_set 2 42788  97.3234  85.2817 0012095 159.3521 200.8207 15.23399088144212
 * tle_update
 * @endcode
 *
 * @param fmt Str. Parameters format "%d %n"
 * @param params Str. Parameters as string "<line (69 chars including line number)>"
 * @param nparams Int. Number of parameters 2
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int tle_set(char *fmt, char *params, int nparams);

/**
 * Update the TLE structure using the received TLE lines
 * @warning This command do not receive the TLE lines. It only updates the TLE
 * structure. Normally it requires to also call the tle_set command.
 *
 * Example 1, from code:
 * @code
 *  cmd_t *tle1 = cmd_get_str("tle_set");
 *  cmd_add_params_str(tle1, "1 42788U 17036Z   20027.14771603  .00000881  00000-0  39896-4 0  9992");
 *  cmd_send(tle1);
 *
 *  cmd_t *tle2 = cmd_get_str("tle_set");
 *  cmd_add_params_str(tle2, "2 42788  97.3234  85.2817 0012095 159.3521 200.8207 15.23399088144212");
 *  cmd_send(tle2);
 *
 *  cmd_t *tle_u = cmd_get_str("tle_update");
    cmd_send(tle_u);
 * @endcode
 *
 * Example 2, from the serial console or telecommand
 * @code
 * tle_set 1 42788U 17036Z   20027.14771603  .00000881  00000-0  39896-4 0  9992
 * tle_set 2 42788  97.3234  85.2817 0012095 159.3521 200.8207 15.23399088144212
 * tle_update
 * @endcode
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int tle_update(char *fmt, char *params, int nparams);

/**
 * Propagate the TLE to the given datetime to update satellite position in ECI
 * reference. The result is stored in the system variables. The command receives
 * the datetime to propagate in unix timestamp format. If the parameter is 0
 * then uses the current date and time.
 * @warning This command do not receive the TLE lines, nor update the TLE.
 * It only propagates the TLE. Normally it requires to also call the
 * tle_set and obc_update command.
 *
 * Example 1, from code:
 * @code
 *  // Set and update the TLE previously
 *  cmd_t *cmd_prop = cmd_get_str("tle_prop");
 *  cmd_add_params_var(cmd_prop, dt, 1582643144);
 *  cmd_send(cmd_prop);
 * @endcode
 *
 * Example 2, from the serial console or telecommand
 * @code
 * // Prop. TLE to datetime
 * tle_prop 1582643144
 * // Prop. TLE to current datetime
 * tle_prop 0
 * @endcode
 *
 * @param fmt Str. Parameters format "%ld"
 * @param params Str. Parameters as string "<unix_timestamp | 0>"
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int tle_prop(char *fmt, char *params, int nparams);

#endif //_CMDADCS_H
