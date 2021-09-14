/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Matias Ramirez Martinez, nicoram.mt@gmail.com
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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
#include "suchai/taskInit.h"

static const char *tag = "taskInit";

int init_update_status_vars(void);
int init_create_task(void);

#if SCH_COMM_ENABLE

#endif //SCH_COMM_ENABLE

void taskInit(void *param)
{
    int rc = 0;
#ifdef NANOMIND
    on_init_task(NULL);
#endif

    LOGI(tag, "SETUP REPOSITORIES...");
    /* Initialize repositories */
    cmd_repo_init(); // Command repository initialization
    dat_repo_init(); // Update status repository

    /* Initialize system variables */
    LOGI(tag, "SETUP VARIABLES...");
    rc = init_update_status_vars();
    LOGI(tag, "PURGE FLIGHT PLAN...");
    rc = dat_purge_fp();
    // Init LibCSP system
#if SCH_COMM_ENABLE
    LOGI(tag, "SETUP CSP...");
    init_setup_libcsp(SCH_COMM_NODE);
#endif
    // Create tasks
    LOGI(tag, "CREATE TASKS...");
    rc = init_create_task();

    /**
     * Call app initialization routine
     */
#ifdef SCH_HOOK_INIT
    initAppHook(param);
#endif

    LOGI(tag, "EXIT INIT TASK", 0);
    osTaskDelete(NULL);
}

int init_update_status_vars(void) {
    LOGD(tag, "Initializing system variables values...")
    int rc = 0;
    rc += dat_set_system_var(dat_obc_hrs_wo_reset, 0);
    rc += dat_set_system_var(dat_obc_reset_counter, dat_get_system_var(dat_obc_reset_counter) + 1);
    rc += dat_set_system_var(dat_obc_sw_wdt, 0);  // Reset the gnd wdt on boot
    return rc;
}

int init_setup_libcsp(int node)
{
    /* Init communications */
    LOGI(tag, "Initialising CSP...");

    csp_debug_set_level(CSP_ERROR, 1);
    csp_debug_set_level(CSP_WARN, 1);
    csp_debug_set_level(CSP_INFO, 1);
    csp_debug_set_level(CSP_BUFFER, 0);
    csp_debug_set_level(CSP_PACKET, 1);
    csp_debug_set_level(CSP_PROTOCOL, 1);
    csp_debug_set_level(CSP_LOCK, 0);

    /* Init buffer system */
    int t_ok;
    t_ok = csp_buffer_init(SCH_CSP_BUFFERS, SCH_BUFF_MAX_LEN);
    if(t_ok != 0) LOGE(tag, "csp_buffer_init failed!");

    /* Init CSP */
    csp_set_hostname(SCH_NAME);
    csp_init(node); // Init CSP with address MY_ADDRESS

    /* Start router task with SCH_TASK_CSP_STACK word stack, OS task priority 1 */
    t_ok = csp_route_start_task(SCH_TASK_CSP_STACK, 1);
    if(t_ok != 0) LOGE(tag, "Task router not created!");

    /**
     * Add CSP INTERFACES in initAppHook, for example call to
     * csp_add_zmq_iface(SCH_COMM_NODE)
     */

    LOGI(tag, "CSP Node: %d", csp_get_address());
    return 0;
}

int init_create_task(void) {
    LOGD(tag, "Creating client tasks ...");
    int t_ok;
    int n_threads = 6;
    os_thread thread_id[n_threads];

    /* Creating clients tasks */
#if SCH_CON_ENABLED
    t_ok = osCreateTask(taskConsole, "console", SCH_TASK_CON_STACK, NULL, 2, &(thread_id[0]));
    if(t_ok != 0) LOGE(tag, "Task console not created!");
#endif
#if SCH_COMM_ENABLE
    t_ok = osCreateTask(taskCommunications, "comm", SCH_TASK_COM_STACK, NULL, 2, &(thread_id[2]));
    if(t_ok != 0) LOGE(tag, "Task communications not created!");
#endif
#if SCH_FP_ENABLED
    t_ok = osCreateTask(taskFlightPlan,"flightplan", SCH_TASK_FPL_STACK, NULL, 2, &(thread_id[3]));
        if(t_ok != 0) LOGE(tag, "Task flightplan not created!");
#endif

    return t_ok;
}

/**
 * Set ZMQ interfaces as default route
 */
void csp_add_zmq_iface(int node)
{
    /* Set ZMQ interface as a default route*/
    uint8_t addr = (uint8_t)node;
    uint8_t *rxfilter = &addr;
    unsigned int rxfilter_count = 1;

    csp_zmqhub_init_w_name_endpoints_rxfilter(CSP_ZMQHUB_IF_NAME,
                                              rxfilter, rxfilter_count,
                                              SCH_COMM_ZMQ_OUT, SCH_COMM_ZMQ_IN,
                                              &csp_if_zmqhub);
    csp_route_set(CSP_DEFAULT_ROUTE, csp_if_zmqhub, CSP_NODE_MAC);
}