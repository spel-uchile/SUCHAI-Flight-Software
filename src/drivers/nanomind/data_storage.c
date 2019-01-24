//
// Created by carlos on 22-08-17.
//

#include "data_storage.h"

static const char *tag = "data_storage";

static void *db = NULL;
char* fp_table = "flightPlan";

struct temp_data tempdata;
struct ads_data adsdata;

static struct {
    uint16_t  size;
    int sys_index;
} data_map[last_sensor] = {
        {(uint16_t) (sizeof(tempdata)), dat_mem_temp},
        {(uint16_t) (sizeof(adsdata)), dat_mem_ads}
};

/**
 * Arrays for storing the memory addresses of the flight plan and payload tables.
 */
static uint32_t* storage_addresses_payloads;
static uint32_t* storage_addresses_flight_plan;

typedef struct {
    uint32_t exec, peri, name_len, args_len;
} numbers_container_t;

static int max_command_size = (SCH_CMD_MAX_STR_NAME+SCH_CMD_MAX_STR_PARAMS)*sizeof(char)+sizeof(uint32_t)+sizeof(numbers_container_t);

static int dummy_callback(void *data, int argc, char **argv, char **names);

int storage_init(const char *file)
{
    /* Init RTC storage */
    fm33256b_init();

    /* Init FLASH NOR storage */
    spn_fl512s_init((unsigned int) 0);

    /* Init storage addresses */
    int payload_tables_amount = SCH_SECTIONS_PER_PAYLOAD*last_sensor;
    storage_addresses_payloads = malloc(payload_tables_amount*sizeof(uint32_t));
    storage_addresses_flight_plan = malloc(SCH_SECTIONS_FOR_FP*sizeof(uint32_t));

    for (int i = 0; i < payload_tables_amount; i++)
        storage_addresses_payloads[i] = (uint32_t)SCH_FLASH_INIT_MEMORY + i*SCH_SIZE_PER_SECTION;
    for (int i = 0; i < SCH_SECTIONS_FOR_FP; i++)
        storage_addresses_flight_plan[i] = (uint32_t)SCH_FLASH_INIT_MEMORY + (payload_tables_amount+i)*SCH_SIZE_PER_SECTION;

    return 0;
}

int storage_table_repo_init(char* table, int drop)
{
    return 0;
}

int storage_table_flight_plan_init(int drop)
{
    return 0;
}

int storage_repo_get_value_idx(int index, char *table)
{
    // TODO: Check if this is necessary
    data32_t data;
    uint16_t len = (uint16_t)(sizeof(uint32_t));
    uint16_t add = (uint16_t)(index*len);

    fm33256b_read_data(add, data.data8_p, len);

    LOGV(tag, "Read 0x%X", (unsigned int)data.data32);
    return (int)(data.data32);
}

int storage_repo_get_value_str(char *name, char *table)
{
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
    fm33256b_write_data(add, data.data8_p, len);

    return 0;
}

int storage_repo_set_value_str(char *name, int value, char *table)
{
    return 0;
}

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
    int commands_per_section = SCH_SIZE_PER_SECTION/max_command_size;
    int commands_amount = commands_per_section*SCH_SECTIONS_FOR_FP;

    // For every index
    for (int i = 0; i < commands_amount; i++)
    {
        // Translates the index into an address in memory
        int section_index = i/commands_per_section;
        int index_in_section = i%commands_per_section;

        uint32_t add = storage_addresses_flight_plan[section_index] + index_in_section*max_command_size;

        // Reads the entry's timetodo
        uint32_t time;
        spn_fl512s_read_data(add, (uint8_t*)&time, sizeof(uint32_t));

        // If found, returns
        if (time == (uint32_t)timetodo)
            return i;
    }

    // If couldn't find the entry
    return -1;
}

/**
 * Function for deleting a flight plan entry on a given index.
 *
 * @param index Storage index of the entry
 * @return 0 if OK, -1 if Error
 */
static int flight_plan_erase_index(int index)
{
    // Generates a buffer for the flight plan section
    uint8_t section_data[SCH_SIZE_PER_SECTION];

    // Calculates memory address of the section
    int commands_per_section = SCH_SIZE_PER_SECTION/max_command_size;
    int section_index = index/commands_per_section;
    int index_in_section = index%commands_per_section;

    uint32_t add = storage_addresses_flight_plan[section_index];

    // Reads the whole section
    spn_fl512s_read_data(add, section_data, SCH_SIZE_PER_SECTION);

    // Deletes the section
    LOGI(tag, "Deleting section in address %u", (unsigned int)add);
    int rc = spn_fl512s_erase_block(add);
    if (rc != 0)
    {
        LOGE(tag, "Failed attempt at deleting data in storage address %u", (unsigned int)add);
        return -1;
    }

    // Rewrites all commands, except the one that was going to be erased and any that were 'null'
    for (int i = 0; i < commands_per_section; i++)
    {
        int buffer_index = i*max_command_size;
        int timetodo = *((int*)&(section_data[buffer_index]));

        // If the address had a command, and it wasn't the one to erase
        if (i != index_in_section && timetodo != 0)
        {
            rc = spn_fl512s_write_data(add, &(section_data[buffer_index]), max_command_size);
        }

        // Moves to the next address
        add += max_command_size;
    }

    return 0;
}

int storage_flight_plan_set(int timetodo, char* command, char* args, int executions, int periodical)
{
    // Finds an index with an empty entry
    int index = flight_plan_find_index(0);

    if (index < 0)
    {
        LOGE(tag, "Flight plan storage no longer has space for another command");
        return -1;
    }

    // Calculates memory address
    int commands_per_section = SCH_SIZE_PER_SECTION/max_command_size;
    int section_index = index/commands_per_section;
    int index_in_section = index%commands_per_section;

    uint32_t add = storage_addresses_flight_plan[section_index] + index_in_section*max_command_size;

    // Casts timetodo
    uint32_t time = (uint32_t)timetodo;

    // Writes the timetodo value
    int rc;
    rc = spn_fl512s_write_data(add, (uint8_t*)&time, sizeof(uint32_t));

    if (rc != 0)
    {
        LOGE(tag, "Failed attempt at writing data in storage address %u", (unsigned int)add);
        return -1;
    }

    add += sizeof(uint32_t);

    // Container for writing all other numeric values in one go
    numbers_container_t numbers_container;

    numbers_container.exec = executions;
    numbers_container.peri = periodical;
    numbers_container.name_len = strlen(command);
    numbers_container.args_len = strlen(args);

    // Writes said values
    rc = spn_fl512s_write_data(add, (uint8_t*)&numbers_container, sizeof(numbers_container_t));

    if (rc != 0)
    {
        LOGE(tag, "Failed attempt at writing data in storage address %u", (unsigned int)add);
        return -1;
    }

    add += sizeof(numbers_container_t);

    // Writes the command's name
    rc = spn_fl512s_write_data(add, (uint8_t*)command, sizeof(char)*numbers_container.name_len);

    if (rc != 0)
    {
        LOGE(tag, "Failed attempt at writing data in storage address %u", (unsigned int)add);
        return -1;
    }

    add += sizeof(char)*SCH_CMD_MAX_STR_NAME;

    // Writes the command's arguments
    rc = spn_fl512s_write_data(add, (uint8_t*)args, sizeof(char)*numbers_container.args_len);

    if (rc != 0)
    {
        LOGE(tag, "Failed attempt at writing data in storage address %u", (unsigned int)add);
        return -1;
    }

    return 0;
}

int storage_flight_plan_get(int timetodo, char** command, char** args, int** executions, int** periodical)
{
    // Finds the table index for timetodo
    int index = flight_plan_find_index(timetodo);

    if (index < 0)
        return -1;

    // Calculates memory address
    int commands_per_section = SCH_SIZE_PER_SECTION/max_command_size;
    int section_index = index/commands_per_section;
    int index_in_section = index%commands_per_section;

    uint32_t add = storage_addresses_flight_plan[section_index] + index_in_section*max_command_size + sizeof(uint32_t);

    // Container for reading all the numeric values in one go
    numbers_container_t numbers_container;

    // Finds the numeric values
    spn_fl512s_read_data(add, (uint8_t*)&numbers_container, sizeof(numbers_container_t));

    add += sizeof(numbers_container_t);

    // Malloc for command name and parameters
    *command = malloc((numbers_container.name_len + 1)*sizeof(char));
    *args = malloc((numbers_container.args_len + 1)*sizeof(char));

    (*command)[numbers_container.name_len] = '\0';
    (*args)[numbers_container.args_len] = '\0';

    // Finds the command name string and sets it
    spn_fl512s_read_data(add, (uint8_t*)*command, numbers_container.name_len*sizeof(char));

    add += SCH_CMD_MAX_STR_NAME*sizeof(char);

    // Finds the parameters string and sets it
    spn_fl512s_read_data(add, (uint8_t*)*args, numbers_container.args_len*sizeof(char));

    // Malloc for the executions and periodical values
    *executions = malloc(sizeof(int));
    *periodical = malloc(sizeof(int));

    // Sets the executions and periodical values
    **executions = *((int*)&(numbers_container.exec));
    **periodical = *((int*)&(numbers_container.peri));

    // Deletes the command from storage
    int rc;
    rc = flight_plan_erase_index(index);

    if (rc != 0)
        return -1;

    // If the command is periodical, a copy is made set to execute later
    rc = storage_flight_plan_set(timetodo+**periodical, *command, *args, **executions, **periodical);

    if (rc != 0)
        return -1;

    return 0;
}

int storage_flight_plan_erase(int timetodo)
{
    // Finds the index to erase
    int index = flight_plan_find_index(timetodo);

    if (index < 0)
    {
        LOGE(tag, "Couldn't find command to erase");
        return -1;
    }

    // Erases the entry in index
    int rc = flight_plan_erase_index(index);

    if (rc != 0)
    {
        LOGE(tag, "Couldn't erase command");
    }

    return rc;
}

int storage_flight_plan_reset(void)
{
    // Deletes all flight plan memory sections
    for (int i = 0; i < SCH_SECTIONS_FOR_FP; i++)
    {
        LOGI(tag, "Deleting section in address %u\n", (unsigned int)storage_addresses_flight_plan[i]);
        int rc = spn_fl512s_erase_block(storage_addresses_flight_plan[i]);
        if (rc != 0)
        {
            LOGE(tag, "Failed attempt at deleting data in storage address %u", (unsigned int)storage_addresses_flight_plan[i]);
            return -1;
        }
    }
    return 0;
}

int storage_show_table(void)
{
    return 0;
}

int storage_close(void)
{
    free(storage_addresses_payloads);
    free(storage_addresses_flight_plan);
    return 0;
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

    int section_index = payload*2 + payload_section;

    uint32_t add = storage_addresses_payloads[section_index] + index_in_section;

    if (payload_section >= SCH_SECTIONS_PER_PAYLOAD)
    {
        LOGE(tag, "Payload address: %u is out of bounds", (unsigned int)add);
        return -1;
    }

    LOGV(tag, "Writing in address: %u, %d bytes\n", (unsigned int)add, data_map[payload].size);
    int ret = spn_fl512s_write_data(add, data, data_map[payload].size);
    if(ret != 0){
        return -1;
    }
    return 0;
}

int storage_add_payload_data(void* data, int payload)
{
    int index = dat_get_system_var(data_map[payload].sys_index);
    LOGV(tag, "Adding data for payload %d in index %d", payload, index);
    int ret = storage_set_payload_data(index, data, payload);
    // Update address
    if(ret==0) {
        dat_set_system_var(data_map[payload].sys_index, index+1);
        return index+1;
    } else {
        LOGE(tag, "Couldn't set data payload %d", payload);
        return -1;
    }
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

    int section_index = payload*2 + payload_section;

    uint32_t add = storage_addresses_payloads[section_index] + index_in_section;

    if (payload_section >= SCH_SECTIONS_PER_PAYLOAD)
    {
        LOGE(tag, "payload address: %u is out of bounds", (unsigned int)add);
        return -1;
    }

    LOGV(tag, "Reading in address: %u, %d bytes\n", (unsigned int)add, data_map[payload].size);
//    printf("Reading values of size %u \n", data_map[payload]);
//    spn_fl512s_read_data(add, (uint8_t *) data, data_map[payload]);
    spn_fl512s_read_data(add, (uint8_t *)data, data_map[payload].size);
//    printf("Reading value %d \n", *(uint8_t*)data);
//    LOGV(tag, "Read 0x%X", (unsigned int)data.data32);
    return 0;
}

int storage_get_recent_payload_data(void * data, int payload, int delay)
{
    int index = dat_get_system_var(data_map[payload].sys_index);
    LOGV(tag, "Obtaining data of payload %d, in index %d, sys_var: %d", payload, index,data_map[payload].sys_index );

    if(index-1-delay >= 0) {
        return storage_get_payload_data(index-1-delay, data, payload);
    }
    else {
        LOGE(tag, "Asked for too great of a delay when requesting payload %d on delay %d", payload, delay);
        return -1;
    }
}

int storage_delete_memory_sections()
{
    // Resetting memory system vars
    for(int i = 0; i < last_sensor; ++i)
    {
        dat_set_system_var(data_map[i].sys_index, 0);
    }

    // Deleting Payload Memory Sections
    for(int i = 0;  i < SCH_SECTIONS_PER_PAYLOAD*last_sensor; ++i)
    {
        LOGI(tag, "deleting section in address %u\n", (unsigned int)storage_addresses_payloads[i]);
        int rc = spn_fl512s_erase_block(storage_addresses_payloads[i]);
        if (rc != 0)
        {
            LOGE(tag, "Failed attempt at deleting data in storage address %u", (unsigned int)storage_addresses_payloads[i]);
            return -1;
        }
    }

    // Deleting Flight Plan Memory Sections
    for (int i = 0; i < SCH_SECTIONS_FOR_FP; i++)
    {
        LOGI(tag, "Deleting section in address %u\n", (unsigned int)storage_addresses_flight_plan[i]);
        int rc = spn_fl512s_erase_block(storage_addresses_flight_plan[i]);
        if (rc != 0)
        {
            LOGE(tag, "Failed attempt at deleting data in storage address %u", (unsigned int)storage_addresses_flight_plan[i]);
            return -1;
        }
    }

    return 0;
}

static int dummy_callback(void *data, int argc, char **argv, char **names)
{
    return 0;
}
