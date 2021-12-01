/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2021, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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

#include "suchai/cmdTM.h"
#if defined(LINUX) || defined(SIM)
#include <sys/stat.h>
#include <dirent.h>
#endif

static const char *tag = "cmdTM";
static int _merging_file_id = 0;

void cmd_tm_init(void)
{
    cmd_add("tm_parse_status", tm_parse_status, "", 0);
    cmd_add("tm_parse_string", tm_parse_string, "", 0);
    cmd_add("tm_send_status", tm_send_status, "%d", 1);
    cmd_add("tm_send_var", tm_send_var, "%d %s", 2);
    cmd_add("tm_get_last", tm_get_last, "%u", 1);
    cmd_add("tm_get_single", tm_get_single, "%u %u", 2);
    cmd_add("tm_send_last", tm_send_last, "%u %u", 2);
    cmd_add("tm_send_all", tm_send_all, "%u %u", 2);
    cmd_add("tm_send_n", tm_send_from, "%u %u %u", 3);
    cmd_add("tm_parse_payload", tm_parse_payload, "%", 0);
    cmd_add("tm_set_ack", tm_set_ack, "%u %u", 2);
    cmd_add("tm_send_cmds", tm_send_cmds, "%d", 1);
    cmd_add("tm_send_fp", tm_send_fp, "%d", 1);
    cmd_add("tm_print_fp", tm_print_fp, "", 0);
#if defined(LINUX) || defined(SIM)
    cmd_add("tm_dump", tm_dump, "%d %s", 2);
    cmd_add("tm_send_file", tm_send_file, "%s %d", 2);
    cmd_add("tm_parse_file", tm_parse_file, "", 0);
    cmd_add("tm_send_file_part", tm_send_file_parts, "%s %d %d %d %d", 5);
    cmd_add("tm_merge_file", tm_merge_file, "%s %d", 2);
    cmd_add("tm_ls", tm_list_files, "%s %d", 2);
#endif
}

int tm_send_status(char *fmt, char *params, int nparams)
{
    //Format: <node>
    int dest_node;
    if(params == NULL || sscanf(params, fmt, &dest_node) != nparams)
    {
        return CMD_SYNTAX_ERROR;
    }

    // Pack status variables to a structure
    int i;
    dat_sys_var_short_t status_buff[dat_status_last_var];
    for(i = 0; i<dat_status_last_var; i++)
    {
        status_buff[i].address = csp_hton16(dat_status_list[i].address);
        status_buff[i].value.u = csp_hton32(dat_get_status_var(dat_status_list[i].address).u);
    }

    // Send telemetry
    return _com_send_data(dest_node, status_buff, sizeof(status_buff), TM_TYPE_STATUS, dat_status_last_var, 0);
}

int tm_send_var(char *fmt, char *params, int nparams)
{
    //Format: <node>
    int dest_node;
    char var_name[SCH_CMD_MAX_STR_PARAMS];

    if(params == NULL || sscanf(params, fmt, &dest_node, var_name) != nparams)
    {
        return CMD_SYNTAX_ERROR;
    }

    // Pack status variable to a structure
    dat_sys_var_short_t status_buff[1];
    dat_sys_var_t system_var = dat_get_status_var_def_name(var_name);
    uint16_t address = system_var.address;
    status_buff[0].address = csp_hton16(address);
    status_buff[0].value.u = csp_hton32(dat_get_status_var(address).u);

    // Send telemetry
    return _com_send_data(dest_node, status_buff, sizeof(status_buff), TM_TYPE_GENERIC, 1, 0);
}

int tm_parse_status(char *fmt, char *params, int nparams)
{
    if(params == NULL)
        return CMD_SYNTAX_ERROR;

    com_frame_t *frame = (com_frame_t *)params;
    dat_sys_var_short_t *status_buff = (dat_sys_var_short_t *)frame->data.data8;

    // Sanity check to params. Detect if params do not come from tm_send_status.
    // Avoid using this command from command line, or tele-command
    if(frame->type != TM_TYPE_STATUS || frame->ndata > sizeof(frame->data)/sizeof(dat_sys_var_short_t))
        return CMD_SYNTAX_ERROR;

    int i;
    for(i = 0; i<frame->ndata; i++)
    {
        uint16_t address = csp_ntoh16(status_buff[i].address);
        value32_t value = {.u = csp_ntoh32(status_buff[i].value.u)};
        dat_sys_var_t system_var = dat_get_status_var_def(address);
        system_var.value = value;
        dat_print_system_var(&system_var);
    }

    return CMD_OK;
}

int tm_parse_string(char *fmt, char *params, int nparams)
{
    if(params == NULL)
        return CMD_SYNTAX_ERROR;

    com_frame_t *frame = (com_frame_t *)params;
    char *cmds_list;
    cmds_list = (char *)malloc(strlen((char *)frame->data.data8));
    memset(cmds_list, '\0', strlen((char *)frame->data.data8 + 1));
    cmds_list = (char *)frame->data.data8;
    //char *cmds_list = (char *)params;
    printf("Available commands list: %s", cmds_list);

    return CMD_OK;
}

int tm_send_tel_from_to(int start, int end, int payload, int dest_node)
{
    int rc_send = 0;
    int structs_per_frame = (COM_FRAME_MAX_LEN) / data_map[payload].size;
    uint16_t payload_size = data_map[payload].size;

    int n_samples = end - start;
    int n_frames = (n_samples)/structs_per_frame;
    if( (n_samples) % structs_per_frame != 0) {
        n_frames += 1;
    }

    int index_pay = dat_get_system_var(data_map[payload].sys_index);
//    int index_ack = dat_get_system_var(data_map[payload].sys_ack);
//    int delay = index_pay - index_ack;

    // New connection
    csp_conn_t *conn;
    conn = csp_connect(CSP_PRIO_NORM, dest_node, SCH_TRX_PORT_APP, 500, CSP_O_NONE);
    if(conn == NULL)
    {
        LOGE(tag, "Cannot create connection!");
        return CMD_ERROR;
    }

    int i;
    for(i=0; i < n_frames; ++i) {

        csp_packet_t *packet = csp_buffer_get(sizeof(com_frame_t));
        if(packet == NULL)
        {
            LOGE(tag, "Error getting CSP buffer!");
            rc_send = 0;
            break;
        }
        packet->length = sizeof(com_frame_t);
        memset(packet->data, 0, sizeof(com_frame_t));
        com_frame_t *frame = (com_frame_t *)(packet->data);
        frame->node = SCH_COMM_NODE;
        frame->nframe = csp_hton16((uint16_t) i);
        frame->type = (uint8_t)(TM_TYPE_PAYLOAD + payload);
        frame->ndata = csp_hton32((uint32_t)structs_per_frame);

        int j;
        for(j=0; j < structs_per_frame; ++j) {
            if( (i*structs_per_frame) +j >= n_samples ) {
                frame->ndata = csp_hton32((uint32_t) j);
                break;
            }

            char buff[payload_size];
            dat_get_recent_payload_sample(buff, payload, (n_samples+(index_pay - end)) - (i * structs_per_frame + j + 1));
            int mem_offset = (j * payload_size);
            memcpy(frame->data.data8 + mem_offset, buff, payload_size);
        }

        for(int k=0; k<sizeof(frame->data)/sizeof(int32_t); k++)
            frame->data.data32[k] = csp_hton32(frame->data.data32[k]);

        LOGI(tag, "Node    : %d", frame->node);
        LOGI(tag, "Frame   : %d", csp_ntoh16(frame->nframe));
        LOGI(tag, "Type    : %d", frame->type);
        LOGI(tag, "Samples : %d", csp_ntoh16(frame->ndata));
        //print_buff(frame->data.data8, payload_size*structs_per_frame);

        // Send packet
        rc_send = csp_send(conn, packet, 500);
        if(rc_send == 0)
        {
            csp_buffer_free(packet);
            LOGE(tag, "Error sending frame %d! (%d)", i, rc_send);
            break; // Exit with error
        }

        if((i+1)%SCH_COM_MAX_PACKETS == 0)
            osDelay(SCH_COM_TX_DELAY_MS);
    }

    // Close connection
    int rc_conn = csp_close(conn);
    if(rc_conn != CSP_ERR_NONE) {
        LOGE(tag, "Error closing connection! (%d)", rc_conn);
        return CMD_ERROR;
    }
    else if(rc_send == 0)
        return CMD_ERROR;
    else
        return CMD_OK;
}

int tm_get_single(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_SYNTAX_ERROR;
    }

    uint32_t payload;
    uint32_t index;

    if(nparams == sscanf(params, fmt, &payload, &index))
    {
        if(payload >= last_sensor) {
            return CMD_SYNTAX_ERROR;
        }

        int payload_size = data_map[payload].size;
        char buff[payload_size];
        int ret;
        ret = dat_get_payload_sample(buff, payload, index);
        if( ret == -1) {
            return CMD_ERROR;
        }
        dat_print_payload_struct(buff, payload);
        return CMD_OK;
    }

    return CMD_SYNTAX_ERROR;
}

int tm_get_last(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_SYNTAX_ERROR;
    }

    uint32_t payload;
    if(nparams == sscanf(params, fmt, &payload))
    {
        if(payload >= last_sensor) {
            return CMD_SYNTAX_ERROR;
        }

        int index_pay = dat_get_system_var(data_map[payload].sys_index);

        if(index_pay < 1){
            return CMD_ERROR;
        }
        int payload_size = data_map[payload].size;
        char buff[payload_size];
        int ret;
        ret = dat_get_recent_payload_sample(buff, payload,0);
        dat_print_payload_struct(buff, payload);

        if( ret == -1) {
            return CMD_ERROR;
        }

        return CMD_OK;
    }
    else
    {
        LOGW(tag, "Invalid args!");
        return CMD_SYNTAX_ERROR;
    }
}

int tm_send_last(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_SYNTAX_ERROR;
    }

    uint32_t dest_node;
    uint32_t payload;
    if(nparams == sscanf(params, fmt, &payload, &dest_node))
    {
        if(payload >= last_sensor) {
            return CMD_SYNTAX_ERROR;
        }
        int index_pay = dat_get_system_var(data_map[payload].sys_index);

        if(index_pay < 1){
            return CMD_ERROR;
        }

        int structs_per_frame = (COM_FRAME_MAX_LEN) / data_map[payload].size;
        if( (structs_per_frame)  > index_pay ) {
            structs_per_frame = index_pay;
        }

        int rc = tm_send_tel_from_to(index_pay - structs_per_frame, index_pay, payload, dest_node);
        return rc;
    }
    else
    {
        LOGW(tag, "Invalid args!");
        return CMD_SYNTAX_ERROR;
    }
}

int tm_send_all(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_SYNTAX_ERROR;
    }

    uint32_t dest_node;
    uint32_t payload;

    if(nparams == sscanf(params, fmt, &payload, &dest_node)) {

        if(payload >= last_sensor) {
            LOGE(tag, "incorrect payload");
            return CMD_SYNTAX_ERROR;
        }
        int index_pay = dat_get_system_var(data_map[payload].sys_index);
        int index_ack = dat_get_system_var(data_map[payload].sys_ack);
        int rc = tm_send_tel_from_to(index_ack, index_pay, payload, dest_node);
        return rc;
    }
    else
    {
        return CMD_SYNTAX_ERROR;
    }
}

int tm_send_from(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_SYNTAX_ERROR;
    }

    uint32_t dest_node;
    uint32_t payload;
    uint32_t samples;

    if(nparams == sscanf(params, fmt, &payload, &dest_node, &samples)) {

        if(payload >= last_sensor) {
            return CMD_SYNTAX_ERROR;
        }

        if(samples < 1) {
            return CMD_SYNTAX_ERROR;
        }

        int index_pay = dat_get_system_var(data_map[payload].sys_index);
        int index_ack = dat_get_system_var(data_map[payload].sys_ack);

        int des = index_ack + samples;
        if(des > index_pay) {
            des = index_pay;
        }

        int rc = tm_send_tel_from_to(index_ack, des, payload, dest_node);
        return rc;
    }
    else
    {
        return CMD_SYNTAX_ERROR;
    }
}

int tm_parse_payload(char *fmt, char *params, int nparams)
{
    if(params == NULL)
        return CMD_SYNTAX_ERROR;

    com_frame_t *frame = (com_frame_t *)params;
    int payload = frame->type - TM_TYPE_PAYLOAD; // Payload type
    int j, offset, errors = 0;

    if(payload >= last_sensor)
        return CMD_SYNTAX_ERROR;

    _ntoh32_buff(frame->data.data32, sizeof(frame->data.data8)/ sizeof(uint32_t));

    assert(frame->ndata*data_map[payload].size <= COM_FRAME_MAX_LEN);
    for(j=0; j < frame->ndata; j++)
    {
        offset = j * data_map[payload].size; // Select next struct
        int rc = dat_add_payload_sample((frame->data.data8) + offset, payload); //Save next struct
        if(rc == -1)
            errors ++;
    }

    return errors > 0 ? CMD_ERROR : CMD_OK;
}

int tm_set_ack(char *fmt, char *params, int nparams) {
    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_SYNTAX_ERROR;
    }

    uint32_t payload;
    uint32_t k_samples;

    if(nparams == sscanf(params, fmt, &payload, &k_samples)) {

        if(payload >= last_sensor) {
            LOGE(tag, "payload not found")
            return CMD_ERROR;
        }

        if(k_samples < 1) {
            LOGE(tag, "could not acknowledge %d", k_samples)
            return CMD_ERROR;
        }

        int ack_pay = dat_get_system_var(data_map[payload].sys_ack);
        int index_pay = dat_get_system_var(data_map[payload].sys_index);

        if(ack_pay== -1 || index_pay==-1) {
            LOGE(tag, "something bad happen");
            return CMD_ERROR;
        }

        if( ack_pay > k_samples) {
            return CMD_ERROR;
        }

        if( k_samples > index_pay) {
            ack_pay = index_pay;
        } else  {
            ack_pay = k_samples;
        }

        dat_set_system_var(data_map[payload].sys_ack, ack_pay);
        return CMD_OK;
    }
    else
    {
        return CMD_SYNTAX_ERROR;
    }
}

int tm_send_cmds(char *fmt, char *params, int nparams)
{
    int node;
    if(params == NULL || sscanf(params, fmt, &node) != nparams)
        return CMD_SYNTAX_ERROR;

    return _com_send_data(node, cmd_save_all(), strlen(cmd_save_all()), TM_TYPE_HELP, 1, 0);
}

int tm_send_fp(char *fmt, char *params, int nparams)
{
    int node;
    if(params == NULL || sscanf(params, fmt, &node) != nparams)
        return CMD_SYNTAX_ERROR;

    int fp_entries = dat_get_system_var(dat_fpl_queue);
    LOGI(tag, "print fp_entries %d", fp_entries);
    fp_container_t fp_send[fp_entries];
    int i, entry_idx = 0;

    for(i = 0; i < SCH_FP_MAX_ENTRIES; i++)
    {
        fp_entry_t fp_entry;
        int ok = dat_get_fp_st_index(i, &fp_entry);
        if(ok == SCH_ST_OK && fp_entry.unixtime != ST_FP_NULL && entry_idx < fp_entries)
        {
            fp_send[entry_idx].unixtime = csp_hton32(fp_entry.unixtime);
            fp_send[entry_idx].node = csp_hton32(fp_entry.node);
            fp_send[entry_idx].executions = csp_hton32(fp_entry.executions);
            fp_send[entry_idx].periodical = csp_hton32(fp_entry.periodical);
            memset(fp_send[entry_idx].cmd_args, '\0', FP_CMDARGS_MAX_LEN);
            snprintf(fp_send[entry_idx].cmd_args, FP_CMDARGS_MAX_LEN, "%s %s", fp_entry.cmd, fp_entry.args);
            entry_idx ++;
        }
    }
    // Send all data over one connection
    int rc = com_send_telemetry(node, SCH_TRX_PORT_TM, TM_TYPE_FP, &fp_send, sizeof(fp_send), fp_entries, 0);
    return rc;
}

int tm_print_fp(char *fmt, char *params, int nparams)
{
    if(params == NULL)
        return CMD_SYNTAX_ERROR;

    int max_buff_len = FP_CMDARGS_MAX_LEN + 50; //Maximum length of commands, parameters, unixtime, node, executions,
            // periodical as concatenated string
    char buff[max_buff_len];
    com_frame_t *frame = (com_frame_t *)params;
    fp_container_t *fp_recv = (fp_entry_t *)frame->data.data8;

    snprintf(buff, max_buff_len, "%s\t%d\t%d\t%d\t%d", fp_recv->cmd_args, csp_ntoh32(fp_recv->unixtime),
             csp_ntoh32(fp_recv->node), csp_ntoh32(fp_recv->executions), csp_ntoh32(fp_recv->periodical));
    LOGI(tag, "Flight plan entry is %s\n", buff);
    return CMD_OK;
}

#if defined(LINUX) || defined(SIM)
int tm_dump(char *fmt, char *params, int nparams)
{
    int payload_id;
    char filename[SCH_CMD_MAX_STR_PARAMS];

    if(params == NULL || sscanf(params, fmt, &payload_id, filename) != nparams)
        return CMD_SYNTAX_ERROR;

    FILE *outfile = fopen(filename,"w");
    if(outfile == NULL)
        return CMD_ERROR;

    // Write header (comma separated)
    char *header = strdup(data_map[payload_id].var_names);
    for(int i=0; i<strlen(header); i++)
        if(header[i] == ' ') header[i]=','; // Replace spaces with ','
    fprintf(outfile, "%s,\n", header);
    free(header);

    // Write data
    int ret;
    int payload_len = dat_get_system_var(data_map[payload_id].sys_index);
    int payload_size = data_map[payload_id].size;
    char buff[payload_size];
    for(int i=0; i<payload_len; i++)
    {
        ret = dat_get_payload_sample(buff, payload_id,i);
        if(ret == 0)
            dat_fprint_payload_struct(outfile, buff, payload_id);
    }

    fclose(outfile);
    return CMD_OK;
}

int tm_send_file(char *fmt, char *params, int nparams)
{
    if(params == NULL)
    {
        LOGE(tag, "params is null!");
        return CMD_SYNTAX_ERROR;
    }

    char file_name[100];
    int node;
    if(nparams == sscanf(params, fmt, file_name, &node))
    {
        // Open file
        FILE *fptr;
        fptr = fopen(file_name,"rb");  // r for read, b for binary
        if(fptr == NULL)
        {
            LOGE(tag, "Error reading file %s", file_name);
            return CMD_ERROR;
        }
        // Determine file size
        fseek(fptr, 0L, SEEK_END);
        long sz = ftell(fptr);
        fseek(fptr, 0L, SEEK_SET);
        // Read file
        char *buffer = malloc((int)sz);
        if(buffer == NULL)
            return CMD_ERROR;
        fread(buffer, (int)sz,1, fptr);
        fclose(fptr);
        // Send file using CSP
        char *bname = basename(file_name);
        int rc = com_send_file(node, bname, buffer, sz);
        // Clean and return
        free(buffer);
        return rc;
    }
    else
        return CMD_SYNTAX_ERROR;

}

int tm_parse_file(char *fmt, char *params, int nparams)
{
    if(params == NULL)
        return CMD_SYNTAX_ERROR;

    com_frame_file_t *frame = (com_frame_file_t *)params;
    int type = frame->type;

    char *bname = "recv_files/";
    static char *fname = NULL;
    static int last_id = 0;
    static int last_frame = 0;

    // Check if base directory exist
    struct stat st = {0};
    if (stat(bname, &st) == -1) {
        mkdir(bname, 0700);
    }


    if(type == TM_TYPE_FILE_START)
    {
        char *rname = (char *)frame->data;
        char idname[10];
        static char *expectedfname = NULL;

        snprintf(idname, 10, "%d_", frame->fileid);
        size_t path_len = strlen(bname) + strlen(idname) + strlen(rname) + 1;
        fname = calloc(path_len, 1);
        assert(fname != NULL);

        expectedfname = calloc(path_len, 1);
        assert(expectedfname != NULL);
        strncat(expectedfname, bname, path_len);
        strncat(expectedfname, rname, path_len);
        if(access(expectedfname, F_OK) == 0 ) {
            //file exists
            strncat(fname, bname, path_len);
            strncat(fname, idname, path_len);
            strncat(fname, rname, path_len);
        } else
            //file doesn't exist
            strcpy(fname, expectedfname);

        last_id = frame->fileid;
        last_frame = frame->nframe;
        LOGI(tag, "New file! Id: %d. Frame: %d/%d. Name: %s", frame->fileid, frame->nframe, frame->total, fname);
    }

    if(type == TM_TYPE_FILE_PART)
    {
        size_t sname = strlen(bname)+20;
        fname = calloc(sname, 1);
        snprintf(fname, sname, "%s%d_%d_%d.part", bname, frame->fileid, frame->nframe, frame->total);
        last_id = frame->fileid;
        last_frame = frame->nframe-1;
        LOGI(tag, "New part! Id: %d. Frame: %d/%d. Name: %s", frame->fileid, frame->nframe, frame->total, fname);
    }

    if(type == TM_TYPE_FILE_DATA || type == TM_TYPE_FILE_PART || type == TM_TYPE_FILE_END)
    {
        LOGI(tag, "Id: %d. Frame: %d/%d", frame->fileid, frame->nframe, frame->total, fname);
        if(fname == NULL || last_id != frame->fileid || last_frame != frame->nframe-1)
        {
            LOGE(tag, "Invalid file frame! Last Id: %d. Last frame: %d", last_id, last_frame);
            return CMD_ERROR;
        }

        int nbytes = COM_FRAME_MAX_LEN;
        if(type == TM_TYPE_FILE_END || (type == TM_TYPE_FILE_PART && frame->nframe == frame->total))
        {
            // Remove file tail
            int i;
            for(i=COM_FRAME_MAX_LEN-1; i>=0; i--)
                if(frame->data[i] != 0xAA)
                    break;
            nbytes = i+1;
        }

        FILE *fptr;
        char *mode = type == TM_TYPE_FILE_PART ? "wb" : "ab";
        fptr = fopen(fname, mode);
        if(fptr == NULL)
        {
            LOGE(tag, "Error opening file! %s", fname);
            return CMD_ERROR;
        }

        last_id = frame->fileid;
        last_frame = frame->nframe;
        int cur_size = (int)ftell(fptr);
        int written = fwrite(frame->data, 1, nbytes, fptr);
        LOGI(tag, "Written %d. Current file size: %d.", written, cur_size);
        fclose(fptr);
    }

    if(type == TM_TYPE_FILE_END || type == TM_TYPE_FILE_PART)
    {
        LOGR(tag, "Closing file! Id: %d. Frames: %d/%d. Name: %s", frame->fileid, frame->nframe, frame->total, fname);
        if(fname != NULL)
            free(fname);
        fname = NULL;
        last_id = 0;
        last_frame = 0;
    }

    return CMD_OK;
}

int tm_send_file_parts(char *fmt, char *params, int nparams)
{
    char file_name[SCH_CMD_MAX_STR_PARAMS];
    int file_id;
    int start_frame;
    int end_frame;
    int node;
    if(params == NULL || sscanf(params, fmt, file_name, &file_id, &start_frame, &end_frame, &node) != nparams)
        return CMD_SYNTAX_ERROR;

    // Open file
    FILE *fptr;
    fptr = fopen(file_name,"rb");  // r for read, b for binary
    if(fptr == NULL)
    {
        LOGE(tag, "Error reading file %s", file_name);
        return CMD_ERROR;
    }

    // Determine file size
    fseek(fptr, 0L, SEEK_END);
    long file_size = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);

    // Select only the required bytes
    int start_byte = start_frame * COM_FRAME_MAX_LEN;
    size_t read_bytes = (end_frame - start_frame) * COM_FRAME_MAX_LEN;

    if(end_frame < 0)   // end_frame = 1, send all file
        read_bytes = file_size;
    else
        read_bytes = (start_byte + read_bytes) > file_size ? (file_size - start_byte) : read_bytes;

    //Check file limits
    if(start_byte > file_size || start_byte+read_bytes > file_size)
        return CMD_SYNTAX_ERROR;
    // Read file
    char *buffer = malloc(read_bytes);
    if(buffer == NULL)
        return CMD_ERROR;
    fseek(fptr, start_byte, SEEK_SET);
    fread(buffer, read_bytes, 1, fptr);
    fclose(fptr);

    // Send file using CSP
    int total_frames = file_size / COM_FRAME_MAX_LEN;
    total_frames += file_size % COM_FRAME_MAX_LEN ? 1 : 0; // Add extra frame
    int rc = com_send_file_parts(node, buffer, read_bytes, file_id, start_frame, total_frames-1, TM_TYPE_FILE_PART, NULL);

    // Clean and return
    free(buffer);
    return rc;
}

int _filter_file_part_name(const struct dirent * dir)
{
    int id, frame, max;
    if(sscanf(dir->d_name, "%d_%d_%d.part", &id, &frame, &max) == 3)
        if(id == _merging_file_id)
            return 1;
    return 0;
}

int tm_merge_file(char *fmt, char *params, int nparams)
{
    char file_name[SCH_CMD_MAX_STR_PARAMS];
    char cmd[SCH_CMD_MAX_STR_PARAMS];
    int file_id, file_max, rc;
    if(params == NULL || sscanf(params, fmt, file_name, &file_id) != nparams)
        return CMD_SYNTAX_ERROR;

    // Check for missing frames
    struct dirent **namelist;
    _merging_file_id = file_id;
    int n = scandir("recv_files", &namelist, _filter_file_part_name, versionsort);
    if (n > 0)
    {
        int id, frame, max, last=-1;
        for (int i=0; i<n; i++)
        {
            if(sscanf(namelist[i]->d_name, "%d_%d_%d.part", &id, &frame, &max) == 3)
            {
                // Check missing frames between last and current frame number
                while(++last != frame)
                    LOGW(tag, "MISSING FRAME: %d_%d_%d.part", id, last, max);
            }
            free(namelist[i]);
        }
        // Check missing frames between last file and expected frames
        while(last++ < max)
            LOGW(tag, "MISSING FRAME: %d_%d_%d.part", id, last, max);
        free(namelist);
    }

    // Merge with cat and find shell commands
    snprintf(cmd, SCH_CMD_MAX_STR_PARAMS-1, "cat $(find -name \"%d_*.part\" | sort -V) > recv_files/%s", file_id, file_name);
    LOGI(tag, "%s", cmd);
    rc = system(cmd);

    _merging_file_id = 0;
    return rc == 0 ? CMD_OK : CMD_ERROR;
}

int tm_list_files(char* fmt, char* params, int nparams)
{
    DIR *d;
    struct dirent *dir;
    struct stat fstat;
    char path[SCH_CMD_MAX_STR_PARAMS];
    char buff[COM_FRAME_MAX_LEN];
    int node;
    if(params == NULL || sscanf(params, fmt, path, &node) != nparams)
        return CMD_SYNTAX_ERROR;

    d = opendir(path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            snprintf(buff, COM_FRAME_MAX_LEN, "%s/%s", path, dir->d_name);
            stat(buff, &fstat);
            LOGR(tag, "%d\t %s", fstat.st_size, dir->d_name);
            snprintf(buff, COM_FRAME_MAX_LEN,"%ld\t %s", fstat.st_size, dir->d_name);
            int buff_len = strlen(buff);
            int rc = com_send_telemetry(node, SCH_TRX_PORT_DBG_TM, 0, buff, buff_len, buff_len, 0);
            if(rc != CMD_OK)
                break;
        }
        closedir(d);
        return CMD_OK;
    }
    return CMD_ERROR;
}
#endif
