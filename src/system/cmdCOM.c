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

#include "cmdCOM.h"

static const char *tag = "cmdCOM";

void cmd_com_init(void)
{
    cmd_add("ping", com_ping, "%d", 1);
    cmd_add("send", com_send_dbg, "%d %s", 2);
    cmd_add("send_cmd", com_send_cmd, "%d %s", 2);
}

int com_ping(char *fmt, char *params, int nparams)
{
    int node;
    if(sscanf(params, fmt, &node) == nparams)
    {
        int rc = csp_ping((uint8_t)node, 3000, 10, CSP_O_NONE);
        LOGI(tag, "Ping to %d took %d", node, rc);
        if(rc > 0)
            return CMD_OK;
    }
    return CMD_FAIL;
}

int com_send_dbg(char *fmt, char *params, int nparams)
{
    int node;
    char msg[CMD_MAX_STR_PARAMS];

    if(sscanf(params, fmt, &node, msg) == nparams)
    {
        /* Sending message to node debug port */
        int rc = csp_transaction(1, (uint8_t)node, SCH_TRX_PORT_DEBUG, 1000,
                                 (void *)msg, strlen(msg),
                                 (void *)msg, strlen(msg));
        LOGV(tag, "Sending %s to %d. Bytes read %d\n", msg, node, rc);

        if(rc >= 0)
            return CMD_OK;
    }
    return CMD_FAIL;
}

int com_send_cmd(char *fmt, char *params, int nparams)
{
    int node;
    char msg[CMD_MAX_STR_PARAMS];

    if(sscanf(params, fmt, &node, msg) == nparams)
    {
        /* Sending message to node debug port */
        int rc = csp_transaction(1, (uint8_t)node, SCH_TRX_PORT_TC, 1000,
                                 (void *)msg, strlen(msg),
                                 (void *)msg, strlen(msg));
        LOGV(tag, "Sending %s to %d. Bytes read %d\n", msg, node, rc);

        if(rc >= 0)
            return CMD_OK;
    }
    return CMD_FAIL;
}