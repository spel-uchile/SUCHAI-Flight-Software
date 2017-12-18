//
// Created by grynn on 15-12-17.
//

#ifndef SUCHAI_FLIGHT_SOFTWARE_CMDFLIGHTPLAN_C_H
#define SUCHAI_FLIGHT_SOFTWARE_CMDFLIGHTPLAN_C_H

#endif //SUCHAI_FLIGHT_SOFTWARE_CMDFLIGHTPLAN_C_H


#include <repoCommand.h>
#include "data_storage.h"

void cmd_fp_init(void);
int fp_set(char *fmt, char *params, int nparams);
int fp_delete(char* fmt, char* params, int nparams);
int fp_show(char* fmt, char* params, int nparams);
int fp_reset(char* fmt, char* params, int nparams);
