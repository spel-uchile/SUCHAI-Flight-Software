/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2020, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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


void quat_mult(double *lhs, double *rhs, double *res) {
    res[0] = lhs[3]*rhs[0]-lhs[2]*rhs[1]+lhs[1]*rhs[2]+lhs[0]*rhs[3];
    res[1] = lhs[2]*rhs[0]+lhs[3]*rhs[1]-lhs[0]*rhs[2]+lhs[1]*rhs[3];
    res[2] =-lhs[1]*rhs[0]+lhs[0]*rhs[1]+lhs[3]*rhs[2]+lhs[2]*rhs[3];
    res[3] =-lhs[0]*rhs[0]-lhs[1]*rhs[1]-lhs[2]*rhs[2]+lhs[3]*rhs[3];
}

void quat_normalize(double *q, double *res)
{
    double n = 0.0;
    for(int i=0; i<4 ; ++i)
    {
        n += pow(q[i], 2.0);
    }
    if (n == 0.0){return;}
    n = 1.0/ sqrt(n);

    res[0] = q[0] * n;
    res[1] = q[1] * n;
    res[2] = q[2] * n;
    res[3] = q[3] * n;
}

void quat_inverse(double *q, double *res)
{
    double temp1[4];

    quat_normalize(q, temp1);
    quat_conjugate(temp1, res);
}

void quat_conjugate(double *q, double *res)
{
    res[0] = -q[0];
    res[1] = -q[1];
    res[2] = -q[2];
    res[3] = q[3];
}

void quat_frame_conv(double *q_rot_a2b, double *v_a, double *v_b)
{
    double q0 = q_rot_a2b[3]; // real part
    double q1 = q_rot_a2b[0]; // i
    double q2 = q_rot_a2b[1]; // j
    double q3 = q_rot_a2b[2]; // k

    v_b[0] = (2.0 * pow(q0, 2.0) - 1.0 + 2.0 * pow(q1, 2.0)) * v_a[0] + (2 * q1 * q2 + 2.0 * q0 * q3) * v_a[1] + (2.0 * q1 * q3 - 2.0 * q0 * q2) * v_a[2];
    v_b[1] = (2.0 * q1 * q2 - 2.0 * q0 * q3) * v_a[0] + (2 * pow(q2, 2.0) + 2.0 * pow(q0, 2.0) - 1.0) * v_a[1] + (2.0 * q2 * q3 + 2.0 * q0 * q1) * v_a[2];
    v_b[2] = (2.0 * q1 * q3 + 2.0 * q0 * q2) * v_a[0] + (2 * q2 * q3 - 2.0 * q0 * q1) * v_a[1] + (2.0 * pow(q3, 2.0) + 2.0 * pow(q0, 2.0) - 1.0) * v_a[2];
}
