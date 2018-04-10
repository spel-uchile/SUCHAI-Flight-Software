/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2017, Matias Ramirez Martinez, nicoram.mt@gmail.com
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
    LOGV(tag, "\tSetting SPI devices...");
    init_spi();
    lm70_init();
    fm33256b_init();
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
    dat_set_system_var(dat_obc_lastResetSource, reset_source);

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

    LOGD(tag, "Creating client tasks ...");
    unsigned short task_memory = 15*256;
    os_thread thread_id[4];

    /* Creating clients tasks */
    osCreateTask(taskConsole, "console", task_memory, NULL, 2, &(thread_id[0]));

#if SCH_HK_ENABLED
    osCreateTask(taskHousekeeping, "housekeeping", task_memory, NULL, 2, &(thread_id[1]));
#endif
#if SCH_COMM_ENABLE
    osCreateTask(taskCommunications, "comm", task_memory, NULL, 2, &(thread_id[2]));
#endif
#if SCH_FP_ENABLED
    osCreateTask(taskFlightPlan,"flightplan", task_memory,NULL, 2, &(thread_id[3]));
#endif

    osTaskDelete(NULL);
}