/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2018, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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

void taskCommunications(void *param)
{
    LOGD(tag, "Started");
    int rc;

    /* Pointer to current connection, packet and socket */
    csp_conn_t *conn;
    csp_packet_t *packet;
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
    memset(rep_ok_tmp->data, 200, 1);
    rep_ok_tmp->length = 1;

    while(1)
    {
        /* CSP SERVER */
        /* Wait for connection, 1000 ms timeout */
        if((conn = csp_accept(sock, 1000)) == NULL)
            continue; /* Try again later */

        /* Read packets. Timeout is 500 ms */
        while ((packet = csp_read(conn, 500)) != NULL)
        {
            switch (csp_conn_dport(conn))
            {
                case SCH_TRX_PORT_TC:
                    /* Process incoming TC */
                    com_receive_tc(packet);
                    csp_buffer_free(packet);
                    // Create a response packet and send
//                    rep_ok = csp_buffer_clone(rep_ok_tmp);
                    rep_ok = csp_buffer_get(1);
                    rep_ok->data[0] = 200;
                    rep_ok->length = 1;
                    csp_send(conn, rep_ok, 1000);
                    break;

                case SCH_TRX_PORT_TM:
                    /* TODO: Process incoming TM */
                    csp_buffer_free(packet);
                    // Create a response packet and send
                    rep_ok = csp_buffer_clone(rep_ok_tmp);
                    csp_send(conn, rep_ok, 1000);

                case SCH_TRX_PORT_RPT:
                    // Digital repeater port, resend the received packet
                    if(csp_conn_dst(conn) == SCH_COMM_ADDRESS)
                    {
                        rc = csp_sendto(CSP_PRIO_NORM, CSP_BROADCAST_ADDR,
                                        SCH_TRX_PORT_RPT, SCH_TRX_PORT_RPT,
                                        CSP_O_NONE, packet, 1000);
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
                    //rep_ok = csp_buffer_clone(rep_ok_tmp);
                    rep_ok = csp_buffer_get(1);
                    rep_ok->data[0] = 200;
                    rep_ok->length = 1;
                    csp_send(conn, rep_ok, 1000);
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
 * Parse TC frames and generates corresponding commands
 *
 * @param packet A csp buffer containing a null terminated string with the
 *               format <command> [parameters]
 */
static void com_receive_tc(csp_packet_t *packet)
{
    /* TODO: this function should receive several (cmd,args) pairs */
    /* TODO: check com_receive_cmd implementation */

    // Make sure the buffer is a null terminated string
    packet->data[packet->length] = '\0';
    cmd_t *new_cmd = cmd_parse_from_str((char *)(packet->data));

    // Send command to execution if not null
    if(new_cmd != NULL)
        cmd_send(new_cmd);
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