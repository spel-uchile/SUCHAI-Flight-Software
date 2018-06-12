/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2018, Matias Ramirez Martinez, nicoram.mt@gmail.com
 *      Copyright 2018, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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
#include "taskInit.h"

static const char *tag = "taskInit";

void taskInit(void *param)
{
#ifdef NANOMIND
    LOGI(tag, "Setting pheripherals...");

    /* SPI device drivers */
    osDelay(100);
    adc_channels_init();
    osDelay(100);

    /* Init I2C */
    LOGV(tag, "\tSetting I2C driver...");
    twi_init();

    /* Latest reset source */
    LOGV(tag, "\tLast reset source:");
    int reset_source = reset_cause_get_causes();
    print_rst_cause(reset_source);
    dat_set_system_var(dat_obc_last_reset, reset_source);

    /* RTC + 32kHz OSC */
    LOGV(tag, "\tSetting RTC...");
    init_rtc();

    /* Init spansion chip */
    LOGV(tag, "\tSetting FL512...");
    init_spn_fl512();

    /* Init gyro */
    LOGV(tag, "\tSetting SPI gyros...");
    mpu3300_init(MPU3300_BW_5, MPU3300_FSR_225);

    /* Init magnetometer */
    LOGV(tag, "\tSetting SPI magnetometer...");
    hmc5843_init();
#endif
#ifdef AVR32
    /* Init I2C */
    LOGV(tag, "\tSetting I2C driver...");
    twi_init();
#endif

    LOGD(tag, "Creating client tasks ...");
    // FIXME: This memory values seems not work on nanomind (tested 5)
    unsigned short task_memory = 15*256;
    int n_threads = 4;
    os_thread thread_id[n_threads];

    /* Creating clients tasks */
    osCreateTask(taskConsole, "console", task_memory, NULL, 2, &(thread_id[0]));

#if SCH_HK_ENABLED
    osCreateTask(taskHousekeeping, "housekeeping", task_memory, NULL, 2, &(thread_id[1]));
#endif
#if SCH_COMM_ENABLE
    //FIXME: Init_communications is found in init.c and taskInit.c
    //init_communications();
    osCreateTask(taskCommunications, "comm", task_memory, NULL, 2, &(thread_id[2]));
#endif
#if SCH_FP_ENABLED
    osCreateTask(taskFlightPlan,"flightplan", task_memory,NULL, 2, &(thread_id[3]));
#endif

    osTaskDelete(NULL);
}

void init_communications(void)
{
#if SCH_COMM_ENABLE
    /* Init communications */
    LOGI(tag, "Initialising CSP...");

    if(LOG_LEVEL >= LOG_LVL_DEBUG)
    {
        csp_debug_set_level(CSP_ERROR, 1);
        csp_debug_set_level(CSP_WARN, 1);
        csp_debug_set_level(CSP_INFO, 1);
        csp_debug_set_level(CSP_BUFFER, 1);
        csp_debug_set_level(CSP_PACKET, 1);
        csp_debug_set_level(CSP_PROTOCOL, 1);
        csp_debug_set_level(CSP_LOCK, 0);
    }
    else
    {
        csp_debug_set_level(CSP_ERROR, 1);
        csp_debug_set_level(CSP_WARN, 1);
        csp_debug_set_level(CSP_INFO, 1);
        csp_debug_set_level(CSP_BUFFER, 0);
        csp_debug_set_level(CSP_PACKET, 0);
        csp_debug_set_level(CSP_PROTOCOL, 0);
        csp_debug_set_level(CSP_LOCK, 0);
    }

    int t_ok;
    /* Init buffer system with 5 packets of maximum SCH_BUFF_MAX_LEN bytes each */
    t_ok = csp_buffer_init(SCH_BUFFERS_CSP, SCH_BUFF_MAX_LEN);
    if(t_ok != 0) LOGE(tag, "csp_buffer_init failed!");
    /* Init CSP with address MY_ADDRESS */
    csp_init(SCH_COMM_ADDRESS);
    /* Start router task with SCH_TASK_CSP_STACK word stack, OS task priority 1 */
    t_ok = csp_route_start_task(SCH_TASK_CSP_STACK, 1);
    if(t_ok != 0) LOGE(tag, "Task router not created!");

#ifdef LINUX
    /* Set ZMQ interface */
    csp_zmqhub_init_w_endpoints(255, SCH_COMM_ZMQ_OUT, SCH_COMM_ZMQ_IN);
    csp_route_set(CSP_DEFAULT_ROUTE, &csp_if_zmqhub, CSP_NODE_MAC);
#endif

    LOGD(tag, "Route table");
    csp_route_print_table();
    LOGD(tag, "Interfaces");
    csp_route_print_interfaces();
#endif
}