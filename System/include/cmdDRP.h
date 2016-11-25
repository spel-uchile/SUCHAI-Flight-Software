/**
 * @file  cmdPPC.h
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @author Carlos Gonzalez C - carlgonz@ug.uchile.cl
 * @date 2012
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

#include "../../SUCHAI_config.h"

#include "repoData.h"

#define CMD_DRP 0x50 ///< DRP commands group identifier

/**
 * List of availible commands
 */
typedef enum{
    drp_id_update_dat_CubesatVar_hoursWithoutReset = 0x5000, ///< @cmd_first
    drp_id_update_dat_CubesatVar_hoursAlive, ///< @cmd
    drp_id_print_CubesatVar, ///< @cmd

    drp_id_last_one    // Dummy element
}DRP_CmdIndx;

#define DRP_NCMD ((unsigned char)drp_id_last_one)

void drp_onResetCmdDRP(void);

int drp_update_dat_CubesatVar_hoursWithoutReset(void *param);
int drp_print_dat_CubesatVar(void *param);

#endif /* CMD_PCC_H */
