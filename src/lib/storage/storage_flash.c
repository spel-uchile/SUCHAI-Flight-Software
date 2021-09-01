/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2021, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *      Copyright 2021, Camilo Rojas Milla, camrojas@uchile.cl
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

#include "suchai/storage.h"
#include "app/drivers/drivers.h"

static const char *tag = "data_storage";

char* fp_table = "flightPlan";

///< Flight plan address buffer
typedef struct fp_container{
    int unixtime;               ///< Unix-time, sets when the command should next execute
    int executions;             ///< Amount of times the command will be executed per periodic cycle
    int periodical;             ///< Period of time between executions
    int node;                   ///< Node to execute the command
    char cmd[SCH_CMD_MAX_STR_NAME]; ///< Command to execute
    char args[SCH_CMD_MAX_STR_PARAMS]; ///< Command's arguments
} fp_container_t;

static uint32_t* st_flightplan_addr;
static int st_flightplan_base_addr = 0; // TODO: Check this, cannot be equal to st_payload_base_addr
#define FP_CONTAINER_SIZE (sizeof(fp_container_t))
static int sections_for_fp = (SCH_FP_MAX_ENTRIES * FP_CONTAINER_SIZE) / SCH_SIZE_PER_SECTION + 1;
static int st_flightplan_entries = 0;

///< Payloads storage address buffer
static uint32_t* st_payload_addr;
static int payload_tables_amount;
static int st_payload_base_addr = 0; // TODO: Check this, cannot be equal to st_flightplan_base_addr

static int storage_is_open = 0;

static int storage_read_flash(uint8_t partition, uint32_t addr, uint8_t *data, uint16_t len)
{
    int rc = spn_fl512s_read_data(partition, addr, data, len);
    return rc == GS_OK ? SCH_ST_OK : SCH_ST_ERROR;
}

static int storage_write_flash(uint8_t partition, uint32_t addr, const uint8_t *data, uint16_t len){
    int rc = spn_fl512s_write_data(partition, addr, data, len);
    return rc == GS_OK ? SCH_ST_OK : SCH_ST_ERROR;
}

int storage_init(const char *file)
{
#if 0
    /* Init FRAM storage */
    /* FIXME: Not necessary, already performed in init.c */
//    const gs_fm33256b_config_t fram = {.spi_slave = GS_A3200_SPI_SLAVE_FRAM};
//    int error = (int)gs_fm33256b_init(0, &fram);
//    if (error)
//        return -1;

    /* Init FLASH NOR storage */
    /* FIXME: Not necessary, already performed in init.c */
    /* Turn on power */
//    gs_a3200_pwr_switch_enable(GS_A3200_PWR_SD);
//    /* Initialize spansion chip. Requires that the SPI device has been initialized */
//    const spn_fl512s_config_t config = {
//            .bps = 8000000,
//            .cs_part_0 = SPN_FL512S_CS0,
//            .cs_part_1 = SPN_FL512S_CS1,
//            .spi_slave_part_0 = GS_A3200_SPI_SLAVE_SPN_FL512_0,
//            .spi_slave_part_1 = GS_A3200_SPI_SLAVE_SPN_FL512_1,
//            .spi_handle = 1,
//    };
//
//    error = (int)spn_fl512s_init(&config);
//    if (error)
//        return -1;
#endif

    storage_is_open = 1;
    return SCH_ST_OK;
}

int storage_close(void)
{
    storage_is_open = 0;
    if(st_flightplan_addr != NULL) {free(st_flightplan_addr); st_flightplan_addr = NULL;}
    if(st_payload_addr != NULL) {free(st_payload_addr); st_payload_addr = NULL;}

    return SCH_ST_OK;
}

// TODO: Implement this
int storage_table_status_init(char *table, int n_variables, int drop)
{
    return SCH_ST_ERROR;
}

int storage_table_flight_plan_init(char *table, int n_entries, int drop)
{
    if(!storage_is_open)
        return SCH_ST_ERROR;

    //TODO: initialize st_flightplan_entries
    //st_flightplan_entries = ???

    if(st_flightplan_addr != NULL) {
        // Table initialized, but want to drop -> reset table
        if (drop)
        {
            free(st_flightplan_addr);
            storage_flight_plan_reset(n_entries);
        }
        // Table is already initialized?! -> error
        else return SCH_ST_ERROR;
    }

    // Save the sections' addresses reserved for flight plan storage
    st_flightplan_addr = malloc(sections_for_fp * sizeof(uint32_t));
    for (int i = 0; i < sections_for_fp; i++)
        st_flightplan_addr[i] = (uint32_t)SCH_FLASH_INIT_MEMORY + (st_flightplan_base_addr + i) * SCH_SIZE_PER_SECTION;

    return SCH_ST_OK;
}

int storage_table_payload_init(char *table, data_map_t *data_map, int n_entries, int drop)
{
    if(!storage_is_open)
        return SCH_ST_ERROR;

    if(st_payload_addr != NULL) {
        // Table initialized, but want to drop -> reset table
        if (drop)
        {
            free(st_payload_addr);
            st_payload_addr = NULL;
        }
        // Table is already initialized?! -> error
        else return SCH_ST_ERROR;
    }

    /* Init storage addresses */
    payload_tables_amount = SCH_SECTIONS_PER_PAYLOAD*n_entries;
    st_payload_addr = malloc(payload_tables_amount * sizeof(uint32_t));

    for (int i = 0; i < payload_tables_amount; i++)
        st_payload_addr[i] = (uint32_t)SCH_FLASH_INIT_MEMORY + (st_payload_base_addr + i) * SCH_SIZE_PER_SECTION;

    return 0;
}

/****** STATUS VARIABLES FUNCTIONS *******/
int storage_status_get_value_idx(uint32_t index, value32_t *value, char *table)
{

}

int storage_status_set_value_idx(int index, value32_t value, char *table)
{

}

/****** FLIGHT PLAN VARIABLES FUNCTIONS *******/
/**
 * Function for finding the storage index of a command based on it's timetodo field.
 *
 * IMPORTANT: Flight plan entries are saved as consecutive binary values using the following scheme:
 * timetodo(uint32_t) executions(uint32_t) periodical(uint32_t) name_length(uint32_t) args_length(uint32_t) name(char*SCH_CMD_MAX_STR_NAME) args(char*SCH_CMD_MAX_STR_PARAMS)
 *
 * The total size of each command is then stored in 'max_command_size'.
 *
 * @param timetodo Execution time of the command to find
 * @return The command's index, -1 if not found or error
 */
static int flight_plan_find_index(int timetodo)
{
    // Calculates information on how the flight plan is stored
    int commands_per_section = SCH_SIZE_PER_SECTION / FP_CONTAINER_SIZE;

    // For every index
    for (int i = 0; i < st_flightplan_entries; i++)
    {
        // Translates the index into a flash section index and section address
        int section_index = i/commands_per_section;
        int index_in_section = i%commands_per_section;
        uint32_t addr = st_flightplan_addr[section_index] + index_in_section * FP_CONTAINER_SIZE;

        // Reads the entry's timetodo
        uint32_t found_time;
        storage_read_flash(0, addr, (uint8_t*)&found_time, sizeof(fp_container_t.unixtime));

        // If found, returns
        if (found_time == (uint32_t)timetodo)
            return i;
    }

    // If couldn't find the entry
    return -1;
}

/**
 * Function for deleting a flight plan entry on a given index.
 *
 * @param index Storage index of the entry
 * @return entries written if OK, -1 if Error
 */
static int flight_plan_erase_index(int index, int entries)
{
    if (index < 0 || index >= entries)
    {
        LOGW(tag, "Failed attempt at erasing flight plan entry index %d, out of bounds", index);
        return -1;
    }

    // Generates a buffer for the flight plan entries
//    uint8_t section_data[entries * FP_CONTAINER_SIZE];
    fp_container_t section_data[entries];

    // Calculates memory address of the section
    int commands_per_section = SCH_SIZE_PER_SECTION / FP_CONTAINER_SIZE;
    int section_index = index/commands_per_section;
    int index_in_section = index%commands_per_section;

    uint32_t addr = st_flightplan_addr[section_index];

    // Reads the whole section
    storage_read_flash(0, addr, (uint8_t*)section_data, (uint16_t)(entries*FP_CONTAINER_SIZE));

    // Deletes the section
    LOGI(tag, "Deleting section in address %u", (unsigned int)addr);
    int rc = spn_fl512s_erase_block(0, addr);
    if (rc != 0)
    {
        LOGE(tag, "Failed attempt at deleting data in storage address %u", (unsigned int)addr);
        return -1;
    }

    // Counts the amount of entries written
    int written_entries = 0;

    // Rewrites all commands, except the one that was going to be erased and any that were 'null'
    for (int i = 0; i < entries; i++)
    {
        // int buffer_index = i * FP_CONTAINER_SIZE;
        // int timetodo = *((int*)&(section_data[buffer_index]));
        int timetodo = section_data[i].unixtime;

        // If the address had a command, and it wasn't the one to erase
        if (i != index_in_section && timetodo != 0)
        {
            // Writes
            rc = storage_write_flash(0, addr, (uint8_t *)(section_data+i), FP_CONTAINER_SIZE);

            if (rc != 0)
            {
                LOGE(tag, "Failed attempt at writing data in storage address %u", (unsigned int)addr);
                entries = written_entries;
                return -1;
            }

            // Moves to the next address
            addr += FP_CONTAINER_SIZE;
            written_entries++;
        }
    }

    // Sets the amount of entries written
    entries = written_entries;
    return entries;
}

int storage_flight_plan_set_st(fp_entry_t *row)
{
    if(row==NULL || !storage_is_open)
        return SCH_ST_ERROR;

    // Finds an index with an empty entry
    int index = st_flightplan_entries;
    if (index >= SCH_FP_MAX_ENTRIES)
    {
        LOGE(tag, "Flight plan storage no longer has space for another command");
        return SCH_ST_ERROR;
    }

    // Calculates memory address
    int commands_per_section = SCH_SIZE_PER_SECTION / FP_CONTAINER_SIZE;
    int section_index = index/commands_per_section;
    int index_in_section = index%commands_per_section;
    uint32_t addr = st_flightplan_addr[section_index] + index_in_section * FP_CONTAINER_SIZE;

    fp_container_t new_entry;
    new_entry.unixtime = row->unixtime;
    new_entry.executions = row->executions;
    new_entry.periodical = row->periodical;
    new_entry.node = row->node;
    memset(new_entry.cmd, 0, sizeof(new_entry.cmd));
    strncpy(new_entry.cmd, row->cmd, sizeof(new_entry.cmd));
    memset(new_entry.args, 0, sizeof(new_entry.args));
    strncpy(new_entry.args, row->args, sizeof(new_entry.args));

    // Writes fp entry value
    int rc = storage_write_flash(0, addr, (uint8_t *)&new_entry, sizeof(new_entry));
    if(rc == SCH_ST_OK)
        st_flightplan_entries += 1;
    return rc;
}

int storage_flight_plan_set(int timetodo, char* command, char* args, int executions, int period, int node)
{
    if(command == NULL || args == NULL || !storage_is_open)
        return SCH_ST_ERROR;

    fp_entry_t fp_entry;
    fp_entry.unixtime = timetodo;
    fp_entry.executions = executions;
    fp_entry.periodical = period;
    fp_entry.node = node;
    fp_entry.cmd = command;
    fp_entry.args = args;

    return storage_flight_plan_set_st(&fp_entry);
}

int storage_flight_plan_get_idx(int index, fp_entry_t *row)
{
    if(index > st_flightplan_entries || row == NULL)
        return SCH_ST_ERROR;

    // Calculates memory address
    int commands_per_section = SCH_SIZE_PER_SECTION / FP_CONTAINER_SIZE;
    int section_index = index/commands_per_section;
    int index_in_section = index%commands_per_section;
    uint32_t addr = st_flightplan_addr[section_index] + index_in_section * FP_CONTAINER_SIZE;

    // Read one entry
    fp_container_t fp_entry;
    int rc = storage_read_flash(0, addr, (uint8_t*)&fp_entry , sizeof(fp_container_t));
    if(rc != SCH_ST_OK)
        return SCH_ST_ERROR;

    //Copy values
    row->unixtime = fp_entry.unixtime;
    row->periodical = fp_entry.periodical;
    row->executions = fp_entry.executions;
    row->node = fp_entry.node;
    row->cmd = strdup(fp_entry.cmd);
    row->args = strdup(fp_entry.args);

    // Deletes the command from storage
    rc = flight_plan_erase_index(index);
    return rc;
}

int storage_flight_plan_get_args(int timetodo, char* command, char* args, int* executions, int* period, int* node)
{
    // Finds the table index for timetodo
    int index = flight_plan_find_index(timetodo);
    if (index < 0 || command == NULL || args == NULL || executions == NULL || period == NULL || node == NULL)
        return SCH_ST_ERROR;
}

int storage_flight_plan_erase(int timetodo, int * entries)
{
    // Finds the index to erase
    int index = flight_plan_find_index(timetodo, *entries);

    if (index < 0)
    {
        LOGW(tag, "Couldn't find command to erase");
        return -1;
    }

    // Erases the entry in index
    int rc = flight_plan_erase_index(index, entries);

    if (rc != 0)
    {
        LOGE(tag, "Couldn't erase command");
    }

    return rc;
}

int storage_flight_plan_reset(int * entries)
{

    int commands_per_section = SCH_SIZE_PER_SECTION / FP_CONTAINER_SIZE;

    // Amount of sections that the flight plan uses
    int fp_sections = (*entries/commands_per_section) + 1;

    // Deletes all flight plan memory sections
    for (int i = 0; i < fp_sections; i++)
    {
        LOGI(tag, "Deleting section in address %u\n", (unsigned int)st_flightplan_addr[i]);
        int rc = spn_fl512s_erase_block(0, st_flightplan_addr[i]);
        if (rc != 0)
        {
            LOGE(tag, "Failed attempt at deleting data in storage address %u", (unsigned int)st_flightplan_addr[i]);
            return -1;
        }

        // An entries section gets erased
        *entries = *entries - commands_per_section;
    }

    // The entries amount can't be below zero
    *entries = (*entries < 0) ? 0 : *entries;
    return 0;
}

int storage_flight_plan_show_table(int entries)
{
    if (entries == 0)
    {
        LOGI(tag, "Flight plan table empty");
        return 0;
    }

    LOGI(tag, "Flight plan table");

    int commands_per_section = SCH_SIZE_PER_SECTION / FP_CONTAINER_SIZE;

    for (int index = 0; index < entries; index++)
    {
        // Calculates memory address
        int section_index = index/commands_per_section;
        int index_in_section = index%commands_per_section;

        uint32_t add = st_flightplan_addr[section_index] + index_in_section * FP_CONTAINER_SIZE;

        // Finds numeric values
        uint32_t timetodo;
        numbers_container_t container;

        storage_read_flash(0, add, (uint8_t*)&timetodo, sizeof(uint32_t));

        add += sizeof(uint32_t);

        storage_read_flash(0, add, (uint8_t*)&container, sizeof(numbers_container_t));

        add += sizeof(numbers_container_t);

        // Finds string values
        char command[container.name_len+1];
        char args[container.args_len+1];

        command[container.name_len] = '\0';
        args[container.args_len] = '\0';

        storage_read_flash(0, add, (uint8_t*)command, sizeof(char)*container.name_len);

        add += SCH_CMD_MAX_STR_NAME;

        storage_read_flash(0, add, (uint8_t*)args, sizeof(char)*container.args_len);

        // Prints a row of the table

        time_t timef = timetodo;

        printf("%s\t%s\t%s\t%lu\n", ctime(&timef), command, args, container.peri);
    }

    return 0;
}

/****** PAYLOAD STORAGE FUNCTIONS *******/

int storage_repo_get_value_idx(int index, char *table)
{
    // TODO: Check if this is necessary
    data32_t data;
    uint16_t len = (uint16_t)(sizeof(uint32_t));
    uint16_t add = (uint16_t)(index*len);

    gs_fm33256b_fram_read(0, add, data.data8_p, len);

    LOGV(tag, "Read 0x%X", (unsigned int)data.data32);
    return (int)(data.data32);
}

int storage_repo_get_value_str(char *name, char *table)
{
    // FIXME: return -1 if not implemented?
    return 0;
}

int storage_repo_set_value_idx(int index, int value, char *table)
{
    // TODO: Check if this is necessary
    data32_t data;
    data.data32 = (uint32_t)value;
    uint16_t len = (uint16_t)(sizeof(data));
    uint16_t add = (uint16_t)(index*len);

    LOGV(tag, "Writing 0x%X", (unsigned int)data.data32);
    gs_fm33256b_fram_write(0, add, data.data8_p, len);

    return 0;
}

int storage_repo_set_value_str(char *name, int value, char *table)
{
    return 0;
}

int write_data_with_check(uint32_t add, uint8_t * data, uint16_t size)
{
    int i;
    uint8_t data_aux[size];
    for (i=0 ; i < 10; ++i) {
        uint32_t add_aux = add + i*size;
        int ret_write = storage_write_flash(0, add_aux, data, size);

        int ret_read = storage_read_flash(0, add_aux, data_aux, size);

        if (ret_read != 0 || ret_write != 0 ) {
            return -1;
        }
        // Check integrity
        int j, integrity=0;
        for (j=0; j < size; j++) {
            if (data[j] != data_aux[j]) {
                integrity += 1;
                break;
            }
        }
        if (integrity == 0) {
            return i;
        }
    }
    return -1;
}

int storage_set_payload_data(int index, void* data, int payload)
{
    if(payload >= last_sensor)
    {
        LOGE(tag, "Payload id: %d greater than maximum id: %d", payload, last_sensor);
        return -1;
    }

    int payloads_per_section = SCH_SIZE_PER_SECTION/data_map[payload].size;

    int payload_section = index/payloads_per_section;
    int index_in_section = index%payloads_per_section;

    int section_index = payload*SCH_SECTIONS_PER_PAYLOAD + payload_section;

    uint32_t add = st_payload_addr[section_index] + index_in_section * data_map[payload].size;

    if (payload_section >= SCH_SECTIONS_PER_PAYLOAD)
    {
        LOGE(tag, "Payload address: %u is out of bounds", (unsigned int)add);
        return -1;
    }

    LOGI(tag, "Writing in address: %u, %d bytes\n", (unsigned int)add, data_map[payload].size);
//    int ret = storage_write_flash(0, add, data, data_map[payload].size);
    int ret = write_data_with_check(add, (uint8_t *)data, data_map[payload].size);
    return ret;
}

int read_data_with_check(uint32_t add, uint8_t * data, uint16_t size) {

    int i;
    for (i=0 ; i < 10; ++i) {
        uint32_t add_aux = add + i*size;
        int read_ret = storage_read_flash(0, add_aux, data, size);

        if( read_ret != 0) {
            return -1;
        }
        int j, integity=0;
        for (j=0; j<size/4; ++j) {
            if ( ((uint32_t*)data)[j] == 0xffffffff) {
                integity +=1;
                break;
            }
        }
        if (integity == 0) {
            return i;
        }
    }
    return -1;
}

int storage_get_payload_data(int index, void* data, int payload)
{
    if(payload >= last_sensor)
    {
        LOGE(tag, "payload id: %d greater than maximum id: %d", payload, last_sensor);
        return -1;
    }

    int payloads_per_section = SCH_SIZE_PER_SECTION/data_map[payload].size;

    int payload_section = index/payloads_per_section;
    int index_in_section = index%payloads_per_section;

    int section_index = payload*SCH_SECTIONS_PER_PAYLOAD + payload_section;

    uint32_t add = st_payload_addr[section_index] + index_in_section * data_map[payload].size;

    if (payload_section >= SCH_SECTIONS_PER_PAYLOAD)
    {
        LOGE(tag, "payload address: %u is out of bounds", (unsigned int)add);
        return -1;
    }

    LOGI(tag, "Reading in address: %u, %d bytes\n", (unsigned int)add, data_map[payload].size);
//    printf("Reading values of size %u \n", data_map[payload]);
//    storage_read_flash(add, (uint8_t *) data, data_map[payload]);
    int read_ret = 0;
//    read_ret = storage_read_flash(0, add, (uint8_t *)data, data_map[payload].size);
    read_ret = read_data_with_check(add, (uint8_t *)data, data_map[payload].size);
    if (read_ret < 0) {
        return -1 ;
    }
    return 0;
}

int storage_delete_memory_sections()
{
    // Deleting Payload Memory Sections
    //spn_fl512s_erase_chip(0);
    for(int i = 0;  i < SCH_SECTIONS_PER_PAYLOAD*last_sensor; ++i)
    {
        LOGI(tag, "deleting section in address %u\n", (unsigned int)st_payload_addr[i]);
        int rc = spn_fl512s_erase_block(0, st_payload_addr[i]);
        if (rc != 0)
        {
            LOGE(tag, "Failed attempt at deleting data in storage address %u", (unsigned int)st_payload_addr[i]);
            return -1;
        }
    }
    return 0;
}

int storage_table_payload_init(int drop)
{
    // FIXME: IMPLEMENT
    return 0;
}
