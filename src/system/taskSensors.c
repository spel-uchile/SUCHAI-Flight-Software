/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
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

#include "taskSensors.h"

static const char *tag = "Sensors";

void taskSensors(void *param)
{
    LOGI(tag, "Started");
    portTick xLastWakeTime = osTaskGetTickCount();

    int i;
    cmd_t *cmd_init;
    cmd_t *cmd_get;
    int nsensors = 4;
    char *init_cmds[] = {"init_dummy_sensor", "init_dummy_sensor", "init_dummy_sensor", "init_dummy_sensor"};
    char *get_cmds[] = {"sen_take_sample", "sen_take_sample", "eps_get_hk", "sen_take_sample" };

    status_machine = (dat_stmachine_t) {ST_PAUSE, ACT_START, 0, 5, -1, nsensors};

    if(osSemaphoreCreate(&repo_machine_sem) != CSP_SEMAPHORE_OK)
    {
        LOGE(tag, "Unable to create system status repository mutex");
    }

    for(i=0; i < status_machine.total_sensors; i++)
    {
        cmd_init = cmd_get_str(init_cmds[i]);
        cmd_add_params_str(cmd_init, "2 1");
        cmd_send(cmd_init);
    }

    int elapsed_sec = 0;

    while(1)
    {
        osTaskDelayUntil(&xLastWakeTime, 1000); //Suspend task
        LOGD(tag, "state: %d, action %d, samples left: %d", status_machine.state, status_machine.action, status_machine.samples_left)
        osSemaphoreTake(&repo_machine_sem, portMAX_DELAY);
        // Apply action
        if (status_machine.action != ACT_STAND_BY) {
            if (status_machine.action == ACT_START) {
                status_machine.state = ST_SAMPLING;
                status_machine.action = ACT_STAND_BY;
            } else if (status_machine.action == ACT_PAUSE) {
                status_machine.state = ST_PAUSE;
                status_machine.action = ACT_STAND_BY;
            }
        }

        // States
        if (status_machine.state == ST_SAMPLING) {
            // Check for samples left
            if (status_machine.samples_left == 0) {
                status_machine.state = ST_PAUSE;
                status_machine.action = ACT_STAND_BY;
            }
            // Check for step
            else if (elapsed_sec % status_machine.step == 0) {
                LOGD(tag, "SAMPLING...");

                for(i=0; i<nsensors; i++) {
//                    printf("payload %d active status %d\n", i, dat_stmachine_is_sensor_active(i, status_machine.active_payloads, nsensors));
                    if (dat_stmachine_is_sensor_active(i,
                                                       status_machine.active_payloads,
                                                       status_machine.total_sensors)) {
                        cmd_get = cmd_get_str(get_cmds[i]);
                        char cmd_args[5];
                        sprintf(cmd_args, " %d", i);
                        cmd_add_params_str(cmd_get, cmd_args);
                        cmd_send(cmd_get);
                    }
                }
                if (status_machine.samples_left != -1) {
                    status_machine.samples_left -= 1;
                }
            }
        }

        int action = (int) status_machine.action;
        int state = (int) status_machine.state;
        int step = (int) status_machine.step;
        int active_payloads = (int) status_machine.active_payloads;
        int samples_left = (int) status_machine.samples_left;
        osSemaphoreGiven(&repo_machine_sem);

        dat_set_system_var(dat_drp_mach_action, action);
        dat_set_system_var(dat_drp_mach_state, state);
        dat_set_system_var(dat_drp_mach_step, step);
        dat_set_system_var(dat_drp_mach_payloads, active_payloads);
        dat_set_system_var(dat_drp_mach_left, samples_left);
        elapsed_sec += 1;
    }
}


