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

#include "cmdTM.h"

static const char *tag = "cmdTM";

void cmd_tm_init(void)
{
    cmd_add("tm_parse_status", tm_parse_status, "", 0);
    cmd_add("tm_send_status", tm_send_status, "%d", 1);
    cmd_add("tm_send_var", tm_send_var, "%d %s", 2);
    cmd_add("tm_get_last", tm_get_last, "%u", 1);
    cmd_add("tm_get_single", tm_get_single, "%u %u", 2);
    cmd_add("tm_send_last", tm_send_last, "%u %u", 2);
    cmd_add("tm_send_all", tm_send_all, "%u %u", 2);
    cmd_add("tm_send_from", tm_send_from, "%u %u %u", 3);
    cmd_add("tm_set_ack", tm_set_ack, "%u %u", 2);
}

int tm_send_status(char *fmt, char *params, int nparams)
{
    //Format: <node>
    int dest_node;
    if(params == NULL || sscanf(params, fmt, &dest_node) != nparams)
    {
        return CMD_ERROR;
    }

    // Pack status variables to a structure
    int i;
    dat_sys_var_short_t status_buff[dat_status_last_var];
    for(i = 0; i<dat_status_last_var; i++)
    {
        status_buff[i].address = csp_hton16(dat_status_list[i].address);
        status_buff[i].value.u = csp_hton32(dat_get_status_var(dat_status_list[i].address).u);
    }

    // Send telemetry
    return _com_send_data(dest_node, status_buff, sizeof(status_buff), TM_TYPE_STATUS, dat_status_last_var, 0);
}

int tm_send_var(char *fmt, char *params, int nparams)
{
    //Format: <node>
    int dest_node;
    char var_name[SCH_CMD_MAX_STR_PARAMS];

    if(params == NULL || sscanf(params, fmt, &dest_node, var_name) != nparams)
    {
        return CMD_ERROR;
    }

    // Pack status variable to a structure
    dat_sys_var_short_t status_buff[1];
    dat_sys_var_t system_var = dat_get_status_var_def_name(var_name);
    uint16_t address = system_var.address;
    status_buff[0].address = csp_hton16(address);
    status_buff[0].value.u = csp_hton32(dat_get_status_var(address).u);

    // Send telemetry
    return _com_send_data(dest_node, status_buff, sizeof(status_buff), TM_TYPE_STATUS, 1, 0);
}

int tm_parse_status(char *fmt, char *params, int nparams)
{
    if(params == NULL)
        return CMD_ERROR;

    com_frame_t *frame = (com_frame_t *)params;
    dat_sys_var_short_t *status_buff = (dat_sys_var_short_t *)frame->data.data8;

    int i;
    for(i = 0; i<frame->ndata; i++)
    {
        uint16_t address = csp_ntoh16(status_buff[i].address);
        value32_t value = {.u = csp_ntoh32(status_buff[i].value.u)};
        dat_sys_var_t system_var = dat_get_status_var_def(address);
        system_var.value = value;
        dat_print_system_var(&system_var);
    }

    return CMD_OK;
}

void send_tel_from_to(int from, int des, int payload, int dest_node)
{
    int structs_per_frame = (COM_FRAME_MAX_LEN) / data_map[payload].size;
    uint16_t payload_size = data_map[payload].size;

    int n_samples = des-from;
    int n_frames = (n_samples)/structs_per_frame;
    if( (n_samples) % structs_per_frame != 0) {
        n_frames += 1;
    }

    int index_pay = dat_get_system_var(data_map[payload].sys_index);
//    int index_ack = dat_get_system_var(data_map[payload].sys_ack);
//    int delay = index_pay - index_ack;

    // New connection
    csp_conn_t *conn;
    conn = csp_connect(CSP_PRIO_NORM, dest_node, SCH_TRX_PORT_TM, 500, CSP_O_NONE);
    if(conn == NULL)
    {
        LOGE(tag, "Cannot create connection!");
        return;
    }

    int i;
    for(i=0; i < n_frames; ++i) {

        csp_packet_t *packet = csp_buffer_get(sizeof(com_frame_t));
        packet->length = sizeof(com_frame_t);
        memset(packet->data, 0, sizeof(com_frame_t));
        com_frame_t *frame = (com_frame_t *)(packet->data);
        frame->node = SCH_COMM_ADDRESS;
        frame->nframe = csp_hton16((uint16_t) i);
        frame->type = (uint8_t)(TM_TYPE_PAYLOAD + payload);
        frame->ndata = csp_hton32((uint32_t)structs_per_frame);

        int j;
        for(j=0; j < structs_per_frame; ++j) {
            if( (i*structs_per_frame) +j >= n_samples ) {
                frame->ndata = csp_hton32((uint32_t) j);
                break;
            }

            char buff[payload_size];
            dat_get_recent_payload_sample(buff, payload, (n_samples+(index_pay-des)) - (i*structs_per_frame+j+1));
            int mem_offset = (j * payload_size);
            memcpy(frame->data.data8 + mem_offset, buff, payload_size);
        }

        LOGI(tag, "Sending %d structs of payload %d", frame->ndata, (int)payload);
//        _hton32_buff((uint32_t *)frame->data.data32, sizeof(data.frame.data.data8)/ sizeof(uint32_t));
        //_com_send_data(dest_node, data.frame.data.data8, COM_FRAME_MAX_LEN, data.frame.type, data.frame.ndata, data.frame.nframe);
        LOGI(tag, "Node    : %d", frame->node);
        LOGI(tag, "Frame   : %d", frame->nframe);
        LOGI(tag, "Type    : %d", frame->type);
        LOGI(tag, "Samples : %d", frame->ndata);
//        print_buff(frame->data.data8, payload_size*structs_per_frame);

        // Send packet
        int rc_send = csp_send(conn, packet, 500);
        if(rc_send == 0)
        {
            csp_buffer_free(packet);
            LOGE(tag, "Error sending frame %d! (%d)", i, rc_send);
            break;
        }

        if(i%10 == 0)
        {
            csp_close(conn);
            osDelay(3000);
            conn = csp_connect(CSP_PRIO_NORM, dest_node, SCH_TRX_PORT_TM, 500, CSP_O_NONE);
        }
    }

    // Close connection
    int rc_conn = csp_close(conn);
    if(rc_conn != CSP_ERR_NONE)
        LOGE(tag, "Error closing connection! (%d)", rc_conn);
}

int tm_get_single(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_ERROR;
    }

    uint32_t payload;
    uint32_t index;

    if(nparams == sscanf(params, fmt, &payload, &index))
    {
        if(payload >= last_sensor) {
            return CMD_FAIL;
        }

        int payload_size = data_map[payload].size;
        char buff[payload_size];
        int ret;
        ret = dat_get_payload_sample(buff, payload, index);
        dat_print_payload_struct(buff, payload);

        if( ret == -1) {
            return CMD_ERROR;
        }

        return CMD_OK;
    }

    return CMD_ERROR;
}

int tm_get_last(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_ERROR;
    }

    uint32_t payload;
    if(nparams == sscanf(params, fmt, &payload))
    {
        if(payload >= last_sensor) {
            return CMD_FAIL;
        }

        int index_pay = dat_get_system_var(data_map[payload].sys_index);

        if(index_pay < 1){
            return CMD_FAIL;
        }
        int payload_size = data_map[payload].size;
        char buff[payload_size];
        int ret;
        ret = dat_get_recent_payload_sample(buff, payload,0);
        //FIXME: Check memory usage
        dat_print_payload_struct(buff, payload);

        if( ret == -1) {
            return CMD_ERROR;
        }

        return CMD_OK;
    }
    else
    {
        LOGW(tag, "Invalid args!");
        return CMD_FAIL;
    }
}

int tm_send_last(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_ERROR;
    }

    uint32_t dest_node;
    uint32_t payload;
    if(nparams == sscanf(params, fmt, &payload, &dest_node))
    {
        if(payload >= last_sensor) {
            return CMD_FAIL;
        }
        int index_pay = dat_get_system_var(data_map[payload].sys_index);

        if(index_pay < 1){
            return CMD_FAIL;
        }

        int structs_per_frame = (COM_FRAME_MAX_LEN) / data_map[payload].size;
        if( (structs_per_frame)  > index_pay ) {
            structs_per_frame = index_pay;
        }

        send_tel_from_to(index_pay-structs_per_frame, index_pay, payload, dest_node);
        return CMD_OK;
    }
    else
    {
        LOGW(tag, "Invalid args!");
        return CMD_FAIL;
    }
}

int tm_send_all(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_ERROR;
    }

    uint32_t dest_node;
    uint32_t payload;

    if(nparams == sscanf(params, fmt, &payload, &dest_node)) {

        if(payload >= last_sensor) {
            return CMD_FAIL;
        }
        int index_pay = dat_get_system_var(data_map[payload].sys_index);
        int index_ack = dat_get_system_var(data_map[payload].sys_ack);
        send_tel_from_to(index_ack, index_pay, payload, dest_node);
        return CMD_OK;
    }
    else
    {
        return CMD_ERROR;
    }
}

int tm_send_from(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_ERROR;
    }

    uint32_t dest_node;
    uint32_t payload;
    uint32_t samples;

    if(nparams == sscanf(params, fmt, &payload, &dest_node, &samples)) {

        if(payload >= last_sensor) {
            return CMD_FAIL;
        }

        if(samples < 1) {
            return CMD_FAIL;
        }

        int index_pay = dat_get_system_var(data_map[payload].sys_index);
        int index_ack = dat_get_system_var(data_map[payload].sys_ack);

        int des = index_ack + samples;
        if(des > index_pay) {
            des = index_pay;
        }

        send_tel_from_to(index_ack, des, payload, dest_node);
        return CMD_OK;
    }
    else
    {
        return CMD_ERROR;
    }
}


int tm_set_ack(char *fmt, char *params, int nparams) {
    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_ERROR;
    }

    uint32_t payload;
    uint32_t k_samples;

    if(nparams == sscanf(params, fmt, &payload, &k_samples)) {

        if(payload >= last_sensor) {
            LOGE(tag, "payload not found")
            return CMD_FAIL;
        }

        if(k_samples < 1) {
            LOGE(tag, "could not acknowledge %d", k_samples)
            return CMD_FAIL;
        }

        int ack_pay = dat_get_system_var(data_map[payload].sys_ack);
        int index_pay = dat_get_system_var(data_map[payload].sys_index);

        if(ack_pay== -1 || index_pay==-1) {
            LOGE(tag, "something bad happen");
            return CMD_FAIL;
        }

        if( ack_pay > k_samples) {
            return CMD_FAIL;
        }

        if( k_samples > index_pay) {
            ack_pay = index_pay;
        } else  {
            ack_pay = k_samples;
        }

        dat_set_system_var(data_map[payload].sys_ack, ack_pay);
        return CMD_OK;
    }
    else
    {
        return CMD_ERROR;
    }
}
