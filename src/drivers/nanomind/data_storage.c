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

static data32_t* storage_addresses_payloads;
static data32_t* storage_addresses_flight_plan;

static int dummy_callback(void *data, int argc, char **argv, char **names);

int storage_init(const char *file)
{
    /* Init RTC storage */
    fm33256b_init();

    /* Init FLASH NOR storage */
    spn_fl512s_init((unsigned int) 0);

    /* Init storage addresses */
    int payload_tables_amount = SCH_SECTIONS_PER_PAYLOAD*last_sensor;
    storage_addresses_payloads = malloc(payload_tables_amount*sizeof(data32_t));
    storage_addresses_flight_plan = malloc(SCH_SECTIONS_FOR_FP*sizeof(data32_t));

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

int storage_flight_plan_set(int timetodo, char* command, char* args, int executions, int periodical)
{
    return 0;
}

int storage_flight_plan_get(int timetodo, char** command, char** args, int** executions, int** periodical)
{
    return 0;
}

int storage_flight_plan_erase(int timetodo)
{
    return 0;
}

int storage_flight_plan_reset(void)
{
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

static int storage_set_payload_data(int index, void* data, int payload)
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

static int storage_get_payload_data(int index, void* data, int payload)
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

static int storage_delete_all_memory_sections()
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
        spn_fl512s_erase_block(storage_addresses_payloads[i]);
    }

    // Deleting Flight Plan Memory Sections
    for (int i = 0; i < SCH_SECTIONS_FOR_FP; i++)
    {
        LOGI(tag, "Deleting section in address %u\n", (unsigned int)storage_addresses_flight_plan[i]);
        spn_fl512s_erase_block(storage_addresses_flight_plan[i]);
    }

    return 0;
}

static int dummy_callback(void *data, int argc, char **argv, char **names)
{
    return 0;
}
