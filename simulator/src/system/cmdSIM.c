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

void cmd_sim_init(void)
{
    cmd_add("sim_adcs_point", sim_adcs_point, "", 0);
    cmd_add("sim_adcs_quat", sim_adcs_get_quaternion, "", 0);
}

int sim_adcs_point(char* fmt, char* params, int nparams)
{
    csp_packet_t *packet = csp_buffer_get(COM_FRAME_MAX_LEN);
    if(packet == NULL)
        return CMD_FAIL;
    memset(packet->data, 0, COM_FRAME_MAX_LEN);

    value rx, ry, rz;
    rx.i = dat_get_system_var(dat_ads_pos_x);
    ry.i = dat_get_system_var(dat_ads_pos_y);
    rz.i = dat_get_system_var(dat_ads_pos_z);

    int len = snprintf(packet->data, COM_FRAME_MAX_LEN,
      "adcs_point_to %.06f %.06f %.06f", rx.f, ry.f, rz.f);
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