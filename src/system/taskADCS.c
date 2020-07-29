/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2020, Tomas Opazo Toro, tomas.opazo.t@gmail.com
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

#include "taskADCS.h"

static const char *tag = "ADCS";

void taskADCS(void *param)
{
    LOGI(tag, "Started");

    portTick delay_ms  = 10;            //Task period in [ms]

    unsigned int elapsed_sec = 1;           // Seconds counter
    unsigned int elapsed_msec = 1;
    unsigned int _adcs_ctrl_period = 1000;     // ADCS control period in seconds
    unsigned int _10sec_check = 10*1000;         // 10[s] condition
    unsigned int _01min_check = 1*60*1000;       // 05[m] condition
    unsigned int _05min_check = 5*60*1000;       // 05[m] condition
    unsigned int _1hour_check = 60*60*1000;      // 01[h] condition

    portTick xLastWakeTime = osTaskGetTickCount();

    /**
     * Set-up SGP4 propagator
     */
    cmd_t *tle1 = cmd_get_str("obc_set_tle");
    cmd_add_params_str(tle1, "1 42788U 17036Z   20027.14771603  .00000881  00000-0  39896-4 0  9992");
    cmd_send(tle1);
    cmd_t *tle2 = cmd_get_str("obc_set_tle");
    cmd_add_params_str(tle2, "2 42788  97.3234  85.2817 0012095 159.3521 200.8207 15.23399088144212");
    cmd_send(tle2);
    cmd_t *tle_u = cmd_get_str("obc_update_tle");
    cmd_send(tle_u);
    tle_u = cmd_get_str("obc_get_tle");
    cmd_send(tle_u);
    dat_set_system_var(dat_obc_opmode, DAT_OBC_OPMODE_DETUMB_MAG);

    while(1)
    {
        osTaskDelayUntil(&xLastWakeTime, delay_ms); //Suspend task
        elapsed_msec += delay_ms;

        /**
         * Estimate Loop
         */
        if (elapsed_msec % 10 == 0) {

            double P[6][6];
            double dt = (double) elapsed_msec / 1000.0;
            eskf_predict_state((double*)P, dt);

            if(elapsed_msec % 100 == 0)
            {
                // Update magnetic
                matrix3_t R;
                // TODO: get value from magsensor
                vector3_t mag_sensor = {0.33757741, 0.51358994, 0.78883893};
                // TODO: get value from magnetic model
                vector3_t mag_i = {6723.12366721, 10229.07189747, 15710.68799647};

                quaternion_t q_est;
                _get_sat_quaterion(&q_est, dat_ads_q0);

                vector3_t w;
                _get_sat_vector(&w, dat_ads_omega_x);

                // TODO: call function separately with its own mesuerement freq
                eskf_update_mag(mag_sensor, mag_i, P, &R, &q_est, &w);
            }
        }

        /* 1 second actions */
        dat_set_system_var(dat_rtc_date_time, (int) time(NULL));
        /**
         * Control LOOP
         */
        if ((elapsed_msec % _adcs_ctrl_period) == 0)
        {
            cmd_t *cmd_tle_prop = cmd_get_str("obc_prop_tle");
            cmd_add_params_str(cmd_tle_prop, "0");
            cmd_send(cmd_tle_prop);
            // Update attitude
            cmd_t *cmd_stt = cmd_get_str("adcs_quat");
            cmd_send(cmd_stt);
            cmd_t *cmd_acc = cmd_get_str("adcs_acc");
            cmd_send(cmd_acc);
            cmd_t *cmd_mag = cmd_get_str("adcs_mag");
            cmd_send(cmd_mag);
            // Set target attitude
            //cmd_t *cmd_point = cmd_get_str("sim_adcs_set_target");
            //cmd_add_params_var(cmd_point, 1.0, 1.0, 1.0, 0.01, 0.01, 0.01);
            int mode;
            mode = dat_get_system_var(dat_obc_opmode);
            cmd_t *cmd_point;
            if(mode == DAT_OBC_OPMODE_REF_POINT)
            {
                cmd_point = cmd_get_str("adcs_set_target");
                cmd_add_params_var(cmd_point, 1.0, 1.0, 1.0, 0.01, 0.01, 0.01);
            } else if(mode == DAT_OBC_OPMODE_NAD_POINT)
            {
                cmd_point = cmd_get_str("adcs_set_to_nadir");
            } else if(mode == DAT_OBC_OPMODE_DETUMB_MAG)
            {
                cmd_point = cmd_get_str("adcs_detumbling_mag");
            }
            cmd_send(cmd_point);
            // Do control loop
            cmd_t *cmd_ctrl;
            if(mode == DAT_OBC_OPMODE_DETUMB_MAG)
            {
                cmd_ctrl = cmd_get_str("adcs_mag_moment");
            } else
            {
                cmd_ctrl = cmd_get_str("adcs_do_control");
                cmd_add_params_var(cmd_ctrl, _adcs_ctrl_period * 1000);
            }
            cmd_send(cmd_ctrl);
            // Send telemetry to ADCS subsystem
            cmd_t *cmd_att = cmd_get_str("adcs_send_attitude");
            cmd_send(cmd_att);
        }

        /* 1 hours actions */
        if((elapsed_msec % _1hour_check) == 0)
        {
            LOGD(tag, "1 hour check");
            cmd_t *cmd_1h = cmd_get_str("drp_add_hrs_alive");
            cmd_add_params_var(cmd_1h, 1); // Add 1hr
            cmd_send(cmd_1h);
        }
    }
}

void eskf_predict_state(double* P, double dt)
{
    LOGI(tag, "Kalman Estimate")
    // Predict Nominal
    quaternion_t q;
//    vector3_t w = {0.1002623,  -0.10142402,  0.20332787};
    vector3_t w;
    vector3_t wb = {0.0, 0.0, 0.0};
    vector3_t diffw;
    _get_sat_quaterion(&q, dat_ads_q0);
    _get_sat_vector(&w, dat_ads_omega_x);

    quaternion_t q_est;
    vec_cons_mult(-1.0, &wb, NULL);
    vec_sum(w, wb, &diffw);
    eskf_integrate(q, diffw, dt, &q_est);
    _set_sat_quaterion(&q_est, dat_ads_q0);
//    _set_sat_vector(&omega, dat_ads_omega_x);

    // Predict Error
    _mat_set_diag(P, 1.0,6, 6);
    double Q[6][6];
    _mat_set_diag(Q, 1.0,6, 6);
    eskf_compute_error(diffw, dt, P, Q);
}
