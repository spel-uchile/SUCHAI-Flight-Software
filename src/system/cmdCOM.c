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

#include "cmdCOM.h"

static const char *tag = "cmdCOM";
static char trx_node = SCH_TRX_ADDRESS;

#ifdef SCH_USE_NANOCOM
static void _com_config_help(void);
static void _com_config_find(char *param_name, int *table, gs_param_table_instance_t **param);
#endif

void cmd_com_init(void)
{
    cmd_add("com_ping", com_ping, "%d", 1);
    cmd_add("com_send_rpt", com_send_rpt, "%d %s", 2);
    cmd_add("com_send_cmd", com_send_cmd, "%d %n", 2);
    cmd_add("com_send_tc", com_send_tc_frame, "%d %n", 2);
    cmd_add("com_send_data", com_send_data, "%p", 1);
    cmd_add("com_debug", com_debug, "", 0);
    cmd_add("com_set_node", com_set_node, "%d", 1);
    cmd_add("com_get_node", com_get_node, "", 0);
#ifdef SCH_USE_NANOCOM
    cmd_add("com_reset_wdt", com_reset_wdt, "%d", 1);
    cmd_add("com_get_config", com_get_config, "%s", 1);
    cmd_add("com_set_config", com_set_config, "%s %s", 2);
    cmd_add("com_update_status", com_update_status_vars, "", 0);
#endif
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
            LOGV(tag, "Data sent to repeater successfully. (rc: %d, re: %s)", rc, msg);
            return CMD_OK;
        }
        else
        {
            LOGE(tag, "Error sending data to repeater. (rc: %d)", rc);
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
            return CMD_FAIL;
        }
    }

    LOGE(tag, "Error parsing parameters!");
    return CMD_FAIL;
}

int com_send_tc_frame(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Null arguments!");
        return CMD_ERROR;
    }

    int node, next, n_args;
    uint8_t rep[1];
    char tc_frame[COM_FRAME_MAX_LEN];
    memset(tc_frame, '\0', COM_FRAME_MAX_LEN);

    //format: <node> <command> [parameters];...;<command> [parameters]
    n_args = sscanf(params, fmt, &node, &next);
    if(n_args == nparams-1 && next > 1)
    {
        strncpy(tc_frame, params+next, (size_t)SCH_CMD_MAX_STR_PARAMS);
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
            return CMD_FAIL;
        }
    }

    LOGE(tag, "Error parsing parameters! (np: %d, n: %d)", n_args, next);
    return CMD_FAIL;
}

int com_send_data(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Null arguments!");
        return CMD_ERROR;
    }

    uint8_t rep[1] = {0};
    com_data_t *data_to_send = (com_data_t *)params;

    // Send the data buffer to node and wait 1 seg. for the confirmation
    int rc = csp_transaction(CSP_PRIO_NORM, data_to_send->node, SCH_TRX_PORT_TM,
                             1000, &(data_to_send->frame),
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

int com_set_node(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "Null arguments!");
        return CMD_ERROR;
    }

    int node;
    if(sscanf(params, fmt, &node) == nparams)
    {
        trx_node = node;
        LOGI(tag, "TRX node set to %d", node);
        return CMD_OK;
    }
    return CMD_FAIL;
}

int com_get_node(char *fmt, char *params, int nparams)
{
    printf("TRX Node: %d\n", trx_node);
    return CMD_OK;
}

#ifdef SCH_USE_NANOCOM
int com_reset_wdt(char *fmt, char *params, int nparams)
{

    int rc, node, n_args = 0;

    // If no params received, try to reset the default trx_node node
    if(params == NULL)
        node = trx_node;
    else
    {
        //format: <node>
        n_args = sscanf(params, fmt, &node);
        // If no params received, try to reset the current trx_node
        if(n_args != nparams)
            node = trx_node;
    }

    // Send and empty message to GNDWDT_RESET (9) port
    rc = csp_transaction(CSP_PRIO_CRITICAL, node, AX100_PORT_GNDWDT_RESET, 1000, NULL, 0, NULL, 0);

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
        LOGE(tag, "Null arguments!");
        return CMD_FAIL;
    }

    // Format: <param_name>
    n_args = sscanf(params, fmt, &param);
    if(n_args == nparams)
    {
        int table = 0;
        param_table_t *param_i;

        // If param is 'help' then show the available param names
        if(strcmp(param, "help") == 0)
        {
            _com_config_help();
            return CMD_OK;
        }

        // Find the given parameter by name and get the size, index, type and
        // table; param_i is set to NULL if the parameter is not found.
        _com_config_find(param, &table, &param_i);

        // Warning if the parameter name was not found
        if(param_i == NULL)
        {
            LOGW(tag, "Param %s not found!", param);
            return CMD_FAIL;
        }

        // Actually get the parameter value
        void *out = malloc(param_i->size);
        rc = rparam_get_single(out, param_i->addr, param_i->type, param_i->size,
                table, trx_node, AX100_PORT_RPARAM, 1000);

        // Process the answer
        if(rc > 0)
        {
            char param_str[SCH_CMD_MAX_STR_PARAMS];
            param_to_string(param_i, param_str, 0, out, 1, SCH_CMD_MAX_STR_PARAMS) ;
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

int com_set_config(char *fmt, char *params, int nparams)
{
    int rc, n_args;
    char param[SCH_CMD_MAX_STR_PARAMS];
    char value[SCH_CMD_MAX_STR_PARAMS];
    memset(param, '\0', SCH_CMD_MAX_STR_PARAMS);
    memset(value, '\0', SCH_CMD_MAX_STR_PARAMS);

    if(params == NULL)
    {
        LOGE(tag, "Null arguments!");
        return CMD_FAIL;
    }

    // Format: <param_name> <value>
    n_args = sscanf(params, fmt, &param, &value);
    if(n_args == nparams)
    {
        int table = 0;
        param_table_t *param_i;

        // If param is 'help' then show the available param names
        if(strcmp(param, "help") == 0)
        {
            _com_config_help();
            return CMD_OK;
        }

        // Find the given parameter by name and get the size, index, type and
        // table; param_i is set to NULL if the parameter is not found.
        _com_config_find(param, &table, &param_i);

        // Warning if the parameter name was not found
        if(param_i == NULL)
        {
            LOGW(tag, "Param %s not found!", param);
            return CMD_FAIL;
        }

        // Actually get the parameter value
        void *out = malloc(param_i->size);
        param_from_string(param_i, value, out);
        rc = rparam_set_single(out, param_i->addr, param_i->type, param_i->size,
                               table, trx_node, AX100_PORT_RPARAM, 1000);

        // Process the answer
        if(rc > 0)
        {
            char param_str[SCH_CMD_MAX_STR_PARAMS];
            param_to_string(param_i, param_str, 0, out, 1, SCH_CMD_MAX_STR_PARAMS);
            LOGI(tag, "Param %s (table %d) set to: %s", param_i->name, table, param_str);
            free(out);
            return CMD_OK;
        }
        else
        {
            LOGE(tag, "Error setting parameter %s! (rc: %d)", param, rc);
            free(out);
            return CMD_FAIL;
        }
    }
}

int com_update_status_vars(char *fmt, char *params, int nparams)
{
    char *names[5] = {"freq", "tx_pwr", "baud", "mode", "bcn_interval"};
    dat_system_t vars[5] = {dat_com_freq, dat_com_tx_pwr, dat_com_bcn_period,
                             dat_com_mode, dat_com_bcn_period};
    int table = 0;
    param_table_t *param_i = NULL;
    int rc;

    int i = 0;
    for(i=0; i<5; i++)
    {
        // Find the given parameter by name and get the size, index, type and
        // table; param_i is set to NULL if the parameter is not found.
        _com_config_find(names[i], &table, &param_i);

        // Warning if the parameter name was not found
        assert(param_i != NULL);

        // Actually get the parameter value
        void *out = malloc(param_i->size);
        rc = rparam_get_single(out, param_i->addr, param_i->type, param_i->size,
                               table, trx_node, AX100_PORT_RPARAM, 1000);

        // Process the answer, save value to status variables
        if(rc > 0)
        {
            if(param_i->size == sizeof(int))
                dat_set_system_var(vars[i], *((int *)out));
            else if(param_i->size == sizeof(uint8_t))
                dat_set_system_var(vars[i], *((uint8_t *)out));
            else
                LOGE(tag, "Error casting status variable");

            LOGI(tag, "Param %s (table %d) %d", param_i->name, table, dat_get_system_var(vars[i]));
            free(out);
        }
    }

    return CMD_OK;
}


/* Auxiliary functions */

/**
 * Print the list of available parameters
 */
void _com_config_help(void)
{
    int i;
    LOGI(tag, "List of available TRX parameters:")
    for(i=0; i<ax100_config_count; i++)
    {
        printf("\t%s\n", ax100_config[i].name);
    }
    for(i=0; i<ax100_config_tx_count; i++)
    {
        printf("\t%s\n", ax100_tx_config[i].name);
    }
}

/**
 * Find the parameter table structure and table index by name. This function is
 * used before @rparam_get_single and @rparam_set_single to obtain the parameter
 * type, index and size. If not found, the @param is set to NULL.
 *
 * @param param_name Str. Parameter name
 * @param table int *. The parameter table index will be stored here.
 * @param param param_table_t *. The parameter type, size and index will be
 * stored here. If the parameter is not found, this pointer is set to NULL.
 */
void _com_config_find(char *param_name, int *table, gs_param_table_instance_t **param)
{
    int i = 0;
    int table_tmp = -1;
    *param = NULL;

    char *token;
    token = strtok(param_name, "-");

    // ej: tx-freq
    if(strcmp(token, "tx") == 0)
    {
        table_tmp = AX100_PARAM_TX(0);
        param_name = strtok(NULL, "-");
    }
    // ej: rx-freq
    else if(strcmp(token, "rx") == 0)
    {
        table_tmp = AX100_PARAM_RX;
        param_name = strtok(NULL, "-");
    }
    // ej: tx_pwr or baud
    else
    {
        param_name = token;
        table_tmp = -1;
    }


    // Find the given parameter name in the AX100 CONFIG table
    for(i=0; i < ax100_config_count; i++)
    {
        //printf("%d, %s\n", i, ax100_config[i].name);
        if(strcmp(param_name, ax100_config[i].name) == 0)
        {
            *param = &(ax100_config[i]);
            *table = AX100_PARAM_RUNNING;
            printf("%d, %d, %s\n", i, *table, ax100_config[i].name);
            return;
        }
    }

    // Find the given parameter name in the AX100 RX table
    if(*param == NULL)
    {
        for(i = 0; i < ax100_config_rx_count; i++)
        {
            // printf("(rx) %d, %s\n", i, ax100_rx_config[i].name);
            if(strcmp(param_name, ax100_rx_config[i].name) == 0)
            {
                *param = &(ax100_rx_config[i]);
                *table = table_tmp != -1 ? table_tmp : AX100_PARAM_RX;
                printf("%d, %d, %s\n", i, *table, ax100_rx_config[i].name);
                return;
            }
        }
    }

    // Find the given parameter name in the AX100 TX table
    if(*param == NULL)
    {
        for(i = 0; i < ax100_config_tx_count; i++)
        {
            // printf("(tx) %d, %s\n", i, ax100_tx_config[i].name);
            if(strcmp(param_name, ax100_tx_config[i].name) == 0)
            {
                *param = &(ax100_tx_config[i]);
                *table = table_tmp != -1 ? table_tmp : AX100_PARAM_TX(0);
                printf("%d, %d, %s\n", i, *table, ax100_rx_config[i].name);
                return;
            }
        }
    }
}
#endif //SCH_USE_NANOCOM