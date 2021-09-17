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

/**
 * TOGGLE GPIO
 * ARG  GPIO (PIN)
 *  0 -> 16  (36)
 *  1 -> 17  (11)
 *  2 -> 22  (15)
 *  3 -> 23  (16)
 *  4 -> 24  (18)
 *  5 -> 25  (22)
 *  6 -> 26  (37)
 *  7 -> 27  (13)
 */
int cpu_debug(int arg)
{
    switch (arg) {
        case 0:
        case 16:
            system("gpio -g mode 16 out");
            system("gpio -g toggle 16");
            break;
        case 1:
        case 17:
            system("gpio -g mode 17 out");
            system("gpio -g toggle 17");
            break;
        case 2:
        case 22:
            system("gpio -g mode 22 out");
            system("gpio -g toggle 22");
            break;
        case 3:
        case 23:
            system("gpio -g mode 23 out");
            system("gpio -g toggle 23");
            break;
        case 4:
        case 24:
            system("gpio -g mode 24 out");
            system("gpio -g toggle 24");
            break;
        case 5:
        case 25:
            system("gpio -g mode 25 out");
            system("gpio -g toggle 25");
            break;
        case 6:
        case 26:
            system("gpio -g mode 26 out");
            system("gpio -g toggle 26");
            break;
        case 7:
        case 27:
            system("gpio -g mode 27 out");
            system("gpio -g toggle 27");
            break;
        default:
            printf("CPU DEBUG %d\r\n", arg);
            break;
    }

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