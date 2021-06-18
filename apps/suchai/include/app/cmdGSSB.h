/**
 * @file  cmdGSSB.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to the GSSB
 */
#ifndef CMD_GSSB_H
#define CMD_GSSB_H

#include "drivers.h"
#include "config.h"
#include "globals.h"
#include "log_utils.h"

#include "osDelay.h"
#include "repoCommand.h"

/**
 * Register GSSB related commands
 */
void cmd_gssb_init(void);

/**
 * Enable or disable GSSB switches.
 * @param fmt Str. Parameters format "%d %d"
 * @param param Str. Parameters as string: "<vcc_on> <vcc2_on>"
 * @param nparams Int. Number of parameters 2
 * @return CMD_OK if executed correctly or CMD_ERROR in case of errors
 */
int gssb_pwr(char *fmt, char *params, int nparams);

/**
 * Select I2C device.
 * @param fmt Str. Parameters format "%i"
 * @param param Str. Parameters as string: "<addr>"
 * @param nparams Int. Number of parameters 1
 * @return CMD_OK if executed correctly, CMD_ERROR in case of errors
 */
int gssb_select_addr(char *fmt, char *params, int nparams);

/**
 * Scan bus.
   Function to scan the I2C bus for GSSB devices. If array devices it contains 0 at index i, device addr i exists on bus.
 * @param fmt Str. Parameters format "%i %i"
 * @param param Str. Parameters as string: "<start> <end>"
 * @param nparams Int. Number of parameters 2
 * @return CMD_OK if executed correctly or CMD_ERROR in case of errors
 */
int gssb_bus_scan(char *fmt, char *params, int nparams);

/**
 * Read sun sensor.
   Reads the sun sensors four measurements (4 * uint16).
 * @param fmt Str. ""
 * @param param Str. ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int gssb_read_sunsensor(char *fmt, char *params, int nparams);

/**
 * Get sun sensor temperature.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int gssb_get_temp(char *fmt, char *params, int nparams);

/**
 * Set sun sensor configuration.
 * @param fmt Str. Parameters format "%d"
 * @param param Str. Parameters as string: "<conf>"
 * @param nparams Int. Number of parameters 1
 * @return CMD_OK if executed correctly, CMD_ERROR in case of errors or CMD_ERROR_SYNTAX in case of failures
 */
int gssb_sunsensor_conf(char *fmt, char *params, int nparams);

/**
 * Save sun sensor conf.
   Makes the sun sensors current configuration non volatile.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int gssb_sunsensor_conf_save(char *fmt, char *params, int nparams);

/**
 * Set I2C address.
   Gives a new I2C address to device.
 * @param fmt Str. Parameters format "%i"
 * @param param Str. Parameters as string: "<new_i2c_addr>"
 * @param nparams Int. Number of parameters 1
 * @return CMD_OK if executed correctly, CMD_ERROR in case of errors or CMD_ERROR_SYNTAX in case of failures
 */
int gssb_set_i2c_addr(char *fmt, char *params, int nparams);

/**
 * Commit address.
   Stores the newly set address in non volatile memory.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int gssb_commit_i2c_addr(char *fmt, char *params, int nparams);

/**
 * Get software version.
   Gets the 20 character long version tag from the device.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int gssb_get_version(char *fmt, char *params, int nparams);

/**
 * Get UUID.
   Gets the 4 byte long UUID from the device.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int gssb_get_uuid(char *fmt, char *params, int nparams);

/**
 * Get model.
   Gets the specific GSSB device type at this address.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int gssb_get_model(char *fmt, char *params, int nparams);

/**
 * Get istage temperature.
   Gets the interstages external temperature sensor. Temp equals -256 if no sensor is connected.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int gssb_interstage_temp(char *fmt, char *params, int nparams);

/**
 * Get msp temperature.
   Gets the msp external temperature on solar panel.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int gssb_msp_outside_temp(char *fmt, char *params, int nparams);

/**
 * TODO: Add description.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERROR in case of errors or CMD_ERROR_SYNTAX in case of failures
 */
int gssb_msp_outside_temp_calibrate(char *fmt, char *params, int nparams);

/**
 * Get istage internal MCU temperature.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int gssb_internal_temp(char *fmt, char *params, int nparams);

/**
 * Manual burn istage.
   Makes the interstages burn, when it is armed for manual deploy.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int gssb_interstage_burn(char *fmt, char *params, int nparams);

/**
 * Get istage sun voltage.
   Gets the interstages external sun sensor reading.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int gssb_common_sun_voltage(char *fmt, char *params, int nparams);

/**
 * Get istage burn settings.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERROR in case of errors or CMD_ERROR_SYNTAX in case of failures
 */
int gssb_interstage_get_burn_settings(char *fmt, char *params, int nparams);

/**
 * Set istage burn settings.
 * @param fmt Str. Parameters format "%d %d %d %d %d %d %d"
 * @param param Str. Parameters as string: "<std_time> <increment_ms> <short_cnt_down> <max_repeat> <rep_time_s> <switch_polarity> <reboot_deploy_cnt>"
 * @param nparams Int. Number of parameters 7
 * @return CMD_OK if executed correctly, CMD_ERROR in case of errors or CMD_ERROR_SYNTAX in case of failures
 */
int gssb_interstage_set_burn_settings(char *fmt, char *params, int nparams);

/**
 * Set istage arm auto mode.
   Arms or disarms the interstage for autodeploy.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERROR in case of errors or CMD_ERROR_SYNTAX in case of failures
 */
int gssb_interstage_arm(char *fmt, char *params, int nparams);

/**
 * Set istage arm manual mode.
   Arms or disarms the interstage for manual deploy.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERROR in case of errors or CMD_ERROR_SYNTAX in case of failures
 */
int gssb_interstage_state(char *fmt, char *params, int nparams);

/**
 * Unlock istage settings.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERROR in case of errors or CMD_ERROR_SYNTAX in case of failures
 */
int gssb_interstage_settings_unlock(char *fmt, char *params, int nparams);

/**
 * Software reset.
   Resets GSSB MCU from software.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int gssb_soft_reset(char *fmt, char *params, int nparams);

/**
 * Get istage status.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int gssb_interstage_get_status(char *fmt, char *params, int nparams);


/**
 * Update GSSB status.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int gssb_update_status(char *fmt, char *params, int nparams);

/**
 * TODO: Add description.
 * @param fmt Str. Parameters format "%d %d %d %d"
 * @param param Str. Parameters as string: "<addr> <knife_on> <knife_off> <repeats>"
 * @param nparams Int. Number of parameters 0
 * @return CMD_OK if executed correctly, CMD_ERROR in case of errors or CMD_ERROR_SYNTAX in case of failures
 */
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
