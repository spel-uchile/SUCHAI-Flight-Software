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

#include <cmdCOM.h>
#include "cmdTM.h"

static const char *tag = "cmdTM";

void cmd_tm_init(void)
{
    cmd_add("send_status", tm_send_status, "%d", 1);
    cmd_add("tm_parse_status", tm_parse_status, "", 0);
}

int tm_send_status(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_ERROR;
    }

    int dest_node;
    //Format: <node>
    if(nparams == sscanf(params, fmt, &dest_node))
    {
        com_data_t data;
        memset(&data, 0, sizeof(data));
        data.node = (uint8_t)dest_node;
        data.frame.frame = 0;
        data.frame.type = TM_TYPE_STATUS;

        // Pack status variables to a structure
        dat_status_t status;
        dat_status_to_struct(&status);
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
        dat_print_status(&status);
#endif

        // The total amount of status variables must fit inside a frame
        LOGV(tag, "sizeof(data.frame) = %lu", sizeof(data.frame));
        LOGV(tag, "sizeof(data.frame.data) = %lu", sizeof(data.frame.data));
        assert(sizeof(status) < sizeof(data.frame.data));
        memcpy(data.frame.data.data8, &status, sizeof(status));

        return com_send_data("", (char *)&data, 0);
    }
    else
    {
        LOGW(tag, "Invalid args!");
        return CMD_FAIL;
    }
}

int tm_parse_status(char *fmt, char *params, int nparams)
{
    if(params == NULL)
        return CMD_ERROR;

    dat_status_t *status = (dat_status_t *)params;
    dat_print_status(status);
    return CMD_OK;
}