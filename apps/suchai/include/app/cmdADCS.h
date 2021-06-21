/**
 * @file  cmdCOM.h
 * @author Carlos Gonzalez C carlgonz@uchile.cl
 * @author Camilo Rojas M - camrojas@uchile.cl
 * @author Gustavo Diaz H - gustavo.diaz@ing.uchile.cl
 * @author Elias Obreque S - elias.obreque@uchile.cl
 * @author Javier Morales R - javiermoralesr95@gmail.com
 * @author Luis Jimenez V - luis.jimenez@ing.uchile.cl
 * @date 2020
 * @copyright GNU Public License.
 *
 * This header contains commands related with the ADCS system
 */

#ifndef _CMDADCS_H
#define _CMDADCS_H

#include <math.h>

#include "drivers.h"
#include "csp/csp.h"
#include "math_utils.h"

#include "repoData.h"
#include "repoCommand.h"
#include "cmdCOM.h"
#include "math_utils.h"
#include "suchai/log_utils.h"

void cmd_adcs_init(void);

/**
 * Send "adcs_point_to <x> <y> <z>" command to the ADCS system with current
 * position vector.
 *
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return CMD_OK | CMD_ERROR_SYNTAX | CMD_ERROR
 */
int adcs_point(char* fmt, char* params, int nparams);

/**
 * Read current spacecraft quaternion from the ADCS/STT
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return CMD_OK | CMD_ERROR_SYNTAX | CMD_ERROR
 */
int adcs_get_quaternion(char* fmt, char* params, int nparams);

/**
 * Read gyroscopes. Update values in status variables.
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return CMD_OK | CMD_ERROR_SYNTAX | CMD_ERROR
 */
int adcs_get_omega(char* fmt, char* params, int nparams);

/**
 * Read magnetic sensors. Update values in status variables.
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return CMD_OK | CMD_ERROR_SYNTAX | CMD_ERROR
 */
int adcs_get_mag(char* fmt, char* params, int nparams);

/**
 *
 * @param fmt Str. Parameters format "%lf"
 * @param params Str. Parameters as string: "<control cycle>"
 * @param nparams Int. Number of parameters 1
 * @return CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int adcs_control_torque(char* fmt, char* params, int nparams);

/**
 *
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int adcs_mag_moment(char* fmt, char* params, int nparams);

/**
 * Set ADCS vector (Intertial frame) and velocity (body frame) targets
 * @param fmt "%lf lf lf lf lf lf"
 * @param params "<x y z> <wx wy wz>"
 * @param nparams 6
 * @return CMD_OK | CMD_ERROR | CMD_ERROR_SYNTAX
 */
int adcs_set_target(char* fmt, char* params, int nparams);

/**
 * Set ADCS target to Nadir based on current quaternion and position
 * Uses sim_adcs_set_target
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return CMD_OK | CMD_ERROR | CMD_ERROR_SYNTAX
 */
int adcs_target_nadir(char* fmt, char* params, int nparams);

/**
 * Set ADCS in detumbling mode
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return CMD_OK | CMD_ERROR | CMD_ERROR_SYNTAX
 */
int adcs_detumbling_mag(char* fmt, char* params, int nparams);

/**
 * Send current attitude variables to ADCS system. For testing purposes.
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return CMD_OK | CMD_ERROR | CMD_ERROR_SYNTAX
 */
int adcs_send_attitude(char* fmt, char* params, int nparams);

#endif //_CMDADCS_H
