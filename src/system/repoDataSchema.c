/**
 * @file  dataSchema.h
 * @author Camilo Rojas M - camrojas@uchile.cl
 * @date 2019
 * @copyright GNU GPL v3
 *
 * This file initilize some structs needed for data schema.
 */

#include "repoDataSchema.h"

struct map data_map[last_sensor] = {
        {"temp_data",      (uint16_t) (sizeof(temp_data_t)),     dat_drp_temp, dat_drp_ack_temp, "%u %f %f %f",                   "timestamp obc_temp_1 obc_temp_2 obc_temp_3"},
        { "ads_data",      (uint16_t) (sizeof(ads_data_t)),      dat_drp_ads,  dat_drp_ack_ads,  "%u %f %f %f %f %f %f",          "timestamp acc_x acc_y acc_z mag_x mag_y mag_z"},
        { "eps_data",      (uint16_t) (sizeof(eps_data_t)),      dat_drp_eps,  dat_drp_ack_eps,  "%u %u %u %u %d %d %d %d %d %d", "timestamp cursun cursys vbatt temp1 temp2 temp3 temp4 temp5 temp6"},
        { "langmuir_data", (uint16_t) (sizeof(langmuir_data_t)), dat_drp_lang, dat_drp_ack_lang, "%u %f %f %f %d",                "timestamp sweep_voltage plasma_voltage plasma_temperature particles_counter"}
};

