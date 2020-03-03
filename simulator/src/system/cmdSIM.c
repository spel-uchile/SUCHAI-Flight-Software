/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2020, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *      Copyright 2020, Matias Ramirez Martinez, nicoram.mt@gmail.com
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

#include "cmdSIM.h"
#include <math.h>
static const char* tag = "cmdSIM";

#define ADCS_PORT 7

static inline void _get_sat_quaterion(quaternion_t *q);
void _get_sat_quaterion(quaternion_t *q)
{
  int i;
  for(i=0; i<4; i++)
  {
    value v;
    v.i = dat_get_system_var(dat_q_i2b_0+i);
    q->q[i] = (double)v.f;
  }
}

static inline void _set_sat_quaterion(quaternion_t *q);
void _set_sat_quaterion(quaternion_t *q)
{
  int i;
  for(i=0; i<4; i++)
  {
    value v;
    v.f = (float)q->q[i];
    dat_set_system_var(dat_q_i2b_0+i, v.i);
  }
}

static inline void _get_tgt_quaterion(quaternion_t *q);
void _get_tgt_quaterion(quaternion_t *q)
{
  int i;
  for(i=0; i<4; i++)
  {
    value v;
    v.i = dat_get_system_var(dat_q_tgt_0+i);
    q->q[i] = (double)v.f;
  }
}

static inline void _set_tgt_quaterion(quaternion_t *q);
void _set_tgt_quaterion(quaternion_t *q)
{
  int i;
  for(i=0; i<4; i++)
  {
    value v;
    v.f = (float)q->q[i];
    dat_set_system_var(dat_q_tgt_0+i, v.i);
  }
}

static inline void _get_sat_position(vector3_t *r);
void _get_sat_position(vector3_t *r)
{
    int i;
    for(i=0; i<3; i++)
    {
        value v;
        v.i = dat_get_system_var(dat_ads_pos_x+i);
        r->v[i] = (double)v.f;
    }
}

static inline void _set_sat_position(vector3_t *r);
void _set_sat_position(vector3_t *r)
{
    int i;
    for(i=0; i<3; i++)
    {
        value v;
        v.f = (float)r->v[i];
        dat_set_system_var(dat_ads_pos_x+i, v.i);
    }
}

void cmd_sim_init(void)
{
    cmd_add("sim_adcs_point", sim_adcs_point, "", 0);
    cmd_add("sim_adcs_quat", sim_adcs_get_quaternion, "", 0);
    cmd_add("sim_adcs_control", sim_adcs_control_torque, "", 0);
    cmd_add("sim_adcs_to_nadir", sim_adcs_target_nadir, "", 0);
}

int sim_adcs_point(char* fmt, char* params, int nparams)
{
    csp_packet_t *packet = csp_buffer_get(COM_FRAME_MAX_LEN);
    if(packet == NULL)
        return CMD_FAIL;
    memset(packet->data, 0, COM_FRAME_MAX_LEN);

    vector3_t r;
    _get_sat_position(&r);

    int len = snprintf(packet->data, COM_FRAME_MAX_LEN,
      "adcs_point_to %lf %lf %lf", r.v0, r.v1, r.v2);
    packet->length = len;
    LOGI(tag, "ADCS CMD: (%d) %s", packet->length, packet->data);

    int rc = csp_sendto(CSP_PRIO_NORM, ADCS_PORT, SCH_TRX_PORT_CMD,
      SCH_TRX_PORT_CMD, CSP_O_NONE, packet, 1000);

    if(rc != 0)
    {
        csp_buffer_free((void *)packet);
        return CMD_FAIL;
    }

    return CMD_OK;
}

int sim_adcs_get_quaternion(char* fmt, char* params, int nparams)
{
    char *out_buff = (char *)malloc(COM_FRAME_MAX_LEN);
    char *in_buff = (char *)malloc(COM_FRAME_MAX_LEN);
    memset(out_buff, 0, COM_FRAME_MAX_LEN);
    memset(in_buff, 0, COM_FRAME_MAX_LEN);

    int len = snprintf(out_buff, COM_FRAME_MAX_LEN, "adcs_get_quaternion 0");

    int rc = csp_transaction(CSP_PRIO_NORM, ADCS_PORT, SCH_TRX_PORT_CMD, 100,
      out_buff, COM_FRAME_MAX_LEN, in_buff, COM_FRAME_MAX_LEN);

    if(rc != COM_FRAME_MAX_LEN)
    {
        free(out_buff);
        free(in_buff);
        LOGE(tag, "csp_transaction failed! (%d)", rc);
        return CMD_FAIL;
    }

    LOGI(tag, "QUAT: %s", in_buff);
    quaternion_t q;
    rc = sscanf(in_buff, "%lf %lf %lf %lf", &q.q0, &q.q1, &q.q2, &q.q3);
    if(rc != 4)
    {
        free(out_buff);
        free(in_buff);
        LOGE(tag, "Error reading values!")
        return CMD_FAIL;
    }

    _set_sat_quaterion(&q);

    quaternion_t tmp;
    _get_sat_quaterion(&tmp);
    LOGI(tag, "SAT_QUAT: %.04f, %.04f, %.04f, %.04f", tmp.q0, tmp.q1, tmp.q2, tmp.q3);
    free(out_buff);
    free(in_buff);
    return CMD_OK;
}

int sim_adcs_control_torque(char* fmt, char* params, int nparams)
{
    // GLOBALS
    double ctrl_cycle;
    matrix3_t I_quat;
    mat_set_diag(&I_quat, 0.0, 0.0, 0.0);
    matrix3_t P_quat;
    mat_set_diag(&P_quat, 0.00001, 0.00001, 0.00001);
    matrix3_t P_omega;
    mat_set_diag(&P_omega, 0.01, 0.01, 0.01);

    // PARAMETERS
    quaternion_t q_i2b_est; // Current quaternion. Read as from ADCS
    quaternion_t q_i2b_tar; // Target quaternion. Read as parameter
    _get_sat_quaterion(&q_i2b_est);
    _get_tgt_quaterion(&q_i2b_tar);
//    vector3_t omega_b_est;  // Current GYRO. Read from ADCS
//    vector3_t omega_b_tar;  // Target GYRO. Read as parameter
//    _get_sat_omega(&omega_b_est);

//  libra::Quaternion q_b2i_est = q_i2b_est_.conjugate(); //body frame to inertial frame
//  libra::Quaternion q_i2b_now2tar = q_b2i_est * q_i2b_tar_;//q_i2b_tar_ = qi2b_est * qi2b_now2tar：クオータニオンによる2回転は積であらわされる。
//  q_i2b_now2tar.normalize();
    quaternion_t q_b2i_est;
    quat_conjugate(&q_i2b_est, &q_b2i_est);
    quaternion_t q_i2b_now2tar;
    quat_mult(&q_b2i_est, &q_i2b_tar, &q_i2b_now2tar);
    quat_normalize(&q_i2b_now2tar, NULL);

//    Vector<3> TorqueDirection;
//    TorqueDirection[0] = q_i2b_now2tar[0];
//    TorqueDirection[1] = q_i2b_now2tar[1];
//    TorqueDirection[2] = q_i2b_now2tar[2];
//    TorqueDirection = normalize(TorqueDirection);//q1,q2,q3を標準化している。つまり、p=(l,m,n)Tを求めている。
    vector3_t torq_dir;
    memcpy(torq_dir.v, q_i2b_now2tar.vec, sizeof(vector3_t));
    assert(torq_dir.v0 == q_i2b_now2tar.q0 && torq_dir.v1 == q_i2b_now2tar.q1 && torq_dir.v2 == q_i2b_now2tar.q2);
    vec_normalize(&torq_dir, NULL);

//    double AttitudeRotation = 2 * acos(q_i2b_now2tar[3]) * 180 / M_PI; //回転角θ。q_i2b_now2tar[3]は授業ではq0として扱った。
//    error_integral = ctrl_cycle_ * AttitudeRotation * TorqueDirection;
    double att_rot = 2*acos(q_i2b_now2tar.q[3])*180/M_PI;
    vector3_t error_integral;
    vec_cons_mult(att_rot*ctrl_cycle, &torq_dir, &error_integral);

//    Vector<3> ControlTorque = P_quat_ * (AttitudeRotation * TorqueDirection)
//                              + I_quat_ * error_integral
//                              + P_omega_ * (omega_b_tar_ - omega_b_est_);
    vector3_t torque_rot;
    vec_cons_mult(att_rot, &torq_dir, &torque_rot); //(AttitudeRotation * TorqueDirection)
    vector3_t P;
    mat3_vec3_mult(P_quat, torque_rot, &P);  //P_quat_ * (AttitudeRotation * TorqueDirection)
    vector3_t I;
    mat3_vec3_mult(I_quat, error_integral, &I); //I_quat_ * error_integral
//    vector3_t omega_b;
//    vector3_t P_o;
//    vec_cons_mult(-1.0, &omega_b_est, NULL);
//    vec_sum(omega_b_tar, omega_b_est, &omega_b);
//    mat3_vec3_mult(P_omega, omega_b, &P_o); //P_omega_ * (omega_b_tar_ - omega_b_est_);

    vector3_t control_torque_tmp, control_torque;
    vec_sum(P, I, &control_torque);
//    vec_sum(P_o, control_torque_tmp, &control_torque);

    LOGI(tag, "CTRL_TORQUE: %f, %f, %f", control_torque.v0, control_torque.v1, control_torque.v2);

    csp_packet_t *packet = csp_buffer_get(COM_FRAME_MAX_LEN);
    if(packet == NULL)
      return CMD_FAIL;

    int len = snprintf(packet->data, COM_FRAME_MAX_LEN,
                       "adcs_set_torque %.06f %.06f %.06f",
                       control_torque.v0, control_torque.v1, control_torque.v2);
    packet->length = len;
    LOGI(tag, "ADCS CMD: (%d) %s", packet->length, packet->data);

    int rc = csp_sendto(CSP_PRIO_NORM, ADCS_PORT, SCH_TRX_PORT_CMD,
                        SCH_TRX_PORT_CMD, CSP_O_NONE, packet, 100);

    if(rc != 0)
    {
      csp_buffer_free((void *)packet);
      return CMD_FAIL;
    }

    return CMD_OK;
}

int sim_adcs_target_nadir(char* fmt, char* params, int nparams)
{
    vector3_t b_dir;
    vector3_t b_tar;
    vector3_t b_lambda;
    double rot;

    b_dir.v0=0; b_dir.v1=0; b_dir.v2=1;

    vector3_t i_tar;
    _get_sat_position(&i_tar);
    vec_normalize(&i_tar, NULL);

    quaternion_t q_i2b_est;
    _get_sat_quaterion(&q_i2b_est);

    quat_frame_conv(&q_i2b_est, &i_tar, &b_tar);

    vector3_t nadir_b_t;
    vec_normalize(&b_tar, &nadir_b_t);

    vec_outer_product(b_dir, b_tar, &b_lambda);
    vec_normalize(&b_dir, NULL);
    vec_normalize(&b_tar, NULL);
    rot = acos(vec_inner_product(b_dir, b_tar));

    quaternion_t q_b2b_now2tar;
    axis_rotation_to_quat(b_lambda, rot, &q_b2b_now2tar);
    quat_normalize(&q_b2b_now2tar, NULL);

    quaternion_t q_i2b_tar;
    quat_mult(&q_i2b_est, &q_b2b_now2tar, &q_i2b_tar);

    _set_tgt_quaterion(&q_i2b_tar);

    //TODO: Remove this print
    quaternion_t _q;
    _get_tgt_quaterion(&_q);
    LOGI(tag, "TGT QUAT: %lf %lf %lf %lf", _q.q0, _q.q1, _q.q2, _q.q3);

    return CMD_OK;
}