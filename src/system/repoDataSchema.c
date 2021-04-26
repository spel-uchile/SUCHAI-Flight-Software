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

struct map data_map[last_sensor] = {
        {"temp_data",      (uint16_t) (sizeof(temp_data_t)),     dat_drp_temp, dat_drp_ack_temp, "%u %f %f %f",                   "timestamp obc_temp_1 obc_temp_2 obc_temp_3"},
        { "ads_data",      (uint16_t) (sizeof(ads_data_t)),      dat_drp_ads,  dat_drp_ack_ads,  "%u %f %f %f %f %f %f",          "timestamp acc_x acc_y acc_z mag_x mag_y mag_z"},
        { "eps_data",      (uint16_t) (sizeof(eps_data_t)),      dat_drp_eps,  dat_drp_ack_eps,  "%u %u %u %u %d %d %d %d %d %d", "timestamp cursun cursys vbatt temp1 temp2 temp3 temp4 temp5 temp6"},
        { "langmuir_data", (uint16_t) (sizeof(langmuir_data_t)), dat_drp_lang, dat_drp_ack_lang, "%u %f %f %f %d",                "timestamp sweep_voltage plasma_voltage plasma_temperature particles_counter"}
};


void dat_status_to_list(value32_t *varlist, dat_sys_var_t *status, int nvars)
{
    assert(varlist !=  NULL);
    assert(status != NULL);
    int i;
    for(i = 0; i<nvars; i++)
        varlist[i] = status[i].value;
}

void dat_status_from_list(value32_t *varlist, dat_sys_var_t *status, dat_sys_var_t *reference, int nvars)
{
    assert(varlist !=  NULL);
    assert(status != NULL);
    int i;
    for(i = 0; i<nvars; i++)
    {
        status[i].value = varlist[i];
        if(reference != NULL)
        {
            strcpy(status[i].name, reference[i].name);
            status[i].type = reference[i].type;
        }
    }
}

void dat_print_status(dat_sys_var_t *status, int nvars)
{
    assert(status != NULL);
    int i;
    printf("address, name, value, type\n");
    for(i=0; i<nvars; i++)
    {
        dat_sys_var_t var = status[i];
        switch (var.type) {
            case 'u':
                printf("%d, %s, %u, %d\n", var.address, var.name, var.value.u, var.is_status);
                break;
            case 'i':
                printf("%d, %s, %d, %d\n", var.address, var.name, var.value.i, var.is_status);
                break;
            case 'f':
                printf("%d, %s, %.6f, %d\n", var.address, var.name, var.value.f, var.is_status);
                break;
            default:
                printf("%d, %s, %#X, %d\n", var.address, var.name, var.value.u, var.is_status);
        }
    }
}

