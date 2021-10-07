/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2021, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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

#include "suchai/cmdCOM.h"

static const char *tag = "cmdCOM";

void cmd_com_init(void)
{
    cmd_add("com_ping", com_ping, "%d", 1);
    cmd_add("com_send_rpt", com_send_rpt, "%d %s", 2);
    cmd_add("com_send_cmd", com_send_cmd, "%d %n", 2);
    cmd_add("com_send_tc", com_send_tc_frame, "%d %n", 2);
    cmd_add("com_send_data", com_send_data, "%d %d %n", 3);
    cmd_add("com_debug", com_debug, "", 0);
    cmd_add("com_set_time_node", com_set_time_node, "%d", 1);
    cmd_add("com_set_tle_node", com_set_tle_node, "%d %s", 2);
}

int com_ping(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Null arguments!");
        return CMD_SYNTAX_ERROR;
    }

    int node;
    if(sscanf(params, fmt, &node) == nparams)
    {
        int rc = csp_ping((uint8_t)node, 3000, 10, CSP_O_NONE);
        LOGR(tag, "Ping to %d took %d", node, rc);
        if(rc > 0)
            return CMD_OK;
        else
            return CMD_ERROR;
    }
    return CMD_SYNTAX_ERROR;
}

int com_send_rpt(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Null arguments!");
        return CMD_SYNTAX_ERROR;
    }

    int node;
    char msg[SCH_CMD_MAX_STR_PARAMS];
    memset(msg, '\0', SCH_CMD_MAX_STR_PARAMS);

    // format: <node> <string>
    if(sscanf(params, fmt, &node, msg) == nparams)
    {
        // Create a packet with the message
        size_t msg_len = strlen(msg);
        csp_packet_t *packet = csp_buffer_get(msg_len+1);
        if(packet == NULL)
        {
            LOGE(tag, "Could not allocate packet!");
            return CMD_ERROR;
        }
        packet->length = (uint16_t)(msg_len+1);
        memcpy(packet->data, msg, msg_len+1);

        // Sending message to node RPT, do not require direct answer
        int rc = csp_sendto(CSP_PRIO_NORM, (uint8_t)node, SCH_TRX_PORT_RPT,
                            SCH_TRX_PORT_RPT, CSP_O_NONE, packet, 1000);

        if(rc == 0)
        {
            LOGV(tag, "Data sent to repeater successfully. (rc: %d, re: %s)", rc, msg);
            return CMD_OK;
        }
        else
        {
            LOGE(tag, "Error sending data to repeater. (rc: %d)", rc);
            csp_buffer_free(packet);
            return CMD_ERROR;
        }
    }

    LOGE(tag, "Error parsing parameters!");
    return CMD_SYNTAX_ERROR;
}

int com_send_cmd(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Null arguments!");
        return CMD_SYNTAX_ERROR;
    }

    int node, next, n_args;
    uint8_t rep[1];
    char msg[SCH_CMD_MAX_STR_PARAMS];
    memset(msg, '\0', SCH_CMD_MAX_STR_PARAMS);

    //format: <node> <command> [parameters]
    n_args = sscanf(params, fmt, &node, &next);
    if(n_args == nparams-1 && next > 1)
    {
        strncpy(msg, params+next, (size_t)SCH_CMD_MAX_STR_PARAMS);
        LOGV(tag, "Parsed %d: %d, %s (%d))", n_args, node, msg, next);

        // Sending message to node TC port and wait for response
        int rc = csp_transaction(1, (uint8_t)node, SCH_TRX_PORT_TC, 1000,
                                 (void *)msg, (int)strlen(msg), rep, 1);

        if(rc > 0 && rep[0] == 200)
        {
            LOGV(tag, "Command sent successfully. (rc: %d, re: %d)", rc, rep[0]);
            return CMD_OK;
        }
        else
        {
            LOGE(tag, "Error sending command. (rc: %d, re: %d)", rc, rep[0]);
            return CMD_ERROR;
        }
    }

    LOGE(tag, "Error parsing parameters!");
    return CMD_SYNTAX_ERROR;
}

int com_send_tc_frame(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Null arguments!");
        return CMD_SYNTAX_ERROR;
    }

    int node, next, n_args;
    uint8_t rep[1];
    char tc_frame[COM_FRAME_MAX_LEN];
    memset(tc_frame, '\0', COM_FRAME_MAX_LEN);

    //format: <node> <command> [parameters];...;<command> [parameters]
    n_args = sscanf(params, fmt, &node, &next);
    if(n_args == nparams-1 && next > 1)
    {
        strncpy(tc_frame, params+next, (size_t)COM_FRAME_MAX_LEN-1);
        LOGV(tag, "Parsed %d: %d, %s (%d))", n_args, node, tc_frame, next);
        // Sending message to node TC port and wait for response
        int rc = csp_transaction(1, (uint8_t)node, SCH_TRX_PORT_TC, 1000,
                                 (void *)tc_frame, (int)strlen(tc_frame), rep, 1);

        if(rc > 0 && rep[0] == 200)
        {
            LOGV(tag, "TC sent successfully. (rc: %d, re: %d)", rc, rep[0]);
            return CMD_OK;
        }
        else
        {
            LOGE(tag, "Error sending TC. (rc: %d, re: %d)", rc, rep[0]);
            return CMD_ERROR;
        }
    }

    LOGE(tag, "Error parsing parameters! (np: %d, n: %d)", n_args, next);
    return CMD_SYNTAX_ERROR;
}

int com_send_data(char *fmt, char *params, int nparams)
{
    int node, port, next;
    if(params == NULL || sscanf(params, fmt, &node, &port, &next) != nparams - 1)
    {
        LOGE(tag, "Invalid arguments!");
        return CMD_SYNTAX_ERROR;
    }
    if(next <= 0)
        return CMD_ERROR;

    char *data = params + next;
    int size = strlen(data);
    LOGI(tag, "Sending %s (%d) to node %d port %d", data, size, node, port);

    // Send the data buffer to node and wait 1 seg. for the confirmation
    int rc = csp_transaction(CSP_PRIO_NORM, node, port,1000, data, size, NULL, 0);
    return rc == 1 ? CMD_OK : CMD_ERROR;
}

int _com_send_data(int node, void *data, size_t len, int type, int n_data, int n_frame)
{
    int rc_conn = 0;
    int rc_send = 0;
    int nframe = n_frame;
    int size_data = (type ==TM_TYPE_PAYLOAD) ? (int)len/n_data : len;

    // New connection
    csp_conn_t *conn;
    conn = csp_connect(CSP_PRIO_NORM, node, SCH_TRX_PORT_TM, 500, CSP_O_NONE);
    assert(conn != NULL);

    // Send one or more frames
    while(len > 0)
    {
        // Create packet and frame
        csp_packet_t *packet = csp_buffer_get(sizeof(com_frame_t));
        packet->length = sizeof(com_frame_t);
        com_frame_t *frame = (com_frame_t *)(packet->data);
        frame->node = SCH_COMM_NODE;
        frame->nframe = csp_hton16((uint16_t)nframe++);
        frame->type = (uint8_t)type;
        size_t sent = len < COM_FRAME_MAX_LEN ? len : COM_FRAME_MAX_LEN;
        int data_sent = n_data < COM_FRAME_MAX_LEN/size_data ? n_data : (int)sent/size_data;

        frame->ndata = (type ==TM_TYPE_PAYLOAD) ? csp_hton32((uint32_t)data_sent) : csp_hton32((uint32_t)n_data);

        memcpy(frame->data.data8, data, sent);

        // Send packet
        rc_send = csp_send(conn, packet, 500);
        if(rc_send == 0)
        {
            csp_buffer_free(packet);
            LOGE(tag, "Error sending frame! (%d)", rc_send);
            break;
        }

        // Process more data
        len -= sent;
        if (type == TM_TYPE_PAYLOAD) {
            n_data -= data_sent;
        }
        data += sent;

        if(nframe%SCH_COM_MAX_PACKETS == 0)
            osDelay(SCH_COM_TX_DELAY_MS);
    }

    // Close connection
    rc_conn = csp_close(conn);
    if(rc_conn != CSP_ERR_NONE)
        LOGE(tag, "Error closing connection! (%d)", rc_conn);

    return rc_send == 1 && rc_conn == CSP_ERR_NONE ? CMD_OK : CMD_ERROR;
}

int com_send_telemetry(int node, int port, int type, void *data, size_t n_bytes, int n_structs, int n_frame)
{
    int rc_conn = 0;
    int rc_send = 0;
    int nframe = n_frame;
    int size_data = n_bytes / n_structs;
    int structs_per_frame = COM_FRAME_MAX_LEN / size_data;

    // New connection
    csp_conn_t *conn;
    conn = csp_connect(CSP_PRIO_NORM, node, port, 500, CSP_O_NONE);
    if(conn == NULL)
        return CMD_ERROR;

    // Send one or more frames
    while(n_bytes > 0)
    {
        int structs_sent = n_structs < structs_per_frame ? n_structs : structs_per_frame;
        size_t bytes_sent = structs_sent * size_data;

        // Create packet and frame
        csp_packet_t *packet = csp_buffer_get(sizeof(com_frame_t));
        packet->length = sizeof(com_frame_t);
        com_frame_t *frame = (com_frame_t *)(packet->data);
        frame->node = SCH_COMM_NODE;
        frame->nframe = csp_hton16((uint16_t)nframe++);
        frame->type = (uint8_t)type;
        frame->ndata = csp_hton32((uint32_t)structs_sent);
        memcpy(frame->data.data8, data, bytes_sent);

        // Send packet
        rc_send = csp_send(conn, packet, 500);
        if(rc_send == 0)
        {
            csp_buffer_free(packet);
            LOGE(tag, "Error sending frame! (%d)", rc_send);
            break;
        }

        // Process more data
        n_bytes -= bytes_sent;
        n_structs -= structs_sent;
        data += bytes_sent;

        if(nframe%SCH_COM_MAX_PACKETS == 0)
            osDelay(SCH_COM_TX_DELAY_MS);
    }

    // Close connection
    rc_conn = csp_close(conn);
    if(rc_conn != CSP_ERR_NONE)
        LOGE(tag, "Error closing connection! (%d)", rc_conn);

    return rc_send == 1 && rc_conn == CSP_ERR_NONE ? CMD_OK : CMD_ERROR;
}

int com_send_debug(int node, char *data, size_t len)
{
    com_send_telemetry(node, SCH_TRX_PORT_DBG_TM, 0, data, len, (int)len, 0);
}

int com_send_file(int node, char *name, void *data, size_t n_bytes)
{
    if(name == NULL || data == NULL)
        return CMD_ERROR;

    int rc_conn = 0;
    int rc_send = 0;
    int nframe = 0;
    int data_frames = n_bytes / COM_FRAME_MAX_LEN;
    int n_data_bytes = (n_bytes / COM_FRAME_MAX_LEN) * COM_FRAME_MAX_LEN;
    int n_tail_bytes = n_bytes - n_data_bytes;
    uint16_t fileid = (uint16_t)(rand() % USHRT_MAX);

    // New connection
    csp_conn_t *conn;
    conn = csp_connect(CSP_PRIO_NORM, node, SCH_TRX_PORT_FILE, 500, CSP_O_NONE);
    if(conn == NULL)
        return CMD_ERROR;

    // Send the first packet with the file name
    rc_send = com_send_file_parts(node, name, strlen(name)+1, fileid, 0, data_frames+1, TM_TYPE_FILE_START, conn);

    // Send data packets
    if(rc_send)
        rc_send = com_send_file_parts(node, data, n_data_bytes, fileid, 1, data_frames+1, TM_TYPE_FILE_DATA, conn);

    // Send final packet
    if(rc_send)
        rc_send = com_send_file_parts(node, data+n_data_bytes, n_tail_bytes, fileid, data_frames+1, data_frames+1, TM_TYPE_FILE_END, conn);

    // Close connection
    rc_conn = csp_close(conn);
    if(rc_conn != CSP_ERR_NONE)
        LOGE(tag, "Error closing connection! (%d)", rc_conn);

    return rc_send == 1 && rc_conn == CSP_ERR_NONE ? CMD_OK : CMD_ERROR;
}

int com_send_file_parts(int node, void *data, size_t n_bytes, int file_id, int start_part, int file_parts, int type, csp_conn_t *conn_prev)
{
    if(data == NULL)
        return CMD_ERROR;

    int rc_conn = 0;
    int rc_send = 0;
    int nframe = 0;
    int total_frames = (int)n_bytes % COM_FRAME_MAX_LEN ? 1 : 0;
    total_frames += (int)n_bytes / COM_FRAME_MAX_LEN;
    uint16_t fileid = file_id;

    // Existing or new connection
    csp_conn_t *conn_curr;
    if(conn_prev == NULL)
    {
        conn_curr = csp_connect(CSP_PRIO_NORM, node, SCH_TRX_PORT_FILE, 500, CSP_O_NONE);
        if(conn_curr == NULL)
            return CMD_ERROR;
    }
    else
        conn_curr = conn_prev;

    // Send the file data
    while(n_bytes > 0)
    {
        size_t bytes_sent = n_bytes >= COM_FRAME_MAX_LEN ? COM_FRAME_MAX_LEN : n_bytes;
        // Create packet and frame
        csp_packet_t *packet = csp_buffer_get(sizeof(com_frame_file_t));
        packet->length = sizeof(com_frame_file_t);
        com_frame_file_t *frame = (com_frame_file_t *)(packet->data);
        frame->node = SCH_COMM_NODE;
        frame->nframe = csp_hton16((uint16_t)start_part++);
        frame->type = type;
        frame->fileid = csp_hton16(fileid);
        frame->total = csp_hton16(file_parts);
        memcpy(frame->data, data, bytes_sent);

        // Fill frame
        if(bytes_sent < COM_FRAME_MAX_LEN) {
            LOGD(tag, "Last frame include %d bytes!", bytes_sent);
            memset(frame->data+bytes_sent, 0xAA, COM_FRAME_MAX_LEN-bytes_sent);
        }

        // Send packet
        rc_send = csp_send(conn_curr, packet, 500);
        if(rc_send == 0)
        {
            LOGE(tag, "Error sending frame! (%d)", rc_send);
            csp_buffer_free(packet);
            if(conn_prev == NULL)
                rc_conn = csp_close(conn_curr);
            return CMD_ERROR;
        }

        // Process more data
        n_bytes -= bytes_sent;
        data += bytes_sent;
        nframe ++;

        if(nframe%SCH_COM_MAX_PACKETS == 0)
            osDelay(SCH_COM_TX_DELAY_MS);
    }

    // Close connection if created
    if(conn_prev == NULL)
    {
        rc_conn = csp_close(conn_curr);
        if (rc_conn != CSP_ERR_NONE)
            LOGE(tag, "Error closing connection! (%d)", rc_conn);
    }

    return rc_send == 1 && rc_conn == CSP_ERR_NONE ? CMD_OK : CMD_ERROR;
}


void _hton32_buff(uint32_t *buff, int len)
{
    int i;
    for(i=0; i<len; i++)
        buff[i] = csp_hton32(buff[i]);
}

void _ntoh32_buff(uint32_t *buff, int len)
{
    int i;
    for(i=0; i<len; i++)
        buff[i] = csp_ntoh32(buff[i]);
}

int com_debug(char *fmt, char *params, int nparams)
{
    LOGR(tag, "CSP Node %d", csp_get_address());
    LOGR(tag, "Route table");
    csp_route_print_table();
    LOGR(tag, "Interfaces");
    csp_route_print_interfaces();
    LOGR(tag, "Connections")
    csp_conn_print_table();

    return CMD_OK;
}

int com_set_time_node(char *fmt, char *params, int nparams)
{
    int node;
    if(params == NULL || sscanf(params, fmt, &node) != nparams)
    {
        LOGE(tag, "Error parsing params!");
        return CMD_SYNTAX_ERROR;
    }

    char cmd[SCH_CMD_MAX_STR_NAME];
    sprintf(cmd, "%d obc_set_time %d", node, (int)dat_get_time());
    LOGI(tag, "Sending command 'com_send_cmd %s' to %d", cmd, node);
    com_send_cmd("%d %n", cmd, 2);
}

int com_set_tle_node(char *fmt, char *params, int nparams)
{
    char sat[50]; // TLE sat max name is 24
    int rc, node;
    memset(sat, 0, 50);
    // fmt: %s
    if(params == NULL || sscanf(params, fmt, &node, sat) != nparams)
    {
        LOGE(tag, "Error parsing params!");
        return CMD_SYNTAX_ERROR;
    }

    // Download cubesat TLE file
    rc = system("wget https://www.celestrak.com/NORAD/elements/cubesat.txt -O /tmp/cubesat.tle");
    if(rc < 0)
    {
        LOGW(tag, "Error downloading TLE file (%d)", rc);
        return CMD_ERROR;
    }

    // Search the required satellite tle
    char line[100];
    snprintf(line, 100, "cat /tmp/cubesat.tle | grep -A 2 %s > /tmp/%s.tle", sat, sat);
    LOGI(tag, "%s", line);
    rc = system(line);
    if(rc < 0)
    {
        LOGE(tag, "Error grep TLE for %s (%d)", sat, rc);
        return CMD_ERROR;
    }

    // Read the required TLE file
    memset(line, 0, 100);
    snprintf(line, 100, "/tmp/%s.tle", sat);
    LOGI(tag, "%s", line);
    FILE *file = fopen(line, "r");
    if(file == NULL)
    {
        LOGE(tag, "Error reading file %s", line);
    }

    char cmd[SCH_CMD_MAX_STR_NAME];
    // Read satellite name... skip
    memset(line, 0, 100);
    char *tle = fgets(line, 100, file);
    if(tle == NULL)
        return CMD_ERROR;
    LOGD(tag, line);

    // Read and send first TLE line
    memset(line, 0, 100);
    memset(cmd, 0, SCH_CMD_MAX_STR_NAME);

    tle = fgets(line, 100, file);
    if(tle == NULL)
        return CMD_ERROR;
    memset(line+69, 0, 100-69); // Clean the string from \r, \n others
    LOGD(tag, line);

    snprintf(cmd, SCH_CMD_MAX_STR_NAME, "%d tle_set %s", node, line);
    LOGD(tag, cmd);
    rc = com_send_cmd("%d %n", cmd, 2);
    if(rc != CMD_OK)
        return CMD_ERROR;

    // Read and send second TLE line
    memset(line, 0, 100);
    memset(cmd, 0, SCH_CMD_MAX_STR_NAME);

    tle = fgets(line, 100, file);
    if(tle == NULL)
        return CMD_ERROR;
    memset(line+69, 0, 100-69); // Clean the string from \r, \n others
    LOGD(tag, line);

    snprintf(cmd, SCH_CMD_MAX_STR_NAME, "%d tle_set %s", node, line);
    LOGD(tag, cmd);
    rc = com_send_cmd("%d %n", cmd, 2);
    if(rc != CMD_OK)
        return CMD_ERROR;

    // Send update tle command
    memset(cmd, 0, SCH_CMD_MAX_STR_NAME);
    snprintf(cmd, SCH_CMD_MAX_STR_NAME, "%d tle_update", node);
    LOGD(tag, cmd);
    rc = com_send_cmd("%d %n", cmd, 2);
    if(rc != CMD_OK)
        return CMD_ERROR;

    fclose(file);

    LOGR(tag, "TLE sent ok!")
    return CMD_OK;
}
