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

#include "config.h"

#include "repoData.h"
#include "repoCommand.h"

#define CMD_DRP 0x50 ///< DRP commands group identifier

void cmd_repodata_init(void);

/* TODO: Move documentation to his header */
int drp_update_dat_CubesatVar_hoursWithoutReset(int nparam, void *param);
int drp_print_dat_CubesatVar(int nparam, void *param);

#endif /* CMD_PCC_H */
