/**
 * @file  dataSchema.h
 * @author Camilo Rojas M - camrojas@uchile.cl
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This file initilize some structs needed for data schema.
 */

#include "repoDataSchema.h"
static const char *tag = "repoDataSchema";

dat_sys_var_t dat_get_status_var_def(dat_status_address_t address)
{
    dat_sys_var_t var = {0};
    int i;

    if(address < dat_status_last_address)
    {
        for (i = 0; i < dat_status_last_var; i++)
        {
            if (dat_status_list[i].address == address)
                return dat_status_list[i];
        }
    }

    LOGE(tag, "Status var not found! (%d)", address);
    return var;
}

dat_sys_var_t dat_get_status_var_def_name(char *name)
{
    dat_sys_var_t var;
    var.status = -1;
    int i;

    if(name != NULL)
    {
        for (i = 0; i < dat_status_last_var; i++)
        {
            if (strcmp(dat_status_list[i].name, name) == 0)
                return dat_status_list[i];
        }
    }

    LOGE(tag, "Status var not found! (%s)", name);
    return var;
}

void dat_print_system_var(dat_sys_var_t *status)
{
    assert(status != NULL);

    switch (status->type) {
        case 'u':
            printf("%3d, %-20s, %u, %d\r\n", status->address, status->name, status->value.u, status->status);
            break;
        case 'i':
            printf("%3d, %-20s, %d, %d\r\n", status->address, status->name, status->value.i, status->status);
            break;
        case 'f':
            printf("%3d, %-20s, %.6f, %d\r\n", status->address, status->name, status->value.f, status->status);
            break;
        default:
            printf("%3d, %-20s, %#X, %d\r\n", status->address, status->name, status->value.u, status->status);
    }
}

