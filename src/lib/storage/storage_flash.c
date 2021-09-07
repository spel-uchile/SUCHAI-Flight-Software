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
#include "suchai/log_utils.h"
#ifdef NANOMIND
#include "app/drivers/drivers.h"
#endif
static const char *tag = "data_storage";

char* fp_table = "flightPlan";

/**
 * PLEASE READ THIS!
 * Writes must be aligned to S25FL512S pages of 512 bytes.
 * Please refer to the device datasheet (https://www.cypress.com/file/177971/download)
 * page 94, section 9.5.2 to understand flash write limitations.
 * In the S25FL512S, if a write operation exceeds the page address boundaries, it continues
 * writing to the beginning of the page. Yes, it makes no sense. Yes, you must read the datasheet.
 *
 * So, we fixed the flight plan entry to fit in a 512 byte page
 * and we check small writes do not exceed a page address boundary.
 */
#define ST_CMD_MAX_STR_PARAMS        (248)  ///< Limit for the parameters length
#define ST_CMD_MAX_STR_NAME          (248)  ///< Limit for the length of the name of a command
#define ST_PAGE_SIZE                 (512)  ///< Flash page size in bytes
///< Flight plan entry buffer (aligned to 512 bytes pages)
typedef struct fp_container_s{
    int32_t unixtime;               ///< Unix-time, sets when the command should next execute
    int32_t executions;             ///< Amount of times the command will be executed per periodic cycle
    int32_t periodical;             ///< Period of time between executions
    int32_t node;                   ///< Node to execute the command
    char cmd[ST_CMD_MAX_STR_PARAMS]; ///< Command to execute
    char args[ST_CMD_MAX_STR_NAME]; ///< Command's arguments
} fp_container_t;

///< Flight plan entry address cache structure (aligned to 512 bytes pages)
typedef struct fp_addr_s{
    uint32_t addr;
    int32_t unixtime;
} fp_addr_t;

static uint32_t* st_flightplan_addr = NULL;
static int st_flightplan_base_addr = 0;
#define FP_CONTAINER_SIZE (sizeof(fp_container_t))
static int st_flightplan_sections = (SCH_FP_MAX_ENTRIES * FP_CONTAINER_SIZE) / SCH_SIZE_PER_SECTION + 1;
static int st_flightplan_entries = 0;
static int commands_per_section = SCH_SIZE_PER_SECTION / FP_CONTAINER_SIZE;
static int st_flightplan_tlb_base_addr = 0;
static fp_addr_t st_flightplan_tlb[SCH_FP_MAX_ENTRIES+1];  ///< FP address translation look-up table (Index=0 is TLB metadata)
static int storage_flight_load_tlb(void);

///< Payloads storage address buffer
static uint32_t* st_payload_addr = NULL;
static int st_payload_sections = 0;
static int st_payloads_entries = 0;
static int st_payload_base_addr = 0; // TODO: Check this, cannot be equal to st_flightplan_base_addr

static int storage_is_open = 0;

#if !defined(NANOMIND) && !defined(NDEBUG)
static char flash[256][SCH_SIZE_PER_SECTION];
static char *flash_p = &(flash[0][0]);
static char fram[32*1024];
static char *fram_p = &(fram[0]);
#endif


static int storage_read_flash(uint8_t partition, uint32_t addr, uint8_t *data, uint16_t len)
{
#if defined(NANOMIND)
    int rc = spn_fl512s_read_data(partition, addr, data, len);
    return rc == GS_OK ? SCH_ST_OK : SCH_ST_ERROR;
#elif !defined(NDEBUG) // Debug mode
    memcpy(data, flash_p+addr, len);
    return SCH_ST_OK;
#else
#error STORAGE FLASH ARCHITECTURE NOT VALID
#endif
}

static int storage_write_flash(uint8_t partition, uint32_t addr, const uint8_t *data, uint16_t len){
#if defined(NANOMIND)
    int rc = spn_fl512s_write_data(partition, addr, data, len);
    return rc == GS_OK ? SCH_ST_OK : SCH_ST_ERROR;
#elif !defined(NDEBUG) // Debug mode
    memcpy(flash_p+addr, data, len);
    return SCH_ST_OK;
#else
#error STORAGE FLASH ARCHITECTURE NOT VALID
#endif
}

static int storage_erase_flash(uint8_t partition, uint32_t addr)
{
#if defined(NANOMIND)
    // NOTE: Deleting a section (256 kB) takes about 520 ms
    int rc = spn_fl512s_erase_block(partition, addr);
    return rc == GS_OK ? SCH_ST_OK : SCH_ST_ERROR;
#elif !defined(NDEBUG) // Debug mode
    uint32_t section = addr/SCH_SIZE_PER_SECTION;
    addr = section*SCH_SIZE_PER_SECTION;
    memset(flash_p+addr, 0, SCH_SIZE_PER_SECTION);
    return SCH_ST_OK;
#else
#error STORAGE FLASH ARCHITECTURE NOT VALID
#endif
}

static int storage_write_fram(uint32_t add, void *value, size_t len)
{
#if defined(NANOMIND)
    int rc = gs_fm33256b_fram_write(0, add, value, len);
    return rc == GS_OK ? SCH_ST_OK : SCH_ST_ERROR;
#elif !defined(NDEBUG) // Debug mode
    memcpy(fram_p+add, value, len);
    return SCH_ST_OK;
#else
#error STORAGE FLASH ARCHITECTURE NOT VALID
#endif
}

static int storage_read_fram(uint32_t add, void *value, size_t len)
{
#if defined(NANOMIND)
    int rc = gs_fm33256b_fram_read(0, add, value, len);
    return rc == GS_OK ? SCH_ST_OK : SCH_ST_ERROR;
#elif !defined(NDEBUG) // Debug mode
    memcpy(value, fram_p+add, len);
    return SCH_ST_OK;
#else
#error STORAGE FLASH ARCHITECTURE NOT VALID
#endif

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

/**
 *                  FLASH
 *  Section |        Usage (256 KiB)    |   Addr            |
 *  --------------------------------------------------------|
 *      0   |       FP TLB              |      0 -> 262143  |
 *      1   |       FP TABLE            | 262144 -> 524287  |
 *      2   |       PAYLOAD 1           | 524288 -> ...     |
 *      .   |           ...             |                   |
 *      N   |       PAYLOAD 1           |                   |
 *    N+1   |       PAYLOAD 2           |                   |
 *      .   |           ...             |                   |
 *     2N   |       PAYLOAD 2           |                   |
 *     ...  |           ...             |                   |
 *  --------------------------------------------------------|
 */
    assert(sizeof(fp_container_t) == 512);
    st_flightplan_tlb_base_addr = SCH_FLASH_INIT_MEMORY;
    st_flightplan_base_addr = st_flightplan_tlb_base_addr + SCH_SIZE_PER_SECTION;
    st_payload_base_addr = st_flightplan_base_addr + SCH_SIZE_PER_SECTION*st_flightplan_sections;
    storage_is_open = 1;
    return SCH_ST_OK;
}

int storage_close(void)
{
    storage_is_open = 0;

    if(st_flightplan_addr != NULL) {free(st_flightplan_addr); st_flightplan_addr = NULL;}
    st_flightplan_tlb_base_addr = 0;
    st_flightplan_base_addr = 0;
    st_flightplan_sections = (SCH_FP_MAX_ENTRIES * FP_CONTAINER_SIZE) / SCH_SIZE_PER_SECTION + 1;
    st_flightplan_entries = 0;

    if(st_payload_addr != NULL) {free(st_payload_addr); st_payload_addr = NULL;}
    st_payload_sections = 0;
    st_payloads_entries = 0;
    st_payload_base_addr = 0;

    return SCH_ST_OK;
}

int storage_table_status_init(char *table, int n_variables, int drop)
{
    if(!storage_is_open)
        return SCH_ST_ERROR;
    return SCH_ST_OK;
}

int storage_table_flight_plan_init(char *table, int n_entries, int drop)
{
    if(!storage_is_open)
        return SCH_ST_ERROR;

    if(st_flightplan_addr != NULL) {
        // Table initialized, but want to drop -> reset table
        if (drop)
        {
            free(st_flightplan_addr);
            storage_flight_plan_reset();
        }
        // Table is already initialized?! -> error
        else return SCH_ST_ERROR;
    }

    // Save the sections' addresses reserved for flight plan storage
    st_flightplan_entries = n_entries;
    st_flightplan_addr = malloc(st_flightplan_sections * sizeof(uint32_t));
    LOGD(tag, "Flight plan sections: %d", st_flightplan_sections);
    for (int i = 0; i < st_flightplan_sections; i++) {
        st_flightplan_addr[i] = st_flightplan_base_addr + i * SCH_SIZE_PER_SECTION;
        LOGD(tag, "FP section[%d]=%#X", i, st_flightplan_addr[i]);
    }

    // Initialize FP translation table
    memset(st_flightplan_tlb, -1, sizeof(st_flightplan_tlb));
    storage_flight_load_tlb();

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
    st_payloads_entries = n_entries;
    st_payload_sections = SCH_SECTIONS_PER_PAYLOAD * n_entries;
    st_payload_addr = malloc(st_payload_sections * sizeof(uint32_t));

    LOGD(tag, "Payload sections: %d", st_payload_sections);
    for (int i = 0; i < st_payload_sections; i++) {
        st_payload_addr[i] = st_payload_base_addr + i * SCH_SIZE_PER_SECTION;
        LOGD(tag, "Payload section[%d]=%#X", i, st_payload_addr[i]);
    }

    return 0;
}

/****** STATUS VARIABLES FUNCTIONS *******/
int storage_status_get_value_idx(uint32_t index, value32_t *value, char *table)
{
    uint16_t add = (uint16_t)(index*sizeof(value32_t));
    int rc = storage_read_fram(add, value, sizeof(value32_t));
    return rc;
}

int storage_status_set_value_idx(int index, value32_t value, char *table)
{
    uint16_t add = (uint16_t)(index*sizeof(value32_t));
    int rc = storage_write_fram(add, &value, sizeof(value32_t));
    return rc;
}

/****** FLIGHT PLAN VARIABLES FUNCTIONS *******/
static int storage_flight_load_tlb(void)
{
    return storage_read_flash(0, st_flightplan_tlb_base_addr, (uint8_t *)st_flightplan_tlb, sizeof(st_flightplan_tlb));
}

static int storage_flight_dump_tlb(void)
{
    assert(ST_PAGE_SIZE%sizeof(fp_addr_t) == 0); // Write aligned to page size
    int rc = storage_erase_flash(0, st_flightplan_tlb_base_addr);
    if(rc == SCH_ST_OK)
    {
        size_t total = sizeof(st_flightplan_tlb);
        for(int len=0; len < sizeof(st_flightplan_tlb); len += ST_PAGE_SIZE)
        {
            // Write up to one page
            size_t size = len + ST_PAGE_SIZE < total ? ST_PAGE_SIZE : total - len;
            rc += storage_write_flash(0, st_flightplan_tlb_base_addr, (uint8_t *) (&st_flightplan_tlb + len), size);
            LOGV(tag, "Dump TLB %d bytes (%d/%d)", size, len+size, total);
        }
    }
    return rc != SCH_ST_OK ? SCH_ST_ERROR : SCH_ST_OK;
}

int flight_plan_rebuild_tlb(void)
{
    LOGI(tag, "Rebuild FP TLB");
//    for(int i=0; i<SCH_FP_MAX_ENTRIES; i++)
//        LOGD(tag, "OLD_TLB[%i] = {.time=%d, .addr: %u}", st_flightplan_tlb[i].unixtime, st_flightplan_tlb[i].addr);

    int rc = 0;
    // Load all flight plan entries from flash
    fp_container_t *fp_entries = (fp_container_t *)malloc(commands_per_section*FP_CONTAINER_SIZE);
    storage_read_flash(0, st_flightplan_addr[0], (uint8_t *)fp_entries, commands_per_section*FP_CONTAINER_SIZE);

    // Erase flash
    LOGD(tag, "Deleting FP section at address %d", st_flightplan_addr[0]);
    storage_erase_flash(0, st_flightplan_addr[0]);
    st_flightplan_tlb[SCH_FP_MAX_ENTRIES].addr = 0;

    // Re-write only valid entries
    for(int index_tlb=0; index_tlb < SCH_FP_MAX_ENTRIES; index_tlb++)
    {
        if(st_flightplan_tlb[index_tlb].unixtime != ST_FP_NULL)
        {
            int old_index_flash = (st_flightplan_tlb[index_tlb].addr - st_flightplan_addr[0])/FP_CONTAINER_SIZE;
            fp_container_t fp_entry = fp_entries[old_index_flash];

            // Calculates memory address
            int new_index_flash = (int)st_flightplan_tlb[SCH_FP_MAX_ENTRIES].addr;
            int section_index = new_index_flash / commands_per_section;
            int index_in_section = new_index_flash % commands_per_section;
            uint32_t new_addr = st_flightplan_addr[section_index] + index_in_section * FP_CONTAINER_SIZE;

            // Update TLB
            st_flightplan_tlb[index_tlb].addr = new_addr;
            st_flightplan_tlb[SCH_FP_MAX_ENTRIES].addr = 1 + st_flightplan_tlb[SCH_FP_MAX_ENTRIES].addr;

            // Writes fp entry value
            rc += storage_write_flash(0, new_addr, (uint8_t *)&fp_entry, sizeof(fp_entry));
       }

    }

//    for(int i=0; i<SCH_FP_MAX_ENTRIES; i++)
//        LOGD(tag, "NEW_TLB[%i] = {.time=%d, .addr: %u}", st_flightplan_tlb[i].unixtime, st_flightplan_tlb[i].addr);

    free(fp_entries);
    rc += storage_flight_dump_tlb();
    return rc;
}

/**
 * Function for finding the storage index of a command based on it's timetodo field.
 *
 * IMPORTANT: Flight plan entries are saved as consecutive binary values using the following scheme:
 * timetodo(uint32_t) executions(uint32_t) periodical(uint32_t) name_length(uint32_t) args_length(uint32_t) name(char*ST_CMD_MAX_STR_NAME) args(char*ST_CMD_MAX_STR_PARAMS)
 *
 * The total size of each command is then stored in 'max_command_size'.
 *
 * @param timetodo Execution time of the command to find
 * @return The command's index, -1 if not found or error
 *
 */
static int flight_plan_find_index_tlb(int timetodo)
{
    // Search in the TLB and return flash addr
    for (int i = 0; i < SCH_FP_MAX_ENTRIES; i++)
    {
        if(st_flightplan_tlb[i].unixtime == timetodo)
            return i;
    }
    return -1;
}

static int flight_plan_find_index(int timetodo)
{
    // Calculates information on how the flight plan is stored
    // For every index
    for (int i = 0; i < st_flightplan_entries; i++)
    {
        // Translates the index into a flash section index and section address
        int section_index = i/commands_per_section;
        int index_in_section = i%commands_per_section;
        uint32_t addr = st_flightplan_addr[section_index] + index_in_section * FP_CONTAINER_SIZE;

        // Reads the entry's timetodo
        uint32_t found_time;
        storage_read_flash(0, addr, (uint8_t*)&found_time, sizeof(uint32_t));

        // If found, returns
        if (found_time == (uint32_t)timetodo)
            return i;
    }

    // If it couldn't find the entry
    return -1;
}

/**
 * Function for deleting a flight plan entry on a given index.
 *
 * @param index Storage index of the entry
 * @return SCH_ST_OK if OK, SCH_ST_ERROR if Error
 */
static int flight_plan_erase_index_tlb(int index)
{
    // TLB index = 0 is reserved for TLB metadata
    if (index < 0 || index >= st_flightplan_entries)
    {
        LOGW(tag, "Failed attempt at erasing flight plan entry index %d, out of bounds", index);
        return -1;
    }

    st_flightplan_tlb[index].unixtime = ST_FP_NULL;
    st_flightplan_tlb[index].addr = ST_FP_NULL;
    return storage_flight_dump_tlb();
}

int storage_flight_plan_set_st(fp_entry_t *row)
{
    if(row==NULL || !storage_is_open)
        return SCH_ST_ERROR;

    // Check if flash has more space
    if(st_flightplan_tlb[SCH_FP_MAX_ENTRIES].addr > commands_per_section)
        flight_plan_rebuild_tlb();

    // Finds an index with an empty entry in the TLB (unixtime == ST_FP_NULL (-1))
    int index_tlb = flight_plan_find_index_tlb(ST_FP_NULL);
    if (index_tlb == -1 || index_tlb >= st_flightplan_entries)
    {
        LOGE(tag, "Flight plan TLB has no space for another command!");
        return SCH_ST_ERROR;
    }

    // Calculates memory address
    int index_flash = (int)st_flightplan_tlb[SCH_FP_MAX_ENTRIES].addr;
    int section_index = index_flash / commands_per_section;
    int index_in_section = index_flash % commands_per_section;
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

    // Writes TLB
    st_flightplan_tlb[index_tlb].unixtime = row->unixtime;
    st_flightplan_tlb[index_tlb].addr = addr;
    st_flightplan_tlb[SCH_FP_MAX_ENTRIES].addr = 1 + st_flightplan_tlb[SCH_FP_MAX_ENTRIES].addr;
    int rc = storage_flight_dump_tlb();
    if(rc != SCH_ST_OK)
        return SCH_ST_ERROR;

    // Writes fp entry value
    rc = storage_write_flash(0, addr, (uint8_t *)&new_entry, sizeof(new_entry));
    LOGD(tag, "Writing time %d to index %d, at addr %d, section %d (%d)", new_entry.unixtime, index_flash, addr, section_index, rc);
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

    if(st_flightplan_tlb[index].unixtime == ST_FP_NULL)
        return SCH_ST_ERROR;

    uint32_t addr = st_flightplan_tlb[index].addr;
    // Read one entry
    fp_container_t fp_entry;
    int rc = storage_read_flash(0, addr, (uint8_t*)&fp_entry , sizeof(fp_container_t));
    LOGD(tag, "Read index %d, at addr %d, time %d (%d)", index, addr, fp_entry.unixtime, rc);
    if(rc != SCH_ST_OK)
        return SCH_ST_ERROR;

    //Copy values
    row->unixtime = fp_entry.unixtime;
    row->periodical = fp_entry.periodical;
    row->executions = fp_entry.executions;
    row->node = fp_entry.node;
    row->cmd = strdup(fp_entry.cmd);
    row->args = strdup(fp_entry.args);

    return SCH_ST_OK;

}

int storage_flight_plan_get_st(int timetodo, fp_entry_t *row)
{
    // Finds the table index for timetodo
    int index = flight_plan_find_index_tlb(timetodo);
    if(index < 0)
        return SCH_ST_ERROR;

    return storage_flight_plan_get_idx(index, row);
}

int storage_flight_plan_get_args(int timetodo, char* command, char* args, int* executions, int* period, int* node)
{
    // Finds the table index for timetodo
    int index = flight_plan_find_index_tlb(timetodo);
    if (command == NULL || args == NULL || executions == NULL || period == NULL || node == NULL)
        return SCH_ST_ERROR;

    fp_entry_t fp_entry;
    int rc = storage_flight_plan_get_st(timetodo, &fp_entry);
    if(rc != SCH_ST_OK)
        return SCH_ST_ERROR;

    *executions = fp_entry.executions;
    *period = fp_entry.periodical;
    *node = fp_entry.node;
    strncpy(command, fp_entry.cmd, ST_CMD_MAX_STR_NAME);
    strncpy(args, fp_entry.args, ST_CMD_MAX_STR_PARAMS);

    fp_entry_clear(&fp_entry);
    return SCH_ST_OK;
}

int storage_flight_plan_delete_row(int timetodo)
{
    // Finds the index to erase
    int index = flight_plan_find_index_tlb(timetodo);
    if (index < 0)
    {
        LOGW(tag, "Couldn't find command to erase %d", timetodo);
        return SCH_ST_ERROR;
    }

    // Erases the entry in index
    int rc = storage_flight_plan_delete_row_idx(index);
    return rc;
}

int storage_flight_plan_delete_row_idx(int index)
{
    if(index > st_flightplan_entries)
        return SCH_ST_ERROR;

    // Erases the entry in index
    int rc = flight_plan_erase_index_tlb(index);
    return rc;
}

int storage_flight_plan_reset(void)
{
    if(!storage_is_open || st_payload_addr == NULL) return SCH_ST_ERROR;

    // Deletes all flight plan memory sections
    int rc = SCH_ST_OK;
    for (int i = 0; i < st_flightplan_sections; i++)
    {
        int res = storage_erase_flash(0, st_flightplan_addr[i]);
        rc += res;
        LOGD(tag, "Deleting FP, section %d, addr %#X (rc=%d)", i, st_flightplan_addr[i], rc);
    }

    // Reset TLB
    memset(st_flightplan_tlb, ST_FP_NULL, sizeof(st_flightplan_tlb));
    st_flightplan_tlb[SCH_FP_MAX_ENTRIES].unixtime = 0;
    st_flightplan_tlb[SCH_FP_MAX_ENTRIES].addr = 0;
    rc += storage_flight_dump_tlb();

    return rc == SCH_ST_OK ? SCH_ST_OK : SCH_ST_ERROR;
}

/****** PAYLOAD STORAGE FUNCTIONS *******/
/**
 * Auxiliary function to check errors while writing address not aligned  with flash pages.
 */
int write_data_with_check(uint32_t addr, uint8_t * data, uint16_t size)
{
    uint32_t addr_aux = addr;
    if( (addr + size) / ST_PAGE_SIZE  > addr / ST_PAGE_SIZE ) {
        addr_aux  = (addr + size / ST_PAGE_SIZE) * ST_PAGE_SIZE;
    }
    int ret_write = storage_write_flash(0, addr_aux, data, size);
    if (ret_write != 0) {
        return SCH_ST_ERROR;
    }
    return SCH_ST_OK;
}

/**
 * Auxiliary function to check errors while reading address not aligned with flash pages.
 */
int read_data_with_check(uint32_t addr, uint8_t * data, uint16_t size) {

    uint32_t addr_aux = addr;
    if( (addr + size) / ST_PAGE_SIZE  > addr / ST_PAGE_SIZE ) {
        addr_aux  = (addr + size / ST_PAGE_SIZE) * ST_PAGE_SIZE;
    }

    int read_ret = storage_read_flash(0, addr_aux, data, size);
    if( read_ret != 0) {
        return SCH_ST_ERROR;
    }
    return SCH_ST_OK;
}

/**
 * Auxiliary function to calculate payload sample destination address
 */
int _get_sample_address(int payload, int index, size_t size, uint32_t *address)
{
    int samples_per_section = SCH_SIZE_PER_SECTION / size;
    int sample_section = index / samples_per_section;
    int index_in_section = index % samples_per_section;
    int section_index = payload*SCH_SECTIONS_PER_PAYLOAD + sample_section;
    if(sample_section > SCH_SECTIONS_PER_PAYLOAD)
        return SCH_ST_ERROR;
    if(section_index > st_payloads_entries * SCH_SIZE_PER_SECTION)
        return SCH_ST_ERROR;

    *address = st_payload_addr[section_index] + index_in_section * size;
    if(*address > SCH_FLASH_INIT_MEMORY + st_flightplan_base_addr + st_payloads_entries * SCH_SECTIONS_PER_PAYLOAD * SCH_SIZE_PER_SECTION)
        return SCH_ST_ERROR;

    return SCH_ST_OK;
}

int storage_payload_set_data(int payload, int index, void *data, data_map_t *schema)
{
    if(!storage_is_open || st_payload_addr == NULL) return SCH_ST_ERROR;
    if(data == NULL || schema == NULL) return SCH_ST_ERROR;

    uint32_t addr;
    int rc = _get_sample_address(payload, index, schema->size, &addr);
    if(rc != SCH_ST_OK)
        return SCH_ST_ERROR;

    LOGI(tag, "Writing in address: %u, %d bytes\n", addr, schema->size);
    //rc = storage_write_flash(0, addr, (uint8_t *)data, schema->size);
    rc = write_data_with_check(addr, (uint8_t *)data, schema->size);
    return rc != SCH_ST_ERROR ? SCH_ST_OK : SCH_ST_ERROR;
}

int storage_payload_get_data(int payload, int index, void *data, data_map_t *schema)
{
    if(!storage_is_open || st_payload_addr == NULL) return SCH_ST_ERROR;
    if(data == NULL || schema == NULL) return SCH_ST_ERROR;

    uint32_t addr;
    int rc = _get_sample_address(payload, index, schema->size, &addr);
    if(rc != SCH_ST_OK)
        return SCH_ST_ERROR;

    LOGI(tag, "Reading in address: %u, %d bytes\n", addr, schema->size);
    //rc = storage_read_flash(0, addr, (uint8_t *)data, schema->size);
    rc = read_data_with_check(addr, (uint8_t *)data, schema->size);
    return rc != SCH_ST_ERROR ? SCH_ST_OK : SCH_ST_ERROR;
}

int storage_payload_reset_table(int payload)
{
    if(!storage_is_open || st_payload_addr == NULL) return SCH_ST_ERROR;
    for(int i = 0; i<SCH_SECTIONS_PER_PAYLOAD; i++) {
        int section_idx = i + payload*SCH_SECTIONS_PER_PAYLOAD;
        uint32_t section_addr = st_payload_addr[section_idx];
        int rc = storage_erase_flash(0, section_addr);
        LOGD(tag, "Deleted payload %d, section %d, addr %#X (rc=%d)", payload, section_idx, section_addr, rc);
    }

    return SCH_ST_OK;
}

int storage_payload_reset(void)
{
    if(!storage_is_open || st_payload_addr == NULL || st_payloads_entries == 0) return SCH_ST_ERROR;
    for(int i=0; i<st_payloads_entries; i++)
        storage_payload_reset_table(i);
    return SCH_ST_OK;
}