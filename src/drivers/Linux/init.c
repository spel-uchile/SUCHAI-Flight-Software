/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2018, Matias Ramirez Martinez, nicoram.mt@gmail.com
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

#include "init.h"

static const char *tag = "Initializer";

void on_close(int signal)
{
    dat_repo_close();

    LOGI(tag, "Exit system!");
    exit(signal);
}

void on_reset(void)
{
    /* Register INT/TERM signals */
    struct sigaction act;
    act.sa_handler = on_close;
    sigaction(SIGINT, &act, NULL);  // Register CTR+C signal handler
    sigaction(SIGTERM, &act, NULL);

#if SCH_COMM_ENABLE
    /* Init communications */
    LOGI(tag, "Initialising CSP...");
    /* Init buffer system with 5 packets of maximum 300 bytes each */
    csp_buffer_init(5, 300);
    /* Init CSP with address MY_ADDRESS */
    csp_init(SCH_COMM_ADDRESS);
    /* Start router task with 500 word stack, OS task priority 1 */
    csp_route_start_task(500, 1);

    /* Set ZMQ interface */
    #ifdef LINUX
        csp_zmqhub_init_w_endpoints(255, SCH_COMM_ZMQ_OUT, SCH_COMM_ZMQ_IN);
        csp_route_set(CSP_DEFAULT_ROUTE, &csp_if_zmqhub, CSP_NODE_MAC);
    #endif

    LOGD(tag, "Route table");
    csp_route_print_table();
    LOGD(tag, "Interfaces");
    csp_route_print_interfaces();
#endif
}

void init_communications(void)
{
    /* Init communications */
    LOGI(tag, "Initialising CSP...");
    /* Init buffer system with 5 packets of maximum 300 bytes each */
    csp_buffer_init(5, 300);
    /* Init CSP with address MY_ADDRESS */
    csp_init(SCH_COMM_ADDRESS);
    /* Start router task with 500 word stack, OS task priority 1 */
    csp_route_start_task(500, 1);

    LOGD(tag, "Route table");
    csp_route_print_table();
    LOGD(tag, "Interfaces");
    csp_route_print_interfaces();
}