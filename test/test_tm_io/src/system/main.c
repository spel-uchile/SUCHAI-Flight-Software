/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2020, Camilo Rojas Milla, camrojas@uchile.cl
 *      Copyright 2020, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *      Copyright 2020, Matias Ramirez Martinez, nicoram.mt@gmail.com
 *      Copyright 2020, Tamara Gutierrez Rojo tamigr.2293@gmail.com
 *      Copyright 2020, Ignacio Ibanez Aliaga, ignacio.ibanez@usach.cl
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

#include "suchai/mainFS.h"
#include "suchai/taskInit.h"
#include "suchai/osThread.h"
#include "suchai/log_utils.h"
#include "app/system/taskTest.h"

static char *tag = "app_main";

/**
 * App specific initialization routines
 * This function is called by taskInit
 *
 * @param params taskInit params
 */
void initAppHook(void *params)
{
    /** Include app commands
     * No extra commands for this app*/

    /** Initialize custom CSP interfaces */
#ifdef LINUX
    csp_add_zmq_iface(SCH_COMM_NODE);
#endif

    /** Init app task */
    int t_ok = osCreateTask(taskTest, "test_tm_io", 1024, NULL, 2, NULL);
    // It had 3 prio
    if(t_ok != 0) LOGE("cmd-tm_io", "Task test tm io not created!");
}

int main(void)
{
    /** Call framework main, shouldn't return */
    suchai_main();
}

//    int t_inv_ok = osCreateTask(taskDispatcher,"invoker", SCH_TASK_DIS_STACK, NULL, 3, &threads_id[1]);
//    int t_exe_ok = osCreateTask(taskExecuter, "receiver", SCH_TASK_EXE_STACK, NULL, 4, &threads_id[2]);
//    int t_wdt_ok = osCreateTask(taskWatchdog, "watchdog", SCH_TASK_WDT_STACK, NULL, 2, &threads_id[0]);
//    int t_ini_ok = osCreateTask(taskInit, "init", SCH_TASK_INI_STACK, NULL, 3, &threads_id[3]);
