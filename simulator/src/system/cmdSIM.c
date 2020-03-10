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

void cmd_sim_init(void)
{
    cmd_add("sim_adcs_point", sim_adcs_point, "", 0);
    cmd_add("sim_adcs_quat", sim_adcs_get_quaternion, "", 0);
    cmd_add("sim_adcs_acc", sim_adcs_get_acc, "", 0);
    cmd_add("sim_adcs_mag", sim_adcs_get_mag, "", 0);
    cmd_add("sim_adcs_do_control", sim_adcs_control_torque, "%lf", 1);
    cmd_add("sim_adcs_set_target", sim_adcs_set_target, "%lf %lf %lf %lf %lf %lf", 6);
    cmd_add("sim_adcs_set_to_nadir", sim_adcs_target_nadir, "", 0);
    cmd_add("sim_adcs_send_attitude", sim_adcs_send_attitude, "", 0);
}

static inline void _get_sat_quaterion(quaternion_t *q, dat_system_t index);
void _get_sat_quaterion(quaternion_t *q,  dat_system_t index)
{
  int i;
  for(i=0; i<4; i++)
  {
    assert(index+i < dat_system_last_var);
    value v;
    v.i = dat_get_system_var(index+i);
    q->q[i] = (double)v.f;
  }
}
static inline void _set_sat_quaterion(quaternion_t *q,  dat_system_t index);
void _set_sat_quaterion(quaternion_t *q,  dat_system_t index)
{
  int i;
  for(i=0; i<4; i++)
  {
    assert(index+i < dat_system_last_var);
    value v;
    v.f = (float)q->q[i];
    dat_set_system_var(index+i, v.i);
  }
}
static inline void _get_sat_vector(vector3_t *r, dat_system_t index);
void _get_sat_vector(vector3_t *r, dat_system_t index)
{
    int i;
    for(i=0; i<3; i++)
    {
        assert(index+i < dat_system_last_var);
        value v;
        v.i = dat_get_system_var(index+i);
        r->v[i] = (double)v.f;
    }
}
static inline void _set_sat_vector(vector3_t *r, dat_system_t index);
void _set_sat_vector(vector3_t *r, dat_system_t index)
{
    int i;
    for(i=0; i<3; i++)
    {
        assert(index+i < dat_system_last_var);
        value v;
        v.f = (float)r->v[i];
        dat_set_system_var(index+i, v.i);
    }
}


int sim_adcs_point(char* fmt, char* params, int nparams)
{
    csp_packet_t *packet = csp_buffer_get(COM_FRAME_MAX_LEN);
    if(packet == NULL)
        return CMD_FAIL;
    memset(packet->data, 0, COM_FRAME_MAX_LEN);

    vector3_t r;
    _get_sat_vector(&r, dat_ads_pos_x);

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

    if(rc == COM_FRAME_MAX_LEN)
    {
        LOGI(tag, "QUAT: %s", in_buff);
        quaternion_t q;
        rc = sscanf(in_buff, "%lf %lf %lf %lf", &q.q0, &q.q1, &q.q2, &q.q3);
        if(rc == 4)
        {
            _set_sat_quaterion(&q, dat_ads_q0);
            quaternion_t tmp;
            _get_sat_quaterion(&tmp, dat_ads_q0);
            LOGI(tag, "SAT_QUAT: %.04f, %.04f, %.04f, %.04f", tmp.q0, tmp.q1, tmp.q2, tmp.q3);
            free(out_buff);
            free(in_buff);
            return CMD_OK;
        }
        LOGE(tag, "Error reading values!")
    }
    LOGE(tag, "csp_transaction failed! (%d)", rc);

    free(out_buff);
    free(in_buff);
    return CMD_FAIL;
}

int sim_adcs_get_acc(char* fmt, char* params, int nparams)
{
    char *out_buff = (char *)malloc(COM_FRAME_MAX_LEN);
    char *in_buff = (char *)malloc(COM_FRAME_MAX_LEN);
    memset(out_buff, 0, COM_FRAME_MAX_LEN);
    memset(in_buff, 0, COM_FRAME_MAX_LEN);

    int len = snprintf(out_buff, COM_FRAME_MAX_LEN, "adcs_get_acc 0");

    int rc = csp_transaction(CSP_PRIO_NORM, ADCS_PORT, SCH_TRX_PORT_CMD, 100,
                             out_buff, COM_FRAME_MAX_LEN, in_buff, COM_FRAME_MAX_LEN);

    if(rc == COM_FRAME_MAX_LEN)
    {
        LOGI(tag, "ACC: %s", in_buff);
        vector3_t acc;
        rc = sscanf(in_buff, "%lf %lf %lf", &acc.v0, &acc.v1, &acc.v2);
        if(rc == 3)
        {
            _set_sat_vector(&acc, dat_ads_acc_x);
            vector3_t tmp;
            _get_sat_vector(&tmp, dat_ads_acc_x);
            LOGI(tag, "SAT_ACC: %lf, %lf, %lf", tmp.v0, tmp.v1, tmp.v2);
            free(out_buff);
            free(in_buff);
            return CMD_OK;
        }
        LOGE(tag, "Error reading values!")
    }
    LOGE(tag, "csp_transaction failed! (%d)", rc);

    free(out_buff);
    free(in_buff);
    return CMD_FAIL;
}

int sim_adcs_get_mag(char* fmt, char* params, int nparams)
{
    char *out_buff = (char *)malloc(COM_FRAME_MAX_LEN);
    char *in_buff = (char *)malloc(COM_FRAME_MAX_LEN);
    memset(out_buff, 0, COM_FRAME_MAX_LEN);
    memset(in_buff, 0, COM_FRAME_MAX_LEN);

    int len = snprintf(out_buff, COM_FRAME_MAX_LEN, "adcs_get_mag 0");

    int rc = csp_transaction(CSP_PRIO_NORM, ADCS_PORT, SCH_TRX_PORT_CMD, 100,
                             out_buff, COM_FRAME_MAX_LEN, in_buff, COM_FRAME_MAX_LEN);

    if(rc == COM_FRAME_MAX_LEN)
    {
        LOGI(tag, "MAG: %s", in_buff);
        vector3_t mag;
        rc = sscanf(in_buff, "%lf %lf %lf", &mag.v0, &mag.v1, &mag.v2);
        if(rc == 3)
        {
            _set_sat_vector(&mag, dat_ads_mag_x);
            vector3_t tmp;
            _get_sat_vector(&tmp, dat_ads_mag_x);
            LOGI(tag, "SAT_MAG: %lf, %lf, %lf", tmp.v0, tmp.v1, tmp.v2);
            free(out_buff);
            free(in_buff);
            return CMD_OK;
        }
        LOGE(tag, "Error reading values!")
    }
    LOGE(tag, "csp_transaction failed! (%d)", rc);

    free(out_buff);
    free(in_buff);
    return CMD_FAIL;
}

int sim_adcs_control_torque(char* fmt, char* params, int nparams)
{
    // GLOBALS
    double ctrl_cycle;
    matrix3_t I_quat;
    mat_set_diag(&I_quat, 0.0, 0.0, 0.0);
    matrix3_t P_quat;
    mat_set_diag(&P_quat, 0.0012, 0.0012, 0.0012);
    matrix3_t P_omega;
    mat_set_diag(&P_omega, 0.003, 0.003, 0.003);

    if(params == NULL || sscanf(params, fmt, &ctrl_cycle) != nparams)
        return CMD_FAIL;

    // PARAMETERS
    quaternion_t q_i2b_est; // Current quaternion. Read as from ADCS
    quaternion_t q_i2b_tar; // Target quaternion. Read as parameter
    _get_sat_quaterion(&q_i2b_est, dat_ads_q0);
    _get_sat_quaterion(&q_i2b_tar, dat_tgt_q0);
    vector3_t omega_b_est;  // Current GYRO. Read from ADCS
    _get_sat_vector(&omega_b_est, dat_ads_acc_x);
    vector3_t omega_b_tar;
    _get_sat_vector(&omega_b_tar, dat_tgt_acc_x);

//    libra::Quaternion q_b2i_est = q_i2b_est_.conjugate(); //body frame to inertial frame
//    libra::Quaternion q_i2b_now2tar = q_b2i_est * q_i2b_tar_;//q_i2b_tar_ = qi2b_est * qi2b_now2tar：クオータニオンによる2回転は積であらわされる。
//    q_i2b_now2tar.normalize();
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

//    double AttitudeRotation = 2 * acos(q_i2b_now2tar[3]);
//    error_integral += (AttitudeRotation * ctrl_cycle_ *10E-3) * TorqueDirection;
    double att_rot = 2*acos(q_i2b_now2tar.q[3]);
    vector3_t error_integral; //TODO: SUM?
    vec_cons_mult(att_rot*ctrl_cycle*1e-3, &torq_dir, &error_integral);

//    Vector<3> ControlTorque = (P_quat_ * (AttitudeRotation * TorqueDirection))
//                              + (I_quat_ * error_integral)
//                              + (P_omega_ * (omega_b_tar_ - omega_b_est_));
    vector3_t torque_rot;
    vec_cons_mult(att_rot, &torq_dir, &torque_rot); //(AttitudeRotation * TorqueDirection)
    vector3_t P;
    mat3_vec3_mult(P_quat, torque_rot, &P);  //P_quat_ * (AttitudeRotation * TorqueDirection)
    vector3_t I;
    mat3_vec3_mult(I_quat, error_integral, &I); //I_quat_ * error_integral
    vector3_t omega_b;
    vector3_t P_o;
    vec_cons_mult(-1.0, &omega_b_est, NULL);
    vec_sum(omega_b_tar, omega_b_est, &omega_b);
    mat3_vec3_mult(P_omega, omega_b, &P_o); //P_omega_ * (omega_b_tar_ - omega_b_est_);

    vector3_t control_torque_tmp, control_torque;
    vec_sum(P, I, &control_torque_tmp);
    vec_sum(P_o, control_torque_tmp, &control_torque);

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

int sim_adcs_set_target(char* fmt, char* params, int nparams)
{
    double rot;
    vector3_t i_tar;  // Target vector, intertial frame, read as parameter
    vector3_t b_tar;
    vector3_t b_dir;  // Face to point to, body frame
    vector3_t b_lambda;
    vector3_t omega_tar;  // Target velocity vector, body frame, read as parameter
    quaternion_t q_i2b_est;
    quaternion_t q_b2b_now2tar;
    quaternion_t q_i2b_tar; // Target quaternion, inertial to body frame. Calculate

//    if(params == NULL || sscanf(params, fmt, &i_tar.v0, &i_tar.v1, &i_tar.v2, &omega_tar.v0, &omega_tar.v1, &omega_tar.v2) != nparams)
//        return CMD_ERROR;
    int p = sscanf(params, fmt, &i_tar.v0, &i_tar.v1, &i_tar.v2, &omega_tar.v0, &omega_tar.v1, &omega_tar.v2);
    LOGW(tag, fmt, fmt, i_tar.v0, i_tar.v1, i_tar.v2,
         omega_tar.v0, omega_tar.v1, omega_tar.v2)
    // Set Z+ [0, 0, 1] as the face to point to
    b_dir.v0 = 0.0; b_dir.v1 = 0.0; b_dir.v2 = 1.0;
    vec_normalize(&b_dir, NULL);

    // Get target vector in body frame
    _get_sat_quaterion(&q_i2b_est, dat_ads_q0);
    vec_normalize(&i_tar, NULL);
    quat_frame_conv(&q_i2b_est, &i_tar, &b_tar);
    vec_normalize(&b_tar, NULL);

    // Get I2B target quaternion
    vec_outer_product(b_dir, b_tar, &b_lambda);
    vec_normalize(&b_lambda, NULL);
    rot = acos(vec_inner_product(b_dir, b_tar));
    axis_rotation_to_quat(b_lambda, rot, &q_b2b_now2tar); //Calculate quaternion of shaft rotation
    quat_normalize(&q_b2b_now2tar, NULL);
    quat_mult(&q_i2b_est, &q_b2b_now2tar, &q_i2b_tar); //Calculate quaternion after rotation

    _set_sat_quaterion(&q_i2b_tar, dat_tgt_q0);
    _set_sat_vector(&omega_tar, dat_tgt_acc_x);

    //TODO: Remove this print
    quaternion_t _q;
    _get_sat_quaterion(&_q, dat_tgt_q0);
    LOGI(tag, "TGT QUAT: %lf %lf %lf %lf", _q.q0, _q.q1, _q.q2, _q.q3);

    return CMD_OK;
}

int sim_adcs_target_nadir(char* fmt, char* params, int nparams)
{
    // Get Nadir vector
    vector3_t i_tar;
    _get_sat_vector(&i_tar, dat_ads_pos_x);
    vec_cons_mult(-1.0, &i_tar, NULL);
    vec_normalize(&i_tar, NULL);

    // Get required Nadir velocity
    // Target GYRO. ECI frame. For LEO sat -> nadir
    vector3_t omega_i_tar;
    omega_i_tar.v[0] = 0.00038772452510785394;
    omega_i_tar.v[1] = -0.0010289117582512489;
    omega_i_tar.v[2] = -0.00014131086334682821;
    vector3_t omega_b_tar;
    quaternion_t q_i2b_est;
    _get_sat_quaterion(&q_i2b_est, dat_ads_q0);
    quat_frame_conv(&q_i2b_est, &omega_i_tar, &omega_b_tar);

    char *_fmt = "%lf %lf %lf %lf %lf %lf";
    char _params[SCH_CMD_MAX_STR_PARAMS];
    snprintf(_params, SCH_CMD_MAX_STR_PARAMS, fmt, i_tar.v0, i_tar.v1, i_tar.v2,
             omega_b_tar.v0, omega_b_tar.v1, omega_b_tar.v2);
    int ret = sim_adcs_set_target(_fmt, _params, 6);

    return ret;
}

int sim_adcs_send_attitude(char* fmt, char* params, int nparams)
{
  quaternion_t q_est, q_tgt;
  _get_sat_quaterion(&q_est, dat_ads_q0);
  _get_sat_quaterion(&q_tgt, dat_tgt_q0);

  csp_packet_t *packet = csp_buffer_get(COM_FRAME_MAX_LEN);
  if(packet == NULL)
    return CMD_FAIL;

  int len = snprintf(packet->data, COM_FRAME_MAX_LEN,
                     "adcs_set_attitude %lf %lf %lf %lf %lf %lf %lf %lf",
                     q_est.q0, q_est.q1, q_est.q2, q_est.q3,
                     q_tgt.q0, q_tgt.q1, q_tgt.q2, q_tgt.q3);
  packet->length = len;
  LOGI(tag, "OBC ATT: (%d) %s", packet->length, packet->data);

  int rc = csp_sendto(CSP_PRIO_NORM, ADCS_PORT, SCH_TRX_PORT_CMD,
                      SCH_TRX_PORT_CMD, CSP_O_NONE, packet, 100);

  if(rc != 0)
  {
    csp_buffer_free((void *)packet);
    return CMD_FAIL;
  }

  return CMD_OK;
}