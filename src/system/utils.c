/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2020, Camilo Roja Milla, camrojas@uchile.cl
 *      Copyright 2020, Elias Obreque Sepulveda, elias.obreque@uchile.cl
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
 
#include "utils.h"

osSemaphore log_mutex;  ///< Sync logging functions, require initialization
void (*log_function)(const char *lvl, const char *tag, const char *msg, ...);
log_level_t log_lvl;
uint8_t log_node;

void log_print(const char *lvl, const char *tag, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    fprintf(LOGOUT,"[%s][%lu][%s] ", lvl, (unsigned long)dat_get_time(), tag);
    vfprintf(LOGOUT, msg, args);
    fprintf(LOGOUT,CRLF); fflush(LOGOUT);
    va_end(args);
}

void log_send(const char *lvl, const char *tag, const char *msg, ...)
{
    // Create a packet for the log message
    csp_packet_t *packet = csp_buffer_get(SCH_BUFF_MAX_LEN);
    if(packet == NULL)
        return;

    // Format message with variadic arguments
    va_list args;
    va_start(args, msg);
    int len = vsnprintf((char *)(packet->data), SCH_BUFF_MAX_LEN, msg, args);
    va_end(args);

    // Make sure its is a null terminating string
    packet->length = len;
    packet->data[len] = '\0';

    // Sending message without connection nor reply.
    int rc = csp_sendto(CSP_PRIO_NORM, (uint8_t)log_node, SCH_TRX_PORT_DBG,
                        SCH_TRX_PORT_DBG, CSP_O_NONE, packet, 100);
    if(rc != 0)
        csp_buffer_free((void *)packet);
}

void log_set(log_level_t level, int node)
{
    osSemaphoreTake(&log_mutex, portMAX_DELAY);
    log_lvl = level;
    log_node = (uint8_t)node;
    log_function = node > 0 ? log_send : log_print;
    osSemaphoreGiven(&log_mutex);
}

int log_init(log_level_t level, int node)
{
    int rc = osSemaphoreCreate(&log_mutex);
    log_set(level, node);
    return rc;
}

void axis_rotation_to_quat(vector3_t axis, double rot, quaternion_t * res )
{
    rot *= 0.5;
    res->scalar = cos(rot);

    for(int i=0; i < 3; ++i) {
        res->vec[i] = axis.v[i] * sin(rot);
    }
}

void quat_sum(quaternion_t *q1, quaternion_t *q2, quaternion_t *res)
{
    int i;
    for(i = 0; i<4; i++)
       res->q[i] = q1->q[i] + q2->q[i];
}

void quat_mult(quaternion_t *lhs, quaternion_t *rhs, quaternion_t *res) {
    res->q[0] = lhs->q[3]*rhs->q[0]-lhs->q[2]*rhs->q[1]+lhs->q[1]*rhs->q[2]+lhs->q[0]*rhs->q[3];
    res->q[1] = lhs->q[2]*rhs->q[0]+lhs->q[3]*rhs->q[1]-lhs->q[0]*rhs->q[2]+lhs->q[1]*rhs->q[3];
    res->q[2] =-lhs->q[1]*rhs->q[0]+lhs->q[0]*rhs->q[1]+lhs->q[3]*rhs->q[2]+lhs->q[2]*rhs->q[3];
    res->q[3] =-lhs->q[0]*rhs->q[0]-lhs->q[1]*rhs->q[1]-lhs->q[2]*rhs->q[2]+lhs->q[3]*rhs->q[3];
}

void quat_normalize(quaternion_t *q, quaternion_t *res)
{
    double n = 0.0;
    for(int i=0; i<4 ; i++)
        n += pow(q->q[i], 2.0);

    if (n == 0.0)
      return;

    n = 1.0/sqrt(n);
    for(int i=0; i<4; i++)
      if(res != NULL)
        res->q[i] = q->q[i]*n;
      else
        q->q[i] = q->q[i]*n;
}

void quat_inverse(quaternion_t *q, quaternion_t *res)
{
    quaternion_t temp1;
    quat_normalize(q, &temp1);
    quat_conjugate(&temp1, res);
}

void quat_conjugate(quaternion_t *q, quaternion_t *res)
{
    res->q[0] = -q->q[0];
    res->q[1] = -q->q[1];
    res->q[2] = -q->q[2];
    res->q[3] = q->q[3];
}

void quat_frame_conv(quaternion_t *q_rot_a2b, vector3_t *v_a, vector3_t *v_b)
{
    double q0 = q_rot_a2b->q3; // real part
    double q1 = q_rot_a2b->q0; // i
    double q2 = q_rot_a2b->q1; // j
    double q3 = q_rot_a2b->q2; // k

    v_b->v[0] = (2.0 * pow(q0, 2.0) - 1.0 + 2.0 * pow(q1, 2.0)) * v_a->v[0] + (2 * q1 * q2 + 2.0 * q0 * q3) * v_a->v[1] + (2.0 * q1 * q3 - 2.0 * q0 * q2) * v_a->v[2];
    v_b->v[1] = (2.0 * q1 * q2 - 2.0 * q0 * q3) * v_a->v[0] + (2 * pow(q2, 2.0) + 2.0 * pow(q0, 2.0) - 1.0) * v_a->v[1] + (2.0 * q2 * q3 + 2.0 * q0 * q1) * v_a->v[2];
    v_b->v[2] = (2.0 * q1 * q3 + 2.0 * q0 * q2) * v_a->v[0] + (2 * q2 * q3 - 2.0 * q0 * q1) * v_a->v[1] + (2.0 * pow(q3, 2.0) + 2.0 * pow(q0, 2.0) - 1.0) * v_a->v[2];
}

double vec_norm(vector3_t vec)
{
    double res = 0.0;
    for(size_t i=0; i<3; ++i){
        res += pow(vec.v[i], 2.0);
    }
    return sqrt(res);
}

int vec_normalize(vector3_t *vec, vector3_t *res)
{
    double n = vec_norm(*vec);
    if(n == 0.0) { return 0; }

    n = 1.0/n;
    for(int i=0; i<3; ++i){
        if(res != NULL)
            res->v[i] = vec->v[i]*n;
        else
            vec->v[i]*=n;
    }
    return 1;
}

double vec_inner_product(vector3_t lhs, vector3_t rhs)
{
    double res = 0;
    for(int i=1; i<3; ++i) {
        res += lhs.v[i] * rhs.v[i];
    }
    return res;
}

void vec_outer_product(vector3_t lhs, vector3_t rhs, vector3_t * res)
{
    res->v[0] = lhs.v[1]*rhs.v[2]-lhs.v[2]*rhs.v[1];
    res->v[1] = lhs.v[2]*rhs.v[0]-lhs.v[0]*rhs.v[2];
    res->v[2] = lhs.v[0]*rhs.v[1]-lhs.v[1]*rhs.v[0];
}

double vec_angle(vector3_t v1, vector3_t v2)
{
    double cos = vec_inner_product(v1, v2) / (vec_norm(v1)*vec_norm(v2));
    return acos(cos);
}

void vec_sum(vector3_t lhs, vector3_t rhs, vector3_t * res)
{
    for(int i=0; i<3; ++i) {
        res->v[i] = lhs.v[i]+rhs.v[i];
    }
}

void vec_cons_mult(double a, vector3_t *vec, vector3_t *res)
{
    for(int i=0; i<3; ++i){
        if(res != NULL)
            res->v[i] = vec->v[i] * a;
        else
            vec->v[i] = vec->v[i] * a;
    }
}

void mat3_vec3_mult(matrix3_t mat, vector3_t vec, vector3_t * res)
{
    for(int i=0; i<3; ++i)
    {
        res->v[i] = 0.0;
        for(int j=0; j<3; ++j)
        {
            res->v[i] += mat.m[i][j]*vec.v[j];
        }
    }
}

void mat_set_diag(matrix3_t *m, double a, double b, double c)
{
    m->row0[0] = a; m->row0[1] = 0; m->row0[2] = 0;
    m->row1[0] = 0; m->row1[1] = b; m->row1[2] = 0;
    m->row2[0] = 0; m->row2[1] = 0; m->row2[2] = c;
}