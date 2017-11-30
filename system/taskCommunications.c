/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2017, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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
                    break;

                case SCH_TRX_PORT_DEBUG:
                    /* Debug port, only to print strings */
                    LOGI(tag, (char *)(packet->data));
                    csp_buffer_free(packet);
                    break;

                case SCH_TRX_PORT_CONSOLE:
                    /* Debug port, executes console commands */
                    com_receive_cmd(packet);
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
 * Parse TC frames and generates corresponding commands
 *
 * @param packet TC Buffer
 */
static void com_receive_tc(csp_packet_t *packet)
{
    int next;
    int n_args;
    static cmd_t *new_cmd;
    char tmp_cmd[packet->length];
    char tmp_arg[packet->length];

    char *buffer = (char *)(packet->data);
    LOGI(tag, "New TC: %s", buffer);

    n_args = sscanf(buffer, "%s %n", tmp_cmd, &next);
    strncpy(tmp_arg, buffer+next, SCH_BUFF_MAX_LEN);
    LOGV(tag, "Parsed %d: %s, %s (%d))", n_args, tmp_cmd, tmp_arg, next);

    if (n_args == 1)
    {
        new_cmd = cmd_get_str(tmp_cmd);
        if(new_cmd)
        {
            if(next > 1)
            {
                cmd_add_params_str(new_cmd, tmp_arg);
            }
            else
            {
                cmd_add_params_str(new_cmd, "");
            }

            cmd_send(new_cmd);
        }
    }
    else
    {
        LOGE(tag, "Invalid number of arguments (%d)", n_args)
    }
}

/**
 * Parse tc frame as console commands
 * @note NOT IMPLEMENTED YET
 * @param packet TC Buffer
 */
static void com_receive_cmd(csp_packet_t *packet)
{
    /* FIXME: Not implemented */
    LOGW(tag, "com_receive_cmd NOT implemented! (%s)", (char *)(packet->data))
}