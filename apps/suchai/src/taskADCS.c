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

#include "app/taskADCS.h"

static const char *tag = "ADCS";

void taskADCS(void *param)
{
    LOGI(tag, "Started");

    portTick delay_ms  = 100;            //Task period in [ms]

//    unsigned int elapsed_sec = 0;           // Seconds counter
    unsigned int elapsed_msec = 0;
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

    double P[6][6];
    _mat_set_diag((double*)P, 1.0,6, 6);

    while(1)
    {
        osTaskDelayUntil(&xLastWakeTime, delay_ms); //Suspend task
        elapsed_msec += delay_ms;

        /**
         * Estimate Loop
         */
        if (elapsed_msec % 10 == 0) {
//            double dt = (double) elapsed_msec / 1000.0;
//            eskf_predict_state((double*)P, dt);

            if(elapsed_msec % 500 == 0)
            {

                cmd_t *cmd_tle_prop = cmd_get_str("obc_prop_tle");
                cmd_add_params_str(cmd_tle_prop, "0");
                cmd_send(cmd_tle_prop);

                vector3_t r;
                _get_sat_vector(&r, dat_ads_pos_x);


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

                // Calculate sun direction
                uint32_t curr_time = (uint32_t) time(NULL);
                double jd = unixt_to_jd(curr_time);
                LOGD(tag, "julian day: %f", jd);
                vector3_t  sun_dir;
                calc_sun_pos_i(jd, &sun_dir);
                LOGD(tag, "sun direction: (%f, %f, %f)", sun_dir.v0, sun_dir.v1, sun_dir.v2);

                //  Calculate Magnetic Model
                double dec_year = jd_to_dec(jd);
//                calc_magnetic_model(dec_year, )

                // TODO: call function separately with its own mesuerement freq
//                eskf_update_mag(mag_sensor, mag_i, P, &R, &q_est, &w);
                _set_sat_quaterion(&q_est, dat_ads_q0);
                _set_sat_vector(&w, dat_ads_omega_x);
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
    LOGD(tag, "Kalman Estimate")
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
//    _set_sat_vector(&w, dat_ads_omega_x);

    // Predict Error
    double Q[6][6];
    _mat_set_diag(Q, 1.0,6, 6);
    eskf_compute_error(diffw, dt, P, Q);
}

void calc_magnetic_model(double decyear, double latrad, double lonrad, double altm, double* mag) {
    IgrfCalc(decyear, latrad, lonrad, altm, mag);
    LOGI(tag, "Magnetic field is (%f, %f, %f)", mag[0], mag[1], mag[2]);
}

void calc_sun_pos_i(double jd, vector3_t * sun_dir) {

    // all in degree
//    jd = 2459346.3905324074;
    double au = 149597870.691e3;
    double n = jd - 2451545.0;
    double l = (280.459 + 0.98564736 * n); //% 360
    double m = (357.529 + 0.98560023 * n); //% 360.0
    m *= deg2rad;
    double lam = (l + 1.915 * sin(m) + 0.0200 * sin(2 * m)); // % 360.0
    lam *= deg2rad;
    double e = 23.439 - 3.56e-7 * n;
    e *= deg2rad;

    double r_sun = (1.00014 - 0.01671 * cos(m) - 0.000140 * cos(2 * m)) * au;

    sun_dir->v0 = cos(lam);
    sun_dir->v1 =  cos(e) * sin(lam);
    sun_dir->v2 = sin(lam) * sin(e);
//    float u_v = np.array([np.cos(lam), np.cos(e) * np.sin(lam), np.sin(lam) * np.sin(e)])
//    vec_cons_mult(r_sun, &u_v, sun_dir);
}




double fmod2p(double x)
{
    /* Returns mod 2PI of argument */

    int i;
    double ret_val;

    ret_val=x;
    i= (int)trunc(ret_val/twopi);
    ret_val-=i*twopi;

    if (ret_val<0.0)
        ret_val+=twopi;

    return ret_val;
}

double jd_to_dec(double jd)
{
    double decyear;
    int leapyrs, year;
    double    days, tu, temp;

    /* --------------- find year and days of the year --------------- */
    temp    = jd - 2415019.5;
    tu      = temp / 365.25;
    year    = 1900 + (int)floor(tu);
    leapyrs = (int)floor((year - 1901) * 0.25);

    // optional nudge by 8.64x10-7 sec to get even outputs
    days    = temp - ((year - 1900) * 365.0 + leapyrs) + 0.00000000001;

    /* ------------ check for case of beginning of a year ----------- */
    if (days < 1.0)
    {
        year    = year - 1;
        leapyrs = (int)floor((year - 1901) * 0.25);
        days    = temp - ((year - 1900) * 365.0 + leapyrs);
    }

    decyear = year + days/365.25;
    return decyear;
}


int eci_to_geo(vector3_t sat_pos, double current_jd, vector3_t * lat_lon_alt) {
    double radiusearthkm = 6378.137;     // km
    double f = 1.0 / 298.257223563;

    double r, e2, phi, c;
    double theta;
    double current_side = gstime(current_jd);

    theta = atan2(sat_pos.v1, sat_pos.v0);


    double lon_rad_ = fmod2p(theta - current_side); /* radians */
    r = sqrt(sat_pos.v0 * sat_pos.v0 + sat_pos.v1 * sat_pos.v1);
    e2 = f*(2 - f);
    double lat_rad_ = atan2(sat_pos.v2, r); /* radians */
    do
    {
        phi = lat_rad_;
        c = 1 / sqrt(1 - e2*sin(phi)*sin(phi));
        lat_rad_ = atan2(sat_pos.v2 + radiusearthkm*c*e2*sin(phi) * 1000, r);

    } while (fabs(lat_rad_ - phi) >= 1E-10);
    double alt_m_ = r / cos(lat_rad_) - radiusearthkm*c * 1000; /* kilometers */	//楕円体高さ

    if (lat_rad_ > pi/2.0)
        lat_rad_ -= twopi;

    lat_lon_alt->v0 = lat_rad_;
    lat_lon_alt->v1 = lon_rad_;
    lat_lon_alt->v2 = alt_m_;
}


double unixt_to_jd(uint32_t unix_time) {
    return ( unix_time / 86400.0 ) + 2440587.5;
}


