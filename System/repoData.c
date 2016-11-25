/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2013, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
 *      Copyright 2013, Tomas Opazo Toro, tomas.opazo.t@gmail.com
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

#include "include/repoData.h"

extern osSemaphore repoDataSem;  // Mutex for status repository

#if SCH_STATUS_REPO_MODE == 0
    int DAT_CUBESAT_VAR_BUFF[dat_cubesatVar_last_one];
#endif

/**
 * Sets a status variable's value
 *
 * @param indxVar Variable to set @sa DAT_CubesatVar
 * @param value Value to set
 */
void dat_setCubesatVar(DAT_CubesatVar indxVar, int value)
{
    osSemaphoreTake(&repoDataSem, portMAX_DELAY);
    #if SCH_STATUSCH_STATUS_REPO_MODE == 0
        //Uses internal memory
        DAT_CUBESAT_VAR_BUFF[indxVar] = value;
    #else
        //Uses external memory
        #if __linux__
            printf("writeIntEEPROM1\n");
        #else
            writeIntEEPROM1( (unsigned char)indxVar, value);
        #endif
    #endif
    osSemaphoreGiven(&repoDataSem);
}

/**
 * Returns a status variable's value
 *
 * @param indxVar Variable to set @sa DAT_CubesatVar
 * @return Variable value
 */
int dat_getCubesatVar(DAT_CubesatVar indxVar)
{
    int value = 0;

    osSemaphoreTake(&repoDataSem, portMAX_DELAY);
    #if SCH_STATUSCH_STATUS_REPO_MODE == 0
        //Uses internal memory
        value = DAT_CUBESAT_VAR_BUFF[indxVar];
    #else
        //Uses external memory
        #if __linux__
            printf("readIntEEPROM1\n");
            value = 0;
        #else
            value = readIntEEPROM1( (unsigned char)indxVar );
        #endif
    #endif
    osSemaphoreGiven(&repoDataSem);

    return value;
}

/**
 * Initializes status repository
 */
void dat_onResetCubesatVar(void)
{
    #if (SCH_SSCH_STATUS_REPO_MODE == 0)
    {
        //Uses interal memory
        int i;
        for(i=0; i<dat_cubesatVar_last_one; i++)
        {
            
            dat_setCubesatVar(i,INT_MAX);
        }
    }
    #endif

    //Updates all dat_CubesatVar
    drp_update_all_status_var(NULL);
}

/**
 * This command updates all status variables
 * TODO: register with cmdOBC
 * 
 * @param param Not used
 * @return 1, success. 0, fail
 */
int drp_update_all_status_var(void* param)
{
    return 1;
}

///**
// * Esta funcion retorna un comando desde el fligh plan. Lee desde la SD el
// * i-esimo comando y su parametro.
// * @param index Index-esimo dato del fligh plan */
//DispCmd dat_getFlightPlan(unsigned int index){
//    DispCmd NewCmd;
//    NewCmd.cmdId = CMD_CMDNULL;
//    NewCmd.idOrig = CMD_IDORIG_TFLIGHTPLAN;
//    NewCmd.execCmd.param = 0;
//
//    if(index < SCH_FLIGHTPLAN_N_CMD)
//    {
//        int id, param;
//         // La organizacion de los datos en la SD es
//         // Primera mitad comandos, segunda mitad parametros
//
//        msd_getVar_256BlockExtMem(dat_FlightPlan_256Block, index, &id);
//        msd_getVar_256BlockExtMem(dat_FlightPlan_256Block, 0xFFFF - index, &param);
//
//        NewCmd.cmdId = id;
//        NewCmd.execCmd.param = 0;
//    }
//
//    return NewCmd;
//}
//
///**
// * Esta funcion escribe un comando en un determinado indice el flight plan
// * @param index Indice del f. plan que sera actualizado
// * @param cmdId Comando que sera escrito
// * @return 1, OK. 0, Fallo
// */
//int dat_setFlightPlan_cmd(unsigned int index, unsigned int cmdID){
//    if(index < SCH_FLIGHTPLAN_N_CMD)
//    {
//        msd_setVar_256BlockExtMem( dat_FlightPlan_256Block, index, cmdID);
//        return 1;
//    }
//    else
//        return 0;
//}
///**
// * Esta funcion escribe el parametro (asociado a un comando) de un determinado indice el flight plan
// * @param index Indice del f. plan que sera actualizado
// * @param param Parametro que sera escrito
// * @return 1, OK. 0, Fallo
// */
//int dat_setFlightPlan_param(unsigned int index, int param){
//    if(index < SCH_FLIGHTPLAN_N_CMD)
//    {
//        msd_setVar_256BlockExtMem( dat_FlightPlan_256Block, 0xFFFF-index, param);
//        return 1;
//    }
//    else
//        return 0;
//}
//
///**
// * Funcion con acciones a seguri sobre el Fligth Plan, luego de un Reset del PIC.
// * @return 1 si OK, 0 si fallo
// */
//int dat_onResetFlightPlan(void){
//    /* Do nothing onReset
//    unsigned long i; int cmdid;
//
//    for(i=0; i < SCH_FLIGHTPLAN_N_CMD; i++){
//        cmdid = CMD_CMDNULL;
//        msd_setVar_256BlockExtMem( DAT_FLIGHT_PLAN_BUFF_256BLOCK, i, cmdid);
//    }
//    */
//
//    return 1;
//}
//
///**
// * Borra todo el Fligth Plan
// */
//void dat_erase_FlightPlanBuff(void){
//    #if (SCH_repoData_VERBOSE>=1)
//        char ret[10];
//        con_printf("  dat_erase_FlightPlanBuff()..");
//        con_printf("    starting at block=");
//        sprintf (ret, "%d", (unsigned int)dat_FlightPlan_256Block);
//        con_printf(ret); con_printf("\r\n");
//    #endif
//
//    unsigned int i;
//    for(i=0;i<256;i++){
//        dat_memSD_BlockErase(dat_FlightPlan_256Block+i);
//        ClrWdt();
//    }
//}
