/**
 * @file  cmdFP.h
 * @author Matias Ramirez M  - nicoram.mt@gmail.com
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2017
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to the flight plan
 */


#ifndef CMD_FLIGHTPLAN_H
#define CMD_FLIGHTPLAN_H

#include "data_storage.h"

#include "repoCommand.h"

/* TODO: Documentation required */
void cmd_fp_init(void);
int fp_set(char *fmt, char *params, int nparams);
int fp_delete(char* fmt, char* params, int nparams);
int fp_show(char* fmt, char* params, int nparams);
int fp_reset(char* fmt, char* params, int nparams);

#endif //CMD_FLIGHTPLAN_H
