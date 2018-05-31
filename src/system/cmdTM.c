/**
 *
 */

#include "cmdTM.h"

static const char *tag = "cmdTM";

void cmd_tm_init(void)
{
    cmd_add("send_status", cmd_tm_send_status, "%d", 1);
}

int cmd_tm_send_status(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_ERROR;
    }

    int dest_node;
    if(nparams == sscanf(params, fmt, &dest_node))
    {
        com_data_t data;
        data.node = (uint8_t)dest_node;
        data.frame.frame = 0;
        data.frame.type = 1;
        memset(data.frame.data.data8, 0, sizeof(data.frame.data.data8));

        // The total amount of status variables must fit inside a frame
        assert((dat_system_last_var / 4) < sizeof(data.frame.data));

        LOGV(tag, "sizeof(data.frame.data) = %d", sizeof(data.frame.data));
        LOGV(tag, "sizeof(data.frame) = %d", sizeof(data.frame));
        LOGV(tag, "sizeof(data) = %d", sizeof(data));

        dat_system_t var;
        for (var = dat_obc_opmode; var < dat_system_last_var; var++)
        {
            data.frame.data.data32[var] = (uint32_t) dat_get_system_var(var);
        }

        return com_send_data(NULL, (char *)&data, 0);
    }
    else
    {
        LOGW(tag, "Invalid args!");
        return CMD_FAIL;
    }
}