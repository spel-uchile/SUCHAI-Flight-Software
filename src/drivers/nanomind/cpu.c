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
#include "drivers.h"
#include "time.h"

int64_t get_unixtime(void)
{
//    gs_timestamp_t timestamp;
//    gs_clock_get_time(&timestamp);
//    return (int64_t)timestamp.tv_sec;
    return time(NULL);
}

int set_unixtime(int64_t time)
{
    gs_timestamp_t timestamp = {(uint32_t)time, 0};
    gs_clock_set_time(&timestamp);
    return 0;
}