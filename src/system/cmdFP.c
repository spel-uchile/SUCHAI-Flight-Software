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
    cmd_add("fp_delete_command", fp_delete, "%d %s", 2);
    cmd_add("fp_show", fp_show, "%s", 1);
    cmd_add("fp_reset", fp_reset,"%s", 1);

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

int fp_delete(char* fmt, char* params, int nparams){

    int timetodo;
    char table[CMD_MAX_STR_PARAMS];

    if(sscanf(params, fmt, &timetodo, &table) == nparams){
        int rc = storage_flight_plan_erase(timetodo, table);

        if(rc==0)
            return CMD_OK;
        else if(rc == -1)
            return CMD_FAIL;
        else
            return CMD_ERROR;
    }
}

int fp_show(char* fmt, char* params, int nparams)
{
    char table[CMD_MAX_STR_PARAMS];

    if(sscanf(params, fmt, &table) == nparams) {
        int rc= storage_show_table(table);

        if(rc==0)
            return CMD_OK;
        else if(rc == -1)
            return CMD_FAIL;
        else
            return CMD_ERROR;
    }
}

int fp_reset(char* fmt, char* params, int nparams)
{
    char table[CMD_MAX_STR_PARAMS];

    if(sscanf(params, fmt, &table) == nparams){
        int rc = storage_flight_plan_reset(table);

        if(rc==0)
            return CMD_OK;
        else if(rc == -1)
            return CMD_FAIL;
        else
            return CMD_ERROR;
    }
}

