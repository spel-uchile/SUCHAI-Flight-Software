/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2018, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2018, Tomas Opazo Toro, tomas.opazo.t@gmail.com
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

#include "taskHousekeeping.h"

static const char *tag = "Housekeeping";

enum phase_n{
    phase_a0, // 0: (A0) base,
    phase_a,  // 1: (A) ascend,
    phase_b,  // 2: (B) equilibrium,
    phase_b1, // 3: (B1) deploy1,
    phase_b2, // 4: (B2) deploy2,
    phase_c,  // 5: (C) descend,
    phase_c1, // 6: (C1) landing
};

const int  MIN_PHASE_A  =   1;
const int  MIN_PHASE_B  = 110;
const int  MIN_PHASE_B1 = 150;
const int  MIN_PHASE_B2 = 155;
const int  MIN_PHASE_C  = 160;
const int  MIN_PHASE_C1 = 270;

void taskHousekeeping(void *param)
{
    LOGD(tag, "Started");

    portTick delay_ms    = 1000;            //Task period in [ms]

    unsigned int elapsed_sec = 0;           // Seconds counter
    unsigned int _10sec_check = 10;         //10[s] condition
    unsigned int _01min_check = 1*60;       //1[m] condition
    unsigned int _05min_check = 5*60;       //5[m] condition
    unsigned int _10min_check = 10*60;      //10[m] condition
    unsigned int _1hour_check = 60*60;      //01[h] condition

    int phase = dat_get_system_var(dat_balloon_phase);      // current phase
    int balloons = dat_get_system_var(dat_balloon_deploys); // balloons available

    portTick xLastWakeTime = osTaskGetTickCount();
    
    while(1)
    {

        osTaskDelayUntil(&xLastWakeTime, delay_ms); // Suspend task
        elapsed_sec++; //= delay_ms/1000; // Update seconds counts

        change_system_phase();
        phase = dat_get_system_var(dat_balloon_phase); // Determine current phase

        /**
         * Phases A(0), A1(1), B(2) and C(3): sample radiosonde data at 1Hz and
         * send beacon at 1/4 Hz
         */
        if (phase < phase_c) {
            //1 second action: sample radiosonde data
            if ((elapsed_sec % 5) == 0) {
//                cmd_t *cmd_sonde_1 = cmd_get_str("get_weather");
//                cmd_send(cmd_sonde_1);
            }

            // 4 seconds actions: download radiosonde data
            if ((elapsed_sec % 10) == 0) {
//                cmd_t *cmd_send_sonde_trx = cmd_get_str("send_weather_data");
//                cmd_send(cmd_send_sonde_trx);
//                cmd_t *cmd_send_sonde_trx_2 = cmd_get_str("send_weather_data");
//                cmd_send(cmd_send_sonde_trx_2);
            }
        }

        /**
         * Phase A1 y B: execute star tracker + sample and send radiosonde data
         * (see code above)
         */
        if (phase == phase_a || phase == phase_b) {
            if ((elapsed_sec % _05min_check) == 0) {
//                cmd_t *cmd_startracker = cmd_get_str("cmd_system");
//                cmd_add_params_str(cmd_startracker, "startracker");
//                cmd_send(cmd_startracker);
            }
        }


        if (phase == phase_b1) {
            /* Send command to cut the one balloon */
            balloons = dat_get_system_var(dat_balloon_deploys);
            if (balloons > 1) {
                balloons--;
                dat_set_system_var(dat_balloon_deploys, balloons);
//                cmd_t *cmd_cut_balloon = cmd_get_str("cut_balloon");
                // or 13
//                cmd_add_params_var(cmd_cut_balloon, 12); /* Cut first balloon */
//                cmd_send(cmd_cut_balloon);
            }
        }

        /**
         * Phase C: cut balloon, sample radiosonde data at 1Hz, send radiosonde
         * data via Iridium at 1/10Hz, execute super-resolution experiment
         * every minute.
         */

        if (phase == phase_c) {
            /* Sample radiosonde data at 1HZ */
            if ((elapsed_sec % 5) == 0) {
//                cmd_t *cmd_sonde_2 = cmd_get_str("get_weather");
//                cmd_send(cmd_sonde_2);
            }

            /* Send radiosonde data via Iridium every 10 seconds */
            if ((elapsed_sec % _10sec_check) == 0) {
//                cmd_t *cmd_iridium = cmd_get_str("send_iridium");
//                cmd_add_params_var(cmd_iridium, 1);
//                cmd_send(cmd_iridium);
            }

            /* Execute super-resolution experiment every minute */
            if ((elapsed_sec % _01min_check) == 0) {
//                cmd_t *cmd_startracker = cmd_get_str("cmd_system");
//                cmd_add_params_str(cmd_startracker, "superres");
//                cmd_send(cmd_startracker);
            }
        }

        /**
         * Phase C1: cut the second balloon, keep sending radiosonde data via
         * Iridium
         */
        if(phase == phase_c1)
        {
            /* Send command to cut the second balloon */
            balloons = dat_get_system_var(dat_balloon_deploys);   // balloons available
            if(balloons > 0)
            {
                balloons --;
                dat_set_system_var(dat_balloon_deploys, balloons);
//                cmd_t *cmd_cut_balloon = cmd_get_str("cut_balloon");
//                cmd_add_params_var(cmd_cut_balloon, 13); /* Cut second balloon */
//                cmd_send(cmd_cut_balloon);
            }

            /* Do not sample radiosonde data, but send existing radiosonde data
             * via Iridium every 10 seconds */
//            if((elapsed_sec % _10sec_check) == 0)
//            {
//                cmd_t *cmd_iridium = cmd_get_str("send_iridium");
//                cmd_send(cmd_iridium);
//            }
        }

        /**
         * Always to do list
         */
        /* 1 min actions, update minutes alive counter*/
//        if((elapsed_sec % 1) == 0)
        if((elapsed_sec % _01min_check) == 0)
        {
            LOGD(tag, "1 hour check");
            cmd_t *cmd_1h = cmd_get_str("update_hours_alive");
            cmd_add_params_var(cmd_1h, 1); // Add 1 min
            cmd_send(cmd_1h);
            LOGI(tag, "Phase: %d", phase);
        }
    }
}

void change_system_phase()
{
    int min_alive;
    int current_phase = dat_get_system_var(dat_balloon_phase);
    min_alive = dat_get_system_var(dat_obc_hrs_alive);

    if(min_alive <= MIN_PHASE_A)
    {
        LOGD(tag, "We are in PHASE A0");
        // We are in phase A0
        if(current_phase != phase_a0) {
            dat_set_system_var(dat_balloon_phase, phase_a0);

        }
    }
    else if (min_alive > MIN_PHASE_A && min_alive <= MIN_PHASE_B)
    {
        LOGD(tag, "We are in PHASE A");
        // We are in phase A1
        if(current_phase != phase_a){
            dat_set_system_var(dat_balloon_phase, phase_a);

        }
    }
    else if (min_alive > MIN_PHASE_B && min_alive <= MIN_PHASE_B1)
    {
        LOGD(tag, "We are in PHASE B");
        // We are in phase B
        if(current_phase != phase_b){

            dat_set_system_var(dat_balloon_phase, phase_b);
        }

    }
    else if (min_alive > MIN_PHASE_B1 && min_alive <= MIN_PHASE_B2)
    {
        LOGD(tag, "We are in PHASE B1");
        // We are in phase B1
        if(current_phase != phase_b1){

            dat_set_system_var(dat_balloon_phase, phase_b1);
        }

    }
    else if (min_alive > MIN_PHASE_B2 && min_alive <= MIN_PHASE_C)
    {
        LOGD(tag, "We are in PHASE B2");
        // We are in phase B2
        if(current_phase != phase_b2){

            dat_set_system_var(dat_balloon_phase, phase_b2);
        }

    }
    else if (min_alive > MIN_PHASE_C && min_alive <= MIN_PHASE_C1)
    {
        LOGD(tag, "We are in PHASE C");
        // We are in phase C
        if(current_phase != phase_c) {

            dat_set_system_var(dat_balloon_phase, phase_c);
        }

    }
    else if (min_alive > MIN_PHASE_C1)
    {
        LOGD(tag, "We are in PHASE C1");
        // We are in phase C1
        if(current_phase != phase_c1){

            dat_set_system_var(dat_balloon_phase, phase_c1);
        }
    }
}
