/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2013, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
 *      Copyright 2013, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "include/cmdDRP.h"

cmdFunction drp_Function[DRP_NCMD];
int drp_sysReq[DRP_NCMD];

/**
 * This function registers the list of command in the system, initializing the
 * functions array. This function must be called at every system start up.
 */
void drp_onResetCmdDRP(void)
{
    drp_Function[(unsigned char)drp_id_update_dat_CubesatVar_hoursWithoutReset] = drp_update_dat_CubesatVar_hoursWithoutReset;
    drp_sysReq[(unsigned char)drp_id_update_dat_CubesatVar_hoursWithoutReset]  = CMD_SYSREQ_MIN;
    drp_Function[(unsigned char)drp_id_print_CubesatVar] = drp_print_dat_CubesatVar;
    drp_sysReq[(unsigned char)drp_id_print_CubesatVar]  = CMD_SYSREQ_MIN;
}

/**
 * Updates the following status variables:
 *  - hoursWithoutReset
 *  - hoursAlive
 *
 * The comands adds the value of param to current variable's value
 * 
 * @param param Hours to be added
 * @return 1, success. 0, fail.
 */
int drp_update_dat_CubesatVar_hoursWithoutReset(void *param)
{
    int arg, variable;

    arg = *((int *)param);
    variable = dat_getCubesatVar(dat_obc_hoursWithoutReset);
    dat_setCubesatVar(dat_obc_hoursWithoutReset, variable + arg);

    variable = dat_getCubesatVar(dat_obc_hoursAlive);
    dat_setCubesatVar(dat_obc_hoursAlive, variable + arg);

    return 1;
}

/**
 * Print all system's status variables.
 * 
 * @param param Not used
 * @return 1, success.
 */
int drp_print_dat_CubesatVar(void *param)
{
    printf("===================================\n");
    printf("        Status repository\n");
    printf("===================================\n");

    DAT_CubesatVar indxVar;
    for(indxVar=0; indxVar<dat_cubesatVar_last_one; indxVar++)
    {
        int var = dat_getCubesatVar(indxVar);
        printf("%d, %d\n", indxVar, var);
    }

    return 1;
}