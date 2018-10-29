//
// Created by carlos on 22-08-17.
//

#include "data_storage.h"

static const char *tag = "data_storage";

static void *db = NULL;
char* fp_table = "flightPlan";

struct temp_data tempdata;
struct ads_data adsdata;
uint16_t data_map[ads_sensors+1];


static int dummy_callback(void *data, int argc, char **argv, char **names);

int storage_init(const char *file)
{
    /* Init RTC storage */
    fm33256b_init();

    /* Init FLASH NOR storage */
    spn_fl512s_init((unsigned int) 0);


    data_map[0] = (uint16_t) (sizeof(tempdata));
    data_map[1] = (uint16_t) (sizeof(adsdata));

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

int storage_show_table (void)
{
    return 0;
}

int storage_close(void)
{
    return 0;
}

int storage_set_payload_data(int index, void* data, int payload)
{
//    data32_t data;
//    data.data32 = (uint32_t)value;
//    uint16_t len = (uint16_t)(sizeof(data));


//    uint32_t addr, uint8_t *data, uint16_t le

    uint32_t add = (uint32_t) SCH_FLASH_INIT_MEMORY + (uint32_t)index;
//    int i;
//    for(i=0;i<payload; ++i)
//    {
//        printf("Adress is: %u", add);
//        add += (uint16_t) (data_map[i]*SCH_MAX_DATA_SIZE);
//    }
//    add += (uint16_t) (index*data_map[payload]);

//    LOGV(tag, "Writing 0x%X", (unsigned int)data.data32);
//    printf("Writing in addresss: %u \n", add);
//    printf("Writing values of size %u for payload %d \n", data_map[payload], payload);
//    spn_fl512s_write_data(add, (uint8_t*) data, data_map[payload]);
//    uint8_t  dat = 4;
    printf("Writing in addresss: %u \n", add);
    printf("Writing value %d \n", *(uint8_t*)data);
    spn_fl512s_erase_block(add);
    int ret = spn_fl512s_write_data(add, data, 1);
//    printf("Writing return: %d \n", ret);
    return 0;
}

int storage_get_payload_data(int index, void* data, int payload)
{
//    data32_t data;
//    uint16_t len = (uint16_t)(sizeof(uint32_t));
//    uint16_t add = (uint16_t)(index*len);

    uint32_t add= (uint32_t) SCH_FLASH_INIT_MEMORY + (uint32_t)index;
//    int i;
//    for(i=0;i<payload; ++i)
//    {
//        add += (uint16_t) (data_map[i]*SCH_MAX_DATA_SIZE);
//    }
//    add += (uint16_t) (index*data_map[payload]);

    printf("Reading in addresss: %u \n", add);
//    printf("Reading values of size %u \n", data_map[payload]);
//    spn_fl512s_read_data(add, (uint8_t *) data, data_map[payload]);
    uint8_t  dat;
    spn_fl512s_read_data(add, &dat, 1);
    printf("Reading value %d \n", dat);
//    LOGV(tag, "Read 0x%X", (unsigned int)data.data32);
    return 0;
}

static int dummy_callback(void *data, int argc, char **argv, char **names)
{
    return 0;
}
