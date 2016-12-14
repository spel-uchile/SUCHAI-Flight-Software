/**
 * @file  commandRepoitory.h
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @author Carlos Gonzalez C - carlgonz@ug.uchile.cl
 * @date 2012
 * @copyright GNU GPL v3
 *
 * This header is an API to Status Repository and Data Repository:
 *  Status Repository:
 *      DAT_Cubesatvar
 *
 *  Data Repository:
 *      DAT_FligthPlanBuff
 *      DAT_TeleCmdBuff
 *      DAT_PayloadVar
 *      DAT_GnrlPurpBuff
 */

#ifndef DATA_REPO_H
#define DATA_REPO_H

#include "../../OS/include/osSemphr.h"

#include "../../SUCHAI_config.h"


#define DAT_OBC_OPMODE_NORMAL   (0) ///< Normal operation
#define DAT_OBC_OPMODE_WARN     (1) ///< Fail safe operation
#define DAT_OBC_OPMODE_FAIL     (2) ///< Generalized fail operation

/**
 * Cubesat's State Variables
 */
typedef enum{
    //OBC => (C&DH subsystem)
    dat_obc_opMode=0,           ///< General operation mode
    dat_obc_lastResetSource,    ///< Last reset source
    dat_obc_hoursAlive,         ///< Hours since first boot
    dat_obc_hoursWithoutReset,  ///< Hours since last reset
    dat_obc_resetCounter,       ///< Number of reset since first boot

    //DEP => (C&DH subsystem)
    dat_dep_ant_deployed,       ///< Is antena deployed?

    //RTC => (C&DH subsystem)
    dat_rtc_year,               ///< System current date and clock
    dat_rtc_month,              ///< System current date and clock
    dat_rtc_week_day,           ///< System current date and clock
    dat_rtc_day_number,         ///< System current date and clock
    dat_rtc_hours,              ///< System current date and clock
    dat_rtc_minutes,            ///< System current date and clock
    dat_rtc_seconds,            ///< System current date and clock

    //EPS => (Energy subsystem)
    dat_eps_status,             ///< Energy system general status
    dat_eps_soc,                ///< Current availible energy level

    //TRX => (Communication subsystem)
    dat_trx_opmode,             ///< TRX Operation mode
    dat_trx_beacon_pwr,         ///< TRX Beacon power level
    dat_trx_telemetry_pwr,      ///< TRX Telemetry Power
    dat_trx_count_tm,           ///< number of sended TM
    dat_trx_count_tc,           ///< number of received TC
    dat_trx_lastcmd_day,        ///< day of the last received tc (since 1/1/00)

    // Cmd buffer control
    dat_trx_newTcFrame,         ///< Exist any unprocessed TC?
    dat_trx_newCmdBuff,         ///< Exist unprocessed CMD in the internal buffer?

    //FLIGHT PLAN
    dat_fpl_index,              ///< Flight plan entry to be edited

    /* Add custom status variables here */

    dat_cubesatVar_last_one     ///< Dummy element
}DAT_CubesatVar;


void dat_setCubesatVar(DAT_CubesatVar indxVar, int value);
int dat_getCubesatVar(DAT_CubesatVar indxVar);
void dat_onResetCubesatVar(void);


/* The following is an API to interface with the repoData cubesat fligthPlan */

typedef enum _DAT_FligthPlanBuff{
    dat_fpb_last_one
}DAT_FligthPlanBuff;

//DispCmd dat_getFlightPlan(unsigned int index);
//int dat_setFlightPlan_cmd(unsigned int index, unsigned int cmdID);
//int dat_setFlightPlan_param(unsigned int index, int param);
//int dat_onResetFlightPlan(void);
//void dat_erase_FlightPlanBuff(void);

int drp_update_all_status_var(void* param);


#endif // DATA_REPO_H

