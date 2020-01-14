/**
 * @file  cmdGSSB.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2019
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to the GSSB
 */
#ifndef CMD_GSSB_H
#define CMD_GSSB_H

#include "drivers.h"
#include "config.h"
#include "globals.h"
#include "utils.h"

#include "osDelay.h"
#include "repoCommand.h"

/**
 * Register GSSB related commands
 */
void cmd_gssb_init(void);

/* TODO: Add documentation */
int gssb_pwr(char *fmt, char *params, int nparams);
int gssb_select_addr(char *fmt, char *params, int nparams);
int gssb_bus_scan(char *fmt, char *params, int nparams);
int gssb_read_sunsensor(char *fmt, char *params, int nparams);
int gssb_get_temp(char *fmt, char *params, int nparams);
int gssb_sunsensor_conf(char *fmt, char *params, int nparams);
int gssb_sunsensor_conf_save(char *fmt, char *params, int nparams);
int gssb_set_i2c_addr(char *fmt, char *params, int nparams);
int gssb_commit_i2c_addr(char *fmt, char *params, int nparams);
int gssb_get_version(char *fmt, char *params, int nparams);
int gssb_get_uuid(char *fmt, char *params, int nparams);
int gssb_get_model(char *fmt, char *params, int nparams);
int gssb_interstage_temp(char *fmt, char *params, int nparams);
int gssb_msp_outside_temp(char *fmt, char *params, int nparams);
int gssb_msp_outside_temp_calibrate(char *fmt, char *params, int nparams);
int gssb_internal_temp(char *fmt, char *params, int nparams);
int gssb_interstage_burn(char *fmt, char *params, int nparams);
int gssb_common_sun_voltage(char *fmt, char *params, int nparams);
int gssb_interstage_get_burn_settings(char *fmt, char *params, int nparams);
int gssb_interstage_set_burn_settings(char *fmt, char *params, int nparams);
int gssb_interstage_arm(char *fmt, char *params, int nparams);
int gssb_interstage_state(char *fmt, char *params, int nparams);
int gssb_interstage_settings_unlock(char *fmt, char *params, int nparams);
int gssb_soft_reset(char *fmt, char *params, int nparams);
int gssb_interstage_get_status(char *fmt, char *params, int nparams);

int gssb_update_status(char *fmt, char *params, int nparams);
int gssb_antenna_release(char *fmt, char *params, int nparams);

//NOT REGISTERED AS COMMANDS
int gssb_ar6_burn(char *fmt, char *params, int nparams);
int gssb_ar6_get_status(char *fmt, char *params, int nparams);
int gssb_common_burn_channel(char *fmt, char *params, int nparams);
int gssb_common_stop_burn(char *fmt, char *params, int nparams);
int gssb_ant6_get_status_all_channels(char *fmt, char *params, int nparams);
int gssb_i4_get_status_all_channels(char *fmt, char *params, int nparams);
int gssb_common_reset_count(char *fmt, char *params, int nparams);
int gssb_common_backup_settings(char *fmt, char *params, int nparams);

#endif //CMD_GSSB_H
