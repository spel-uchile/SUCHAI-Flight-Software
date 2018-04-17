//
// Created by carlos on 22-08-17.
//

#include "data_storage.h"

static const char *tag = "data_storage";

static void *db = NULL;
char* fp_table = "flightPlan";

static int dummy_callback(void *data, int argc, char **argv, char **names);

int storage_init(const char *file)
{
    fm33256b_init();

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

static int dummy_callback(void *data, int argc, char **argv, char **names)
{
    return 0;
}
