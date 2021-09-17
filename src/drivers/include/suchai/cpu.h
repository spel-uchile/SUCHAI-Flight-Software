/**
 * @file  i2c.h
 * @author Carlos Gonzalez C. - carlgonz@uchile.cl
 * @date 2021
 * @copyright GNU GPL v3
 *
 * This header have definitions of functions related to specific CPU
 */

#ifndef SCH_CPU_H
#define SCH_CPU_H

#include <stdint.h>

/**
 * Get system/rtc time in unix timestamp format
 * @return Unix timestamp
 */
int64_t cpu_get_unixtime(void);

/**
 * Set system/rtc time in unix timestamp
 * @param time Unix timestamp
 * @return 1 if Ok, 0 if errors.
 */
int cpu_set_unixtime(int64_t time);

/**
 * Debug CPU functioning, usually by blinking a LED.
 * Arg. can be used to select different kind o debug, or select
 * different LEDs.
 * @param arg Debug type
 * @return 1 if Ok, 0 if errors.
 */
int cpu_debug(int arg);

/**
 * Reset CPU (internal/external) watchdog timer.
 * Use arg to determine specific operations
 * @param arg WDT type:
 *  0: Internal or CPU WTD
 *  1: External WDT
 *  2: Other
 * @return 1 if Ok, 0 if errors.
 */
int cpu_reset_wdt(int arg);

/**
 * Reset (reboot) CPU.
 * Use arg to determine specific operations
 * @param arg Reset type:
 *  0: Soft reset
 *  1: Hard reset (power down/up)
 */
void cpu_reboot(int arg);

#endif //SCH_CPU_H