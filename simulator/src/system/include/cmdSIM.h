/**
 * @file  cmdSIM.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Camilo Rojas C - camrojas@uchile.cl
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to the flight software
 * simulator
 */

#ifndef _CMDSIM_H
#define _CMDSIM_H

#include <math.h>

#include "csp/csp.h"

#include "log_utils.h"
#include "math_utils.h"

#include "repoData.h"
#include "repoCommand.h"
#include "cmdCOM.h"

void cmd_sim_init(void);

/**
 * Send "adcs_point_to <x> <y> <z>" command to the ADCS system with current
 * position vector.
 *
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return CMD_OK | CMD_FAIL | CMD_ERROR
 */
int sim_adcs_point(char* fmt, char* params, int nparams);

/**
 * Read current spacecraft quaternion from the ADCS/STT
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return CMD_OK | CMD_FAIL | CMD_ERROR
 */
int sim_adcs_get_quaternion(char* fmt, char* params, int nparams);

/**
 * Read current accelerations from ADCS
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return
 */
int sim_adcs_get_acc(char* fmt, char* params, int nparams);

/**
 * Read current magnetic sensors from ADCS
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return
 */
int sim_adcs_get_mag(char* fmt, char* params, int nparams);

/**
 *
 * @param fmt
 * @param params
 * @param nparams
 * @return
 */
int sim_adcs_control_torque(char* fmt, char* params, int nparams);

/**
 *
 * @param fmt
 * @param params
 * @param nparams
 * @return
 */
int sim_adcs_mag_moment(char* fmt, char* params, int nparams);

/**
 * Set ADCS vector (Intertial frame) and velocity (body frame) targets
 * @param fmt "%lf lf lf lf lf lf"
 * @param params "<x y z> <wx wy wz>"
 * @param nparams 6
 * @return CMD_OK | CMD_ERROR | CMD_FAIL
 */
int sim_adcs_set_target(char* fmt, char* params, int nparams);

/**
 * Set ADCS target to Nadir based on current quaternion and position
 * Uses sim_adcs_set_target
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return CMD_OK | CMD_ERROR | CMD_FAIL
 */
int sim_adcs_target_nadir(char* fmt, char* params, int nparams);

/**
 * Set ADCS in detumbling mode
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return CMD_OK | CMD_ERROR | CMD_FAIL
 */
int sim_adcs_detumbling_mag(char* fmt, char* params, int nparams);

/**
 * Send current attitude variables to ADCS system. For testing purposes.
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return CMD_OK | CMD_ERROR | CMD_FAIL
 */
int sim_adcs_send_attitude(char* fmt, char* params, int nparams);

#endif //_CMDSIM_H
