/**
 * @file  cmdOBC.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to the on board computer
 * (OBC) features.
 */

#ifndef CMD_OBC_H
#define CMD_OBC_H

#include <signal.h>
#include <malloc.h>

#include "drivers.h"
#include "config.h"

#include "os/os.h"
#include "repoCommand.h"
#include "repoData.h"

/**
 * Register on board computer related (OBC) commands
 */
void cmd_obc_init(void);

/**
 * Print ID information such as software version, device id, name and node.
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int obc_ident(char* fmt, char* params, int nparams);

/**
 * Check if the OBC is working, usually blinking a led or printing a debug
 * message. In AVR32, NANOMIND and ESP32 blink the led indicated in the
 * parameter in LINUX just print a debug message.
 *
 * @param fmt Str. Parameters format "d"
 * @param params Str. Parameters as string "1"
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int obc_debug(char *fmt, char *params, int nparams);

/**
 * Reset the watchdog timer
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int obc_reset_wdt(char *fmt, char *params, int nparams);

/**
 * Reset (reboot) the system.
 *
 * @warning: In GNU/Linux if the params is the string "reboot" then "sudo reboot"
 * is executed to reboot the system, if params is null or any other value then
 * just exit the application
 *
 * @param fmt Str. Parameters format: "%s"
 * @param params Str. Parameters as string: "" or "reboot" in Linux to actually
 *                    reboot the system.
 * @param nparams Int. Number of parameters 0 or 1
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int obc_reset(char *fmt, char *params, int nparams);

/**
 * Debug system memory
 * @note: In POSIX just cat /proc/<id>/status file
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int obc_get_os_memory(char *fmt, char *params, int nparams);

/**
 * Set the system time only if is not running Linux
 *
 * @param fmt Str. Parameters format "%d"
 * @param params Str. Parameters as string "<time to set>"
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int obc_set_time(char* fmt, char* params,int nparams);

/**
 * Show the system time in the format given. Param equal 0 is ISO format,
 * param equal 1 is UNIX TIME format
 *
 * @param fmt Str. Parameters format "%d"
 * @param params Str. Parameters as string "<format>"
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int obc_get_time(char *fmt, char *params, int nparams);

/**
 * Execute a shell command by calling system(). No check are performed over the
 * params, so use this command carefully. @see man system
 * @warning only available in GNU/Linux systems
 *
 * @param fmt str. Parameters format: "%s"
 * @param params  str. Parameters, the system command to execute: eg: "echo hello world"
 * @param nparams int. Number of parameters: 1
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int obc_system(char* fmt, char* params, int nparams);

/**
 *
 * Function to test the console lecture on flight plan
 *
 * @param fmt Str. Parameters format "%d %s %d"
 * @param params Str. Parameters as string "<int> <string> <int>"
 * @param nparams Int. Number of parameters 3
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int test_fp(char* fmt, char* params,int nparams);

/**
 * Change <duty> cycle of pwm <channel>, so use this command carefully.
 * @warning only available in Nanomind
 * <duty> as percentage: 0-100, 10% ~ 1.0V RMS and 90% ~ 3.0V RMS
 * <channel> 0:X, 1:Y, 2:Z
 * 
 * @param fmt str. Parameters format: "%d %d"
 * @param params  str. Parameters as string <channel> <duty>,
 * @param nparams int. Number of parameters: 2
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int obc_set_pwm_duty(char* fmt, char* params, int nparams);

/**
 * Change pwm <channel> freq to <freq>, so use this command carefully.
 * @warning only available in Nanomind
 * <freq> as hz: 0.1-433.0 Hz
 * <channel> 0:X, 1:Y, 2:Z
 * 
 * @param fmt str. Parameters format: "%d %f"
 * @param params  str. Parameters as string <channel> <freq>,
 * @param nparams int. Number of parameters: 2
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int obc_set_pwm_freq(char* fmt, char* params, int nparams);

/**
 * Set PWM Switch on/off
 * @warning only available in Nanomind
 * <enable> 1:on, 0:off (>0:on, <=0: off)
 * 
 * @param fmt str. Parameters format: "%d"
 * @param params  str. Parameters as string <enable>,
 * @param nparams int. Number of parameters: 2
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int obc_pwm_pwr(char *fmt, char *params, int nparams);

/**
 * Read OBC sensors and save values as Temperatures and ADCS payloads data.
 * For Nanomind A3200 reads the temperature, gyroscope and magnetometer on board.
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int obc_get_sensors(char *fmt, char *params, int nparams);

/**
 * Read OBC sensors, print the results and save the values to status repository.
 * For Nanomind A3200 reads the temperature, gyroscope and magnetometer on board.
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int obc_update_status(char *fmt, char *params, int nparams);

/**
 * Log current TLE lines
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_FAIL in case of errors or
 * CMD_ERROR in case of parameter errors.
 */
int obc_get_tle(char *fmt, char *params, int nparams);

/**
 * Set TLE lines, once a at time. Call this command two times, one for each
 * line to set up the TLE. The command recognizes the line number by the first
 * parameter of each TLE line, the stores the full file.
 * @warning This command do not update the TLE structure. It only saves the TLE
 * lines. Normally it requires to also call the obc_update_tle command.
 *
 * Example 1, from code:
 * @code
 *  cmd_t *tle1 = cmd_get_str("obc_set_tle");
 *  cmd_add_params_str(tle1, "1 42788U 17036Z   20027.14771603  .00000881  00000-0  39896-4 0  9992");
 *  cmd_send(tle1);
 *
 *  cmd_t *tle2 = cmd_get_str("obc_set_tle");
 *  cmd_add_params_str(tle2, "2 42788  97.3234  85.2817 0012095 159.3521 200.8207 15.23399088144212");
 *  cmd_send(tle2);
 *
 *  cmd_t *tle_u = cmd_get_str("obc_update_tle");
    cmd_send(tle_u);
 * @endcode
 *
 * Example 2, from the serial console or telecommand
 * @code
 * obc_set_tle 1 42788U 17036Z   20027.14771603  .00000881  00000-0  39896-4 0  9992
 * obc_set_tle 2 42788  97.3234  85.2817 0012095 159.3521 200.8207 15.23399088144212
 * obc_update_tle
 * @endcode
 *
 * @param fmt Str. Parameters format "%d %n"
 * @param params Str. Parameters as string "<line (69 chars including line number)>"
 * @param nparams Int. Number of parameters 2
 * @return  CMD_OK if executed correctly, CMD_FAIL in case of errors or
 * CMD_ERROR in case of parameter errors.
 */
int obc_set_tle(char *fmt, char *params, int nparams);

/**
 * Update the TLE structure using the received TLE lines
 * @warning This command do not receive the TLE lines. It only updates the TLE
 * structure. Normally it requires to also call the obc_set_tle command.
 *
 * Example 1, from code:
 * @code
 *  cmd_t *tle1 = cmd_get_str("obc_set_tle");
 *  cmd_add_params_str(tle1, "1 42788U 17036Z   20027.14771603  .00000881  00000-0  39896-4 0  9992");
 *  cmd_send(tle1);
 *
 *  cmd_t *tle2 = cmd_get_str("obc_set_tle");
 *  cmd_add_params_str(tle2, "2 42788  97.3234  85.2817 0012095 159.3521 200.8207 15.23399088144212");
 *  cmd_send(tle2);
 *
 *  cmd_t *tle_u = cmd_get_str("obc_update_tle");
    cmd_send(tle_u);
 * @endcode
 *
 * Example 2, from the serial console or telecommand
 * @code
 * obc_set_tle 1 42788U 17036Z   20027.14771603  .00000881  00000-0  39896-4 0  9992
 * obc_set_tle 2 42788  97.3234  85.2817 0012095 159.3521 200.8207 15.23399088144212
 * obc_update_tle
 * @endcode
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_FAIL in case of errors or
 * CMD_ERROR in case of parameter errors.
 */
int obc_update_tle(char *fmt, char *params, int nparams);

/**
 * Propagate the TLE to the given datetime to update satellite position in ECI
 * reference. The result is stored in the system variables. The command receives
 * the datetime to propagate in unix timestamp format. If the parameter is 0
 * then uses the current date and time.
 * @warning This command do not receive the TLE lines, nor update the TLE.
 * It only propagates the TLE. Normally it requires to also call the
 * obc_set_tle and obc_update command.
 *
 * Example 1, from code:
 * @code
 *  // Set and update the TLE previously
 *  cmd_t *cmd_prop = cmd_get_str("obc_prop_tle");
 *  cmd_add_params_var(cmd_prop, dt, 1582643144);
 *  cmd_send(cmd_prop);
 * @endcode
 *
 * Example 2, from the serial console or telecommand
 * @code
 * // Prop. TLE to datetime
 * obc_prop_tle 1582643144
 * // Prop. TLE to current datetime
 * obc_prop_tle 0
 * @endcode
 *
 * @param fmt Str. Parameters format "%ld"
 * @param params Str. Parameters as string "<unix_timestamp | 0>"
 * @param nparams Int. Number of parameters 1
 * @return  CMD_OK if executed correctly, CMD_FAIL in case of errors or
 * CMD_ERROR in case of parameter errors.
 */
int obc_prop_tle(char *fmt, char *params, int nparams);

#endif /* CMD_OBC_H */
