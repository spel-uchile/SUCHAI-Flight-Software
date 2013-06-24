/**
 * @file  cmdPPC.h
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @author Carlos Gonzalez C - carlgonz@ug.uchile.cl
 * @date 2012
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to the on borad computer
 * (OBC) features.
 */

#ifndef CMD_OBC_H
#define CMD_OBC_H

#include "SUCHAI_config.h"

#define OBC_SYS_RESET() {__asm__ volatile("reset");} ///< processor software reset

#define CMD_OBC 0x10 ///< OBC commands group identifier

/**
 * List of availible commands
 */
typedef enum{
    obc_id_reset = 0x1000, ///< @cmd_first
    obc_id_get_rtos_memory, ///< @cmd
        
    obc_id_last_one    // Dummy element
}OBC_CmdIndx;

#define OBC_NCMD ((unsigned char)obc_id_last_one)

void obc_onResetCmdOBC(void);

int obc_reset(void* param);
int obc_get_rtos_memory(void *param);


#endif /* CMD_PCC_H */
