/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2021, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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
#include "app/system/taskHousekeeping.h"
#include "app/system/cmdAPP.h"

#include <csp/csp.h>
#include <csp/arch/csp_thread.h>
#include <csp/drivers/usart.h>
#include <csp/drivers/can_socketcan.h>
#include <csp/interfaces/csp_if_zmqhub.h>

#define SCH_TNC_ADDRESS 9
static char *tag = "app_main";

static csp_iface_t *csp_if_zmqhub;
static csp_iface_t csp_if_kiss;

static csp_kiss_handle_t csp_kiss_driver;
void my_usart_rx(uint8_t * buf, int len, void * pxTaskWoken) {
    csp_kiss_rx(&csp_if_kiss, buf, len, pxTaskWoken);
}

/**
 * App specific initialization routines
 * This function is called by taskInit
 *
 * @param params taskInit params
 */
void initAppHook(void *params)
{
    /** Include app commands */
    cmd_app_init();

    /** Initialize custom CSP interfaces */
#ifdef LINUX
    csp_add_zmq_iface(SCH_COMM_NODE);
#endif


    /** Init app tasks */
    int t_ok = osCreateTask(taskHousekeeping, "housekeeping", 1024, NULL, 2, NULL);
    if(t_ok != 0) LOGE("simple-app", "Task housekeeping not created!");
}

int main(void)
{
    /** Call framework main, shouldn't return */
    suchai_main();
}
