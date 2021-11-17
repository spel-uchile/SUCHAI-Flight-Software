//
// Created by franco on 17-11-21.
//
#include "suchai/mainFS.h"
#include "suchai/taskInit.h"
#include "suchai/osThread.h"
#include "suchai/log_utils.h"
#include "app/system/taskHousekeeping.h"
#include "app/system/cmdAPP.h"

static char *tag = "app_nanopb_simple_app";

void initAppHook(void *params)
{
    /** Include app commands*/
    cmd_app_init();
    /** Initialize custom CSP interfaces */
#ifdef LINUX
    csp_add_zmq_iface(SCH_COMM_NODE);
#endif

    /** Init app tasks*/
    int t_ok = osCreateTask(task, "nanopb-simple-app-task", 1024, NULL, 2,NULL);
    if (t_ok != 0) LOGE("nanopb-simple-app", "main task not created!");
}

int main(void){

    /** Call framework main, shouldn't return */
    suchai_main();
}