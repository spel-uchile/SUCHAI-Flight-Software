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
        {"sta_data",    (uint16_t) (sizeof(sta_data_t)), dat_drp_sta, dat_drp_ack_sta, "%u %u %u %u %u %u %f %f %f %u %u %u %u %u %u %u %u %u %u %f %f %f %f %f %f %f %f %f %u %u %f %f %f %f %u %u %u %u %u %u %u %u, %u %u %u", "timestamp obc_last_reset obc_hrs_alive obc_hrs_wo_reset obc_reset_counter obc_sw_wdt obc_temp_1 obc_temp_2 obc_temp_3 obc_executed_cmds obc_failed_cmds dep_deployed dep_ant_deployed dep_date_time com_count_tm com_count_tc com_last_tc fpl_last fpl_queue ads_omega_x ads_omega_y ads_omega_z ads_mag_x ads_mag_y ads_mag_z ads_pos_x ads_pos_y ads_pos_z ads_tle_epoch ads_tle_last ads_q0 ads_q1 ads_q2 ads_q3 eps_vbatt eps_cur_sun eps_cur_sys eps_temp_bat0 drp_temp drp_ads drp_eps drp_sta drp_mach_action drp_mach_state drp_mach_left obc_opmode rtc_date_time com_freq com_tx_pwr com_baud com_mode com_bcn_period obc_bcn_offset tgt_omega_x tgt_omega_y tgt_omega_z tgt_q0 tgt_q1 tgt_q2 tgt_q3 drp_ack_temp drp_ack_ads drp_ack_eps drp_ack_sta drp_mach_step drp_mach_payloads" }
};

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

