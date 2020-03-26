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

#include "taskCommunications.h"

static const char *tag = "Communications";

static void com_receive_tc(csp_packet_t *packet);
static void com_receive_cmd(csp_packet_t *packet);
static void com_receive_tm(csp_packet_t *packet);

void taskCommunications(void *param)
{
    LOGI(tag, "Started");
    int rc;

    /* Pointer to current connection, packet and socket */
    csp_conn_t *conn;
    csp_packet_t *packet;
    csp_packet_t *tmp_packet;
    csp_packet_t *rep_ok_tmp;
    csp_packet_t *rep_ok;

    csp_socket_t *sock = csp_socket(CSP_SO_NONE);
    if((rc = csp_bind(sock, CSP_ANY)) != CSP_ERR_NONE)
    {
        LOGE(tag, "Error biding socket (%d)!", rc)
        return;
    }
    if((rc = csp_listen(sock, 5)) != CSP_ERR_NONE)
    {
        LOGE(tag, "Error listening to socket (%d)", rc)
        return;
    }

    rep_ok_tmp = csp_buffer_get(1);
    rep_ok_tmp->data[0] = 200;
    rep_ok_tmp->length = 1;

    int count_tc;

    while(1)
    {
        /* CSP SERVER */
        /* Wait for connection, 1000 ms timeout */
        if((conn = csp_accept(sock, 1000)) == NULL)
            continue; /* Try again later */

        /* Read packets. Timeout is 500 ms */
        while ((packet = csp_read(conn, 500)) != NULL)
        {
            count_tc = dat_get_system_var(dat_com_count_tc) + 1;
            dat_set_system_var(dat_com_count_tc, count_tc);
            dat_set_system_var(dat_com_last_tc, (int) time(NULL));

            switch (csp_conn_dport(conn))
            {
                case SCH_TRX_PORT_TC:
                    /* Process incoming TC */
                    com_receive_tc(packet);
                    csp_buffer_free(packet);
                    // Create a response packet and send
                    rep_ok = csp_buffer_clone(rep_ok_tmp);
                    csp_send(conn, rep_ok, 1000);
                    break;

                case SCH_TRX_PORT_TM:
                    #ifdef SCH_RESEND_TM_NODE
                    tmp_packet = (csp_packet_t *)csp_buffer_clone(packet);
                    #endif
                    // Process TM packet
                    com_receive_tm(packet);
                    csp_buffer_free(packet);
                    // Create a response packet and send
                    rep_ok = csp_buffer_clone(rep_ok_tmp);
                    csp_send(conn, rep_ok, 1000);
                    #ifdef SCH_RESEND_TM_NODE
                    // Resend a copy of the packet to another node
                    assert(tmp_packet != NULL);
                    assert(tmp_packet != packet);
                    rc = csp_sendto(CSP_PRIO_NORM, SCH_RESEND_TM_NODE, SCH_TRX_PORT_TM, csp_conn_sport(conn), CSP_O_NONE, tmp_packet, 1000);
                    if(rc == -1)
                        csp_buffer_free(tmp_packet);
                    #endif
                    break;

                case SCH_TRX_PORT_RPT:
                    // Digital repeater port, resend the received packet
                    if(csp_conn_dst(conn) == SCH_COMM_ADDRESS)
                    {
                        rc = csp_sendto(CSP_PRIO_NORM, CSP_BROADCAST_ADDR,
                                        SCH_TRX_PORT_RPT, SCH_TRX_PORT_RPT,
                                        CSP_O_NONE, packet, 1000);
                        LOGD(tag, "Repeating message to %d (rc: %d)", CSP_BROADCAST_ADDR, rc);
                        if (rc != 0)
                            csp_buffer_free(packet); // Free the packet in case of errors
                    }
                    // If i am receiving a broadcast packet just print
                    else
                    {
                        LOGI(tag, "RPT: %s", (char *)(packet->data));
                        csp_buffer_free(packet);
                    }
                    break;

                case SCH_TRX_PORT_CMD:
                    /* Command port, executes console commands */
                    com_receive_cmd(packet);
                    csp_buffer_free(packet);
                    // Create a response packet and send
                    rep_ok = csp_buffer_clone(rep_ok_tmp);
                    csp_send(conn, rep_ok, 1000);
                    break;

                case SCH_TRX_PORT_DBG:
                    /* Debug port, print to console */
                    LOGR(tag, "(%d)%s", packet->id.src, (char *)(packet->data));
                    csp_buffer_free(packet);
                    break;

                default:
                    /* Let the service handler reply pings, buffer use, etc. */
                    csp_service_handler(conn, packet);
                    break;
            }
        }

        /* Close current connection, and handle next */
        csp_close(conn);
    }
}

/**
 * Parse TC frames and generates corresponding commands. A TC frame contains
 * a list of <command> [parameter] pairs separated by ";" (semicolon). For
 * example this is a valid TC frame:
 *
 *      "help;send_cmd 10 help;ping 1;print_vars"
 *
 * @param packet A csp buffer containing a null terminated string with the
 *               format <command> [parameters];<command> [parameters];...
 */
static void com_receive_tc(csp_packet_t *packet)
{
    // Make sure the buffer is a null terminated string
    packet->data[packet->length] = '\0';

    // Search for the first ";" separated command
    char *cmd_str;
    cmd_str = strtok((char *)(packet->data), ";");

    while(cmd_str != NULL)
    {
        // Parse and send command for execution
        LOGI(tag, "TC: %s", cmd_str);
        cmd_t *new_cmd = cmd_parse_from_str(cmd_str);
        if (new_cmd != NULL)
            cmd_send(new_cmd);

        // Search for the next ";" separated command
        cmd_str = strtok(NULL, ";");
    }
}

/**
 * Parse tc frame as console commands and execute the commands
 *
 * @param packet A csp buffer containing a null terminated string with the
 *               format <command> [parameters]
 */
static void com_receive_cmd(csp_packet_t *packet)
{
    // Make sure the buffer is a null terminated string
    packet->data[packet->length] = '\0';
    cmd_t *new_cmd = cmd_parse_from_str((char *)(packet->data));

    // Send command to execution if not null
    if(new_cmd != NULL)
        cmd_send(new_cmd);
}

/**
 * Process a TM frame, determine TM type and call corresponding parsing command
 * @param packet a csp buffer containing a com_frame_t structure.
 */
static void com_receive_tm(csp_packet_t *packet)
{
    cmd_t *cmd_parse_tm;
    com_frame_t *frame = (com_frame_t *)packet->data;

#ifdef LINUX
    frame->nframe = csp_ntoh16(frame->nframe);
    frame->type = csp_ntoh16(frame->type);
    frame->ndata = csp_ntoh32(frame->ndata);
    int i = 0;
    for(i=0; i < sizeof(frame->data)/sizeof(uint32_t); i++)
        frame->data.data32[i] = csp_ntoh32(frame->data.data32[i]);
#endif

    LOGI(tag, "Received: %d bytes", packet->length);
    LOGI(tag, "Frame   : %d", frame->nframe);
    LOGI(tag, "Type    : %d", (frame->type));
    LOGI(tag, "Samples : %d", (frame->ndata));

    if(frame->type == TM_TYPE_STATUS)
    {
        cmd_parse_tm = cmd_get_str("tm_parse_status");
        cmd_add_params_raw(cmd_parse_tm, frame->data.data8, sizeof(frame->data));
        cmd_send(cmd_parse_tm);
    }
    else if(frame->type >= TM_TYPE_PAYLOAD && frame->type < TM_TYPE_PAYLOAD+last_sensor)
    {
        int payload = frame->type - TM_TYPE_PAYLOAD; // Payload type
        print_buff16(packet->data16, packet->length/2);
        int j, delay = 0;

        //FIXME: Use a command to add payloads to database
        //Save ndata payload samples to data storage
        assert(frame->ndata*data_map[payload].size <= COM_FRAME_MAX_LEN);
        for(j=0; j < frame->ndata; j++)
        {
            delay = j*data_map[payload].size; // Select next struct
            dat_add_payload_sample((frame->data.data8)+delay, payload); //Save next struct
        }
    }
    else
    {
        LOGW(tag, "Undefined telemetry type %d!", frame->type);
        print_buff(packet->data, packet->length);
        print_buff16(packet->data16, packet->length/2);
    }
}
