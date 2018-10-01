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

#include "cmdCOM.h"

static const char *tag = "cmdCOM";

void cmd_com_init(void)
{
    cmd_add("ping", com_ping, "%d", 1);
    cmd_add("send_rpt", com_send_rpt, "%d %s", 2);
    cmd_add("send_cmd", com_send_cmd, "%d %n", 1);
    cmd_add("send_data", com_send_data, "%p", 1);
    cmd_add("com_debug", com_debug, "", 0);
    cmd_add("com_reset_wdt", com_reset_wdt, "%d", 1);
    cmd_add("com_get_config", com_get_config, "%s", 1);
}

int com_ping(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Null arguments!");
        return CMD_ERROR;
    }

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

int com_send_rpt(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Null arguments!");
        return CMD_ERROR;
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
            return CMD_FAIL;
        }
        packet->length = (uint16_t)(msg_len+1);
        memcpy(packet->data, msg, msg_len+1);

        // Sending message to node RPT, do not require direct answer
        int rc = csp_sendto(CSP_PRIO_NORM, (uint8_t)node, SCH_TRX_PORT_RPT,
                            SCH_TRX_PORT_RPT, CSP_O_NONE, packet, 1000);

        if(rc == 0)
        {
            LOGV(tag, "Data sent successfully. (rc: %d, re: %s)", rc, msg);
            return CMD_OK;
        }
        else
        {
            LOGE(tag, "Error sending data. (rc: %d)", rc);
            csp_buffer_free(packet);
            return CMD_FAIL;
        }
    }

    LOGE(tag, "Error parsing parameters!");
    return CMD_FAIL;
}

int com_send_cmd(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Null arguments!");
        return CMD_ERROR;
    }

    int node, next, n_args;
    char rep[0];
    char msg[SCH_CMD_MAX_STR_PARAMS];
    memset(msg, '\0', SCH_CMD_MAX_STR_PARAMS);

    //format: <node> <command> [parameters]
    n_args = sscanf(params, fmt, &node, &next);
    if(n_args == nparams && next > 1)
    {
        strncpy(msg, params+next, (size_t)SCH_CMD_MAX_STR_PARAMS);
        LOGV(tag, "Parsed %d: %d, %s (%d))", n_args, node, msg, next);

        // Sending message to node TC port and wait for response
        int rc = csp_transaction(1, (uint8_t)node, SCH_TRX_PORT_TC, 1000,
                                 (void *)msg, (int)strlen(msg), rep, 1);

        if(rc > 0 && rep[0] == 200)
        {
            LOGV(tag, "Data sent successfully. (rc: %d, re: %d)", rc, rep[0]);
            return CMD_OK;
        }
        else
        {
            LOGE(tag, "Error sending data. (rc: %d, re: %d)", rc, rep[0]);
            return CMD_FAIL;
        }
    }

    LOGE(tag, "Error parsing parameters!");
    return CMD_FAIL;
}

int com_send_data(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Null arguments!");
        return CMD_ERROR;
    }

    uint8_t rep[1];
    com_data_t *data_to_send = (com_data_t *)params;

    // Send the data buffer to node and wait 1 seg. for the confirmation
    int rc = csp_transaction(CSP_PRIO_NORM, data_to_send->node, SCH_TRX_PORT_TM,
                             1000, (uint8_t *)&data_to_send->frame,
                             sizeof(data_to_send->frame), rep, 1);

    if(rc > 0 && rep[0] == 200)
    {
        LOGV(tag, "Data sent successfully. (rc: %d, re: %d)", rc, rep[0]);
        return CMD_OK;
    }
    else
    {
        LOGE(tag, "Error sending data. (rc: %d, re: %d)", rc, rep[0]);
        return CMD_FAIL;
    }
}

int com_debug(char *fmt, char *params, int nparams)
{
    LOGD(tag, "Route table");
    csp_route_print_table();
    LOGD(tag, "Interfaces");
    csp_route_print_interfaces();

    return CMD_OK;
}

int com_reset_wdt(char *fmt, char *params, int nparams)
{

    int rc, node, n_args = 0;

    // If no params received, try to reset the default SCH_TRX_ADDRESS node
    if(params == NULL)
        node = SCH_TRX_ADDRESS;

    //format: <node>
    n_args = sscanf(params, fmt, &node);
    // If no params received, try to reset the default SCH_TRX_ADDRESS node
    if(n_args != nparams)
        node = SCH_TRX_ADDRESS;

    // Send and empty message to GNDWDT_RESET (9) port
//    rc = csp_transaction(CSP_PRIO_CRITICAL, SCH_TRX_ADDRESS, AX100_PORT_GNDWDT_RESET, 1000, NULL, 0 NULL, 0);

    if(rc > 0)
    {
        LOGV(tag, "GND Reset sent successfully. (rc: %d)", rc);
        return CMD_OK;
    }
    else
    {
        LOGE(tag, "Error sending GND Reset. (rc: %d)", rc);
        return CMD_FAIL;
    }
}

int com_get_hk(char *fmt, char *params, int nparams)
{
    //TODO: Implement
    return CMD_FAIL;
}

int com_get_config(char *fmt, char *params, int nparams)
{
    int rc, n_args;
    char param[SCH_CMD_MAX_STR_PARAMS];
    memset(param, '\0', SCH_CMD_MAX_STR_PARAMS);

    if(params == NULL)
    {
        return CMD_FAIL;
    }

    n_args = sscanf(params, fmt, &param);
    if(n_args == nparams)
    {
        int i, table = 0;
        param_table_t *param_i = NULL;

        // If param is 'help' then show the available param names
        if(strcmp(param, "help") == 0)
        {
            LOGI(tag, "List of available TRX parameters:")
            for(i=0; i<ax100_config_count; i++)
            {
                printf("\t%s\n", ax100_config[i].name);
            }
            for(i=0; i<ax100_config_tx_count; i++)
            {
                printf("\t%s\n", ax100_tx_config[i].name);
            }
            return CMD_OK;
        }

        // Find the given parameter name in the AX100 CONFIG table
        for(i=0; i<ax100_config_count; i++)
        {
            if (strcmp(param, ax100_config[i].name) == 0)
            {
                param_i = &(ax100_config[i]);
                table = AX100_PARAM_RUNNING;
                break;
            }
        }

        // Find the given parameter name in the AX100 TX table
        if(param_i == NULL) {
            for(i = 0; i < ax100_config_tx_count; i++)
            {
                if(strcmp(param, ax100_tx_config[i].name) == 0)
                {
                    param_i = &(ax100_tx_config[i]);
                    table = AX100_PARAM_TX(0);
                    break;
                }
            }
        }

        // Warning if the parameter name was not found
        if(param_i == NULL)
        {
            LOGW(tag, "Param %s not found!", param);
            return CMD_FAIL;
        }

        // Actually get the parameter value
        void *out = malloc(param_i->size);
        rc = rparam_get_single(out, param_i->addr, param_i->type, param_i->size,
                table, SCH_TRX_ADDRESS, AX100_PORT_RPARAM, 1000);

        // Process the answer
        if(rc > 0)
        {
            char param_str[SCH_CMD_MAX_STR_PARAMS];
            param_to_string(param_i, param_str, 0, out, 1, SCH_CMD_MAX_STR_PARAMS);
            LOGI(tag, "Param %s (table %d): %s", param_i->name, table, param_str);
            free(out);
            return CMD_OK;
        }
        else
        {
            LOGE(tag, "Error getting parameter %s! (rc: %d)", param, rc);
            free(out);
            return CMD_FAIL;
        }
    }
}