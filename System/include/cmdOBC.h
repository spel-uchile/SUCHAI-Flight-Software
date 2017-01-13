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
#include "repoCommand.h"

#if __linux__
    /* FIXME: Find an adequate implementation in unix */
	#define OBC_SYS_RESET() { printf("reset\n");} ///< processor software reset
#else
	#define OBC_SYS_RESET() { __asm__ volatile("reset");}///< processor software reset
#endif

#define CMD_OBC 0x10 ///< OBC commands group identifier


void cmd_obc_init(void);

int obc_reset(char *fmt, char *params, int nparams);
int obc_get_rtos_memory(char *fmt, char *params, int nparams);


#endif /* CMD_OBC_H */
