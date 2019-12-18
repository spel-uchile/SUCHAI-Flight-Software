/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2019, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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

#include "taskSensors.h"

static const char *tag = "Sensors";

void taskSensors(void *param)
{
    LOGI(tag, "Started");
    portTick xLastWakeTime = osTaskGetTickCount();

    int i;
    cmd_t *cmd_init;
    cmd_t *cmd_get;
    int nsensors = 1;
    char *init_cmds[] = {"init_dummy_sensor"};
    char *get_cmds[] = {"obc_get_sensors"};

    machine = (sample_machine_t) {ST_SAMPLING, ACT_STAND_BY, last_sensor, 5, -1};

    if(osSemaphoreCreate(&repo_machine_sem) != CSP_SEMAPHORE_OK)
    {
        LOGE(tag, "Unable to create system status repository mutex");
    }

    for(i=0; i<nsensors; i++)
    {
        cmd_init = cmd_get_str(init_cmds[i]);
        cmd_send(cmd_init);
    }

    int elapsed_sec = 0;

    while(1)
    {
        osTaskDelayUntil(&xLastWakeTime, 1000); //Suspend task
        LOGD(tag, "state: %d, action %d, samples left: %d", machine.state, machine.action, machine.samples_left)
        osSemaphoreTake(&repo_machine_sem, portMAX_DELAY);
        // Apply action
        if (machine.action != ACT_STAND_BY) {
            if (machine.action == ACT_START) {
                machine.state = ST_SAMPLING;
                machine.action = ACT_STAND_BY;
            } else if (machine.action == ACT_PAUSE) {
                machine.state = ST_PAUSE;
                machine.action = ACT_STAND_BY;
            }
        }

        // States
        if (machine.state == ST_SAMPLING) {
            // Check for samples left
            if (machine.samples_left == 0) {
                machine.state = ST_PAUSE;
                machine.action = ACT_STAND_BY;
            }
            // Check for step
            else if (elapsed_sec % machine.step == 0) {
                LOGD(tag, "SAMPLING...");
                for(i=0; i<nsensors; i++) {
                    cmd_get = cmd_get_str(get_cmds[i]);
                    cmd_send(cmd_get);
                }
                if (machine.samples_left != -1) {
                    machine.samples_left -= 1;
                }
            }
        }
        osSemaphoreGiven(&repo_machine_sem);
        elapsed_sec += 1;
        // TODO: Store in system variables machine state
    }
}
