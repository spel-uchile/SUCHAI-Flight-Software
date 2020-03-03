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

#include "csp/csp.h"

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
 *
 * @param fmt
 * @param params
 * @param nparams
 * @return
 */
int sim_adcs_control_torque(char* fmt, char* params, int nparams);

#endif //_CMDSIM_H
