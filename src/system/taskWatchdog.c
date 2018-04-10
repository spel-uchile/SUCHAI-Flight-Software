/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2018, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
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

#include "taskWatchdog.h"

static const char *tag = "WDT";

void taskWatchdog(void *param)
{
    LOGD(tag, "Started");

    portTick delay_ms    = 1000;        //Task period in [ms]
    unsigned int max_obc_wdt = SCH_MAX_WDT_TIMER;     // Seconds to send "reset_wdt" command
    unsigned int max_gnd_wdt = SCH_MAX_GND_WDT_TIMER; // Seconds to send "reset" command
    unsigned int elapsed_obc_timer = 0; // OBC timer counter
    unsigned int elapsed_gnd_timer = 0; // GND timer counter
    portTick xLastWakeTime = osTaskGetTickCount();
    
    while(1)
    {
        // Sleep task to count seconds
        osTaskDelayUntil(&xLastWakeTime, delay_ms);
        elapsed_obc_timer++; // Increase timer to reset the obc wdt
        elapsed_gnd_timer = (unsigned  int)dat_get_system_var(dat_gnd_wdt) + 1; //Increase GND timer counter. Should be cleared by a gnd command
        dat_set_system_var(dat_gnd_wdt, elapsed_gnd_timer); // Save increased GND timer

        LOGV(tag, "GND_WDT (%d)", elapsed_gnd_timer);

        // Periodically reset the OBC watchdog
        if(elapsed_obc_timer > max_obc_wdt)
        {
            elapsed_obc_timer = 0;
            cmd_t *rst_wdt = cmd_get_str("reset_wdt");
            cmd_send(rst_wdt);
        }

        // If nobody clears elapsed_gnd_timer, then reset the OBC
        if(elapsed_gnd_timer > max_gnd_wdt)
        {
            LOGW(tag, "GND watchdog overflow")
            cmd_t *rst_obc = cmd_get_str("reset");
            cmd_send(rst_obc);
        }
    }
}
