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
#include "include/taskTest.h"

static const char *tag = "Test";

void taskTest(void *param)
{
    portTick s_delay = 3000;
    portTick xLastWakeTime = osTaskGetTickCount();
    osTaskDelayUntil(&xLastWakeTime, s_delay);
    char* cmd = "test_mult_exe";
    cmd_t *test1 = cmd_get_str(cmd);
    portTick current_time = osTaskGetTickCount();
    cmd_add_params_var(test1, 10, current_time);
    printf("Intial Time: %ul\n" , current_time );
    cmd_send(test1);
    cmd_t *test2 = cmd_get_str(cmd);
    current_time = osTaskGetTickCount();
    cmd_add_params_var(test2, 5, current_time);
    cmd_send(test2);
    cmd_t *test3 = cmd_get_str(cmd);
    current_time = osTaskGetTickCount();
    cmd_add_params_var(test3, 20, current_time);
    cmd_send(test3);
    cmd_t *test4 = cmd_get_str(cmd);
    current_time = osTaskGetTickCount();
    cmd_add_params_var(test4, 30, current_time);
    cmd_send(test4);
    cmd_t *test4 = cmd_get_str(cmd);
    current_time = osTaskGetTickCount();
    cmd_add_params_var(test4, 15, current_time);
    cmd_send(test5);
}

int test_sleep(int s_sleep, int actual_time){

    portTick s_delay = (portTick)s_sleep*1000;
    portTick xLastWakeTime = osTaskGetTickCount();
    osTaskDelayUntil(&xLastWakeTime, s_delay);
    return 0;
}
