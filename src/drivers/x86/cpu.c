/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2021, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
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

#include "suchai/cpu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int64_t cpu_get_unixtime(void)
{
    return (int64_t)time(NULL);
}

int cpu_set_unixtime(int64_t time)
{
    char cmd[64];
    memset(cmd, 0, 64);
    snprintf(cmd, 64, "%s%ld", "sudo date +%s -s @", time);
    int rc = system(cmd);
    return rc != 0 ? 1 : 0;
}

int cpu_debug(int arg)
{
    printf("CPU DEBUG %d\r\n", arg);
    return 1;
}

int cpu_reset_wdt(int arg)
{
    return 0;
}

void cpu_reboot(int arg)
{
    switch (arg) {
        case 0:
            exit(0);
            break;
        case 1:
            system("sudo reboot");
            break;
        default:
            exit(0);
            break;

    }
}