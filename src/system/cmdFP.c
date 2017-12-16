//
// Created by grynn on 15-12-17.
//

#include "cmdFP.h"

static const char* tag = "cmdFlightPlan";

/**
 * This function registers the list of command in the system, initializing the
 * functions array. This function must be called at every system start up.
 */
void cmd_fp_init(void)
{
    cmd_add("fp_set_command", fp_set, "%d %s %s %d %s", 5);

}

int fp_set(char *fmt, char *params, int nparams)
{
    int timetodo;
    char command[CMD_MAX_STR_PARAMS];
    char args[CMD_MAX_STR_PARAMS];
    int repeat;
    char table[CMD_MAX_STR_PARAMS];

    if(sscanf(params, fmt, &timetodo, &command, &args, &repeat, &table) == nparams) {

        int rc = storage_flight_plan_set(timetodo, command, args, repeat, table);

        if (rc == 0)
            return CMD_OK;
        else if (rc == -1)
            return CMD_FAIL;
    }
}

