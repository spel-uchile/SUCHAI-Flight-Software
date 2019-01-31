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
    cmd_add("send_payload", tm_send_pay_data, "%u %d", 2);
}

/**
 * Writes c as a string, into result, in binary notation.
 *
 * @param result String where c will be written in binary
 * @param c Char to transform into binary
 */
static void bin_transform(char* result, unsigned char c)
{
    unsigned char bit_mask = (unsigned char)1;
    unsigned char number = c;

    int length = sizeof(char)*8;

    bit_mask <<= (length-1);

    for (int i = 0; i < length; i++)
    {
        result[i] = ((number & bit_mask) == bit_mask) ? (char)'1' : (char)'0';
        bit_mask >>= 1;
    }

    result[length] = '\0';
}

/**
 * Writes c as a string, into result, in hex notation.
 *
 * @param result String where c will be written in hex
 * @param c Char to transform into hex
 */
static void hex_transform(char* result, unsigned char c)
{
    sprintf(result, "%02X", c);
    result[2] = '\0';
}

/**
 * Prints a number in either binary of hex notation.
 *
 * @param name Name of the object to print
 * @param num De-referenced pointer to the number
 * @param byte_size Byte size of the number
 * @param mode Either 'b' for binary or 'h' for hex
 */
static void print_num(char* name, void* num, int byte_size, char mode)
{
    unsigned char* buffer = (unsigned char*)num;
    char chunk;

    if (mode == 'b')
        chunk = 8;
    else if (mode == 'h')
        chunk = 2;
    else return;

    int sizeof_num = byte_size*chunk;
    int sizeof_char = sizeof(char)*chunk;

    int buffer_parts = sizeof_num/sizeof_char;
    char result[sizeof_num+1];
    char* pointer = result;

    if (mode == 'b')
        for (int part = 0; part < buffer_parts; part++)
            bin_transform(pointer + (part*sizeof_char), buffer[buffer_parts-part-1]);
    else
        for (int part = 0; part < buffer_parts; part++)
            hex_transform(pointer + (part*sizeof_char), buffer[buffer_parts-part-1]);

    printf("%s: \t%s\n", name, result);
}

/**
 * Prints i in binary or hex notation.
 *
 * @param i Int number to print
 */
static void print_i(char* name, int i, char mode)
{
    print_num(name, (void*)&i, sizeof(int), mode);
}
/**
 * Prints f in binary or hex notation.
 *
 * @param f Float number to print
 */
static void print_f(char* name, float f, char mode)
{
    print_num(name, (void*)&f, sizeof(f), mode);
}

/**
 * Prints all variables in a system status struct in binary or hex notation.
 *
 * @param status System status variables collection in a struct
 */
static void print_status(dat_status_t* status, char mode)
{
    printf("\n");

    print_i("\tdat_obc_opmode", status->dat_obc_opmode, mode);
    print_i("\tdat_obc_last_reset", status->dat_obc_last_reset, mode);
    print_i("\tdat_obc_hrs_alive", status->dat_obc_hrs_alive, mode);
    print_i("\tdat_obc_hrs_wo_reset", status->dat_obc_hrs_wo_reset, mode);
    print_i("\tdat_obc_reset_counter", status->dat_obc_reset_counter, mode);
    print_i("\tdat_obc_sw_wdt", status->dat_obc_sw_wdt, mode);

    print_f("\tdat_obc_temp_1", status->dat_obc_temp_1, mode);
    print_f("\tdat_obc_temp_2", status->dat_obc_temp_2, mode);
    print_f("\tdat_obc_temp_3", status->dat_obc_temp_3, mode);

    print_i("\tdat_dep_ant_deployed", status->dat_dep_ant_deployed, mode);
    print_i("\tdat_dep_date_time", status->dat_dep_date_time, mode);
    print_i("\tdat_rtc_date_time", status->dat_rtc_date_time, mode);
    print_i("\tdat_com_count_tm", status->dat_com_count_tm, mode);
    print_i("\tdat_com_count_tc", status->dat_com_count_tc, mode);
    print_i("\tdat_com_last_tc", status->dat_com_last_tc, mode);
    print_i("\tdat_com_freq", status->dat_com_freq, mode);
    print_i("\tdat_com_tx_pwr", status->dat_com_tx_pwr, mode);
    print_i("\tdat_com_baud", status->dat_com_baud, mode);
    print_i("\tdat_com_mode", status->dat_com_mode, mode);
    print_i("\tdat_com_bcn_period", status->dat_com_bcn_period, mode);
    print_i("\tdat_fpl_last", status->dat_fpl_last, mode);
    print_i("\tdat_fpl_queue", status->dat_fpl_queue, mode);

    print_f("\tdat_ads_acc_x", status->dat_ads_acc_x, mode);
    print_f("\tdat_ads_acc_y", status->dat_ads_acc_y, mode);
    print_f("\tdat_ads_acc_z", status->dat_ads_acc_z, mode);
    print_f("\tdat_ads_mag_x", status->dat_ads_mag_x, mode);
    print_f("\tdat_ads_mag_y", status->dat_ads_mag_y, mode);
    print_f("\tdat_ads_mag_z", status->dat_ads_mag_z, mode);

    print_i("\tdat_eps_vbatt", status->dat_eps_vbatt, mode);
    print_i("\tdat_eps_cur_sun", status->dat_eps_cur_sun, mode);
    print_i("\tdat_eps_cur_sys", status->dat_eps_cur_sys, mode);
    print_i("\tdat_eps_temp_bat0", status->dat_eps_temp_bat0, mode);
    print_i("\tdat_mem_temp", status->dat_mem_temp, mode);
    print_i("\tdat_mem_ads", status->dat_mem_ads, mode);

    printf("\n");
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
        if(LOG_LEVEL >= LOG_LVL_DEBUG) {
            LOGD(tag, "Sending system status to node %d", dest_node)
            dat_print_status(&status);

            print_status(&status, 'h');

        }

        // The total amount of status variables must fit inside a frame
        LOGD(tag, "sizeof(status) = %lu", sizeof(status));
        LOGD(tag, "sizeof(data.frame) = %lu", sizeof(data.frame));
        LOGD(tag, "sizeof(data.frame.data) = %lu", sizeof(data.frame.data));
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

    print_status(status, 'h');

    return CMD_OK;
}

int tm_send_pay_data(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_ERROR;
    }

    int dest_node;
    uint32_t payload;
    //Format: <node>
    if(nparams == sscanf(params, fmt, &payload, &dest_node))
    {
        com_data_t data;
        memset(&data, 0, sizeof(data));
        data.node = (uint8_t)dest_node;
        data.frame.frame = 0;
        data.frame.type = (uint16_t)(TM_TYPE_PAYLOAD + payload);

        int n_structs = COM_FRAME_MAX_LEN / data_map[payload].size;
        int index_pay = dat_get_system_var(data_map[payload].sys_index);

        if(index_pay < n_structs) {
            n_structs = index_pay;
        }

        uint8_t n_struct_8 = (uint8_t) n_structs;
        memcpy(data.frame.data.data8, &n_struct_8, sizeof(uint8_t));
        char buff[n_structs*data_map[payload].size];
        // TODO: Change to add n_struct payload data instead of 0
        dat_get_recent_payload_sample(buff, payload, 0);
        memcpy(data.frame.data.data8+1, buff, data_map[payload].size);
        return com_send_data("", (char *)&data, 0);
    }
    else
    {
        LOGW(tag, "Invalid args!");
        return CMD_FAIL;
    }
}
