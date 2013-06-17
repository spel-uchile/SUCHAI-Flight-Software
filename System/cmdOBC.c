#include "cmdOBC.h"

cmdFunction obc_Function[OBC_NCMD];
int ppc_sysReq[OBC_NCMD];

/**
 * This function registers the list of command in the system, initializing the
 * functions array. This function must be called at every system start up.
 */
void obc_onResetCmdOBC(void)
{
    obc_Function[(unsigned char)obc_id_reset] = obc_reset;
    ppc_sysReq[(unsigned char)obc_id_reset]  = CMD_SYSREQ_MIN;
    
    obc_Function[(unsigned char)obc_id_get_rtos_memory] = obc_get_rtos_memory;
    ppc_sysReq[(unsigned char)obc_id_get_rtos_memory]  = CMD_SYSREQ_MIN;
}

/**
 * Perform a microcontroller software reset
 * 
 * @param param Not used
 * @return 1, but function never returns
 */
int obc_reset(void* param)
{
    printf("Resetting system NOW!!\n");

    OBC_SYS_RESET();

    /* Never get here */
    return 1;
}

/**
 * Performs debug taks over current RTOS. Get rtos memory usage in bytes
 *
 * @param param Not used
 * @return Availible heap memory in bytes
 */
int obc_get_rtos_memory(void *param)
{
    size_t mem_heap = xPortGetFreeHeapSize();
    printf("Free RTOS memory: %d", mem_heap);

    return mem_heap;
}
