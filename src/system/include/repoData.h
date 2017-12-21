/**
 * @file  commandRepoitory.h
 * @author Carlos Gonzalez C - carlgonz@ug.uchile.cl
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @date 2017
 * @copyright GNU GPL v3
 *
 * This header is an API to Status Repository and Data Repository:
 *  Status Repository:
 *      dat_system_var
 *
 *  Data Repository:
 *      DAT_FligthPlanBuff
 *      DAT_TeleCmdBuff
 *      DAT_PayloadVar
 *      DAT_GnrlPurpBuff
 */

#ifndef DATA_REPO_H
#define DATA_REPO_H

#include "osSemphr.h"

#include "config.h"
#include "globals.h"
#include "utils.h"

#include "data_storage.h"


#define DAT_OBC_OPMODE_NORMAL   (0) ///< Normal operation
#define DAT_OBC_OPMODE_WARN     (1) ///< Fail safe operation
#define DAT_OBC_OPMODE_FAIL     (2) ///< Generalized fail operation

#define DAT_REPO_SYSTEM "dat_system"

/**
 * System level status variables
 */
typedef enum dat_system{
    //OBC => (C&DH subsystem)
    dat_obc_opmode=0,           ///< General operation mode
    dat_obc_lastResetSource,    ///< Last reset source
    dat_obc_hours_alive,         ///< Hours since first boot
    dat_obc_hours_without_reset, ///< Hours since last reset
    dat_obc_reset_counter,       ///< Number of reset since first boot

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

    dat_system_last_var         ///< Dummy element
}dat_system_t;

/**
 * Initializes data repositories including buffers and mutexes
 */
void dat_repo_init(void);

/**
 * Cleanup and close data repository resources
 */
void dat_repo_close(void);

/**
 * Sets a status variable value
 *
 * @param index dat_system_t. Variable to set @sa dat_system_t
 * @param value Int. Value to set
 */
void dat_set_system_var(dat_system_t index, int value);

/**
 * Returns a status variable's value
 *
 * @param index Variable to set @sa DAT_CubesatVar
 * @return Variable value
 */
int dat_get_system_var(dat_system_t index);

/**
 * TODO: Docummentation required
 * @param elapsed_sec
 * @param command
 * @param args
 * @param repeat
 * @param table
 * @param periodical
 * @return
 */
int dat_get_fp(int elapsed_sec, char** command, char** args, int** repeat, char* table, int** periodical);



/* The following is an API to interface with the repoData cubesat fligthPlan */
//typedef enum _DAT_FligthPlanBuff{
//    dat_fpb_last_one
//}DAT_FligthPlanBuff;
//
//DispCmd dat_getFlightPlan(unsigned int index);
//int dat_setFlightPlan_cmd(unsigned int index, unsigned int cmdID);
//int dat_setFlightPlan_param(unsigned int index, int param);
//int dat_onResetFlightPlan(void);
//void dat_erase_FlightPlanBuff(void);



#endif // DATA_REPO_H

