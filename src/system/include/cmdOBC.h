/**
 * @file  cmdOBC.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @date 2018
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to the on board computer
 * (OBC) features.
 */

#ifndef CMD_OBC_H
#define CMD_OBC_H

#include "config.h"

#ifdef LINUX
    #include <signal.h>
#endif

#ifdef AVR32
    #include "led.h"
    #include "wdt.h"
    #include "avr32_reset_cause.h"
#endif

#ifdef NANOMIND
    #include "compiler.h"
    #include "led.h"
    #include "wdt.h"
    #include "dev/cpu.h"
    #include "gs_pwm.h"
    #include "gssb.h"
    #include "pwr_switch.h"
    #include "util/error.h"
    #include <conf_a3200.h>
    #include <lm70.h>
    #include <mpu3300.h>
    #include <hmc5843.h>
#endif

#ifdef ESP32
    #define BLINK_GPIO 5
    #include "driver/gpio.h"
#endif

#define GSSB_ARMED_NONE 0
#define GSSB_ARMED_AUTO 1
#define GSSB_ARMED_MANUAL 2

#include "repoCommand.h"
#include "os/os.h"

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
 * Change duty cycle of pwm in channel
 * params, so use this command carefully. @see man system
 * @warning only available in Nanomind
 *
 * @param fmt str. Parameters format: "%d %d"
 * @param params  str. Parameters as string <int> <int>,
 * @param nparams int. Number of parameters: 2
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int obc_set_pwm_duty(char* fmt, char* params, int nparams);

/**
 * Read OBC sensors, print the results and save the values to status repository.
 * For Nanomind A3200 reads the temperature, gyroscope and magnetometer on board.
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int obc_get_sensors(char *fmt, char *params, int nparams);

/**
 * This commands are related to inter-stage panels and only available for the
 * Nanomind A3200 with inter-stage panels using the GSSB interface and drivers.
 */
#ifdef NANOMIND

/**
 * Sets the state of the interstage power switch to enable/disable VCC and VCC2.
 * State 0 = disabled, 1 = enabled
 *
 * @param fmt str. Parameters format: "%d %d"
 * @param params  str. Parameters as string. "<vcc> <vcc2>"
 *  vcc:  0 off, 1 on
 *  vcc2: 0 off, 1 on
 * @param nparams int. Number of parameters: 2
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int gssb_pwr(char* fmt, char* params, int nparams);

/**
 * Set GSSB node to talk
 *
 * @param fmt str. Parameters format: "%d"
 * @param params  str. Parameters as string <device i2c address>,
 * @param nparams int. Number of parameters: 1
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int gssb_select_dev(char *fmt, char *params, int nparams);

/**
 * Set GSSB I2C addresss
 *
 * @param fmt str. Parameters format: "%d"
 * @param params  str. Parameters as string <address>,
 * @param nparams int. Number of parameters: 1
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int gssb_set_addr(char *fmt, char *params, int nparams);

/**
 * Get CSP debug information
 *
 * @param fmt str. Parameters format: ""
 * @param params  str. Parameters as string. Not used. ""
 * @param nparams int. Number of parameters: 0
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int gssb_ident(char *fmt, char *params, int nparams);

/**
 * Get current inter-stage panels settings
 *
 * @param fmt str. Parameters format: ""
 * @param params  str. Parameters as string. "". Not used
 * @param nparams int. Number of parameters: 0
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int gssb_istage_get_settings(char* fmt, char* params, int nparams);

/**
 * Set inter-stage panels settings. The settings are the following:
 *
 * 	knife_on_time: Burn time for first burn [ms]
 *	increment: How much to increment burn time for each retry [ms]
 *	deploy_delay: The burn process will run once after this cntr runs out [s]
 *	max_repeats:	Max number of retries [#]
 *	repeat_delay:	Time between retries [s]
 *	switch_polarity: Release sense switch polarity
 *	reboot_deploy_cnt: Wait this number of reboots before deploy in armed auto mode [#]
 *
 *	A reset is required to apply the settings
 *
 * @param fmt str. Parameters format: "%d &d %d %d %d %d %d"
 * @param params  str. Parameters as string.
 *      "<knife_on_time> <increment> <deploy_delay>
 *       <max_repeats> <repeat_delay> <switch_polarity>
 *       <reboot_deploy_cnt>"
 * @param nparams int. Number of parameters: 7
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 * @code
 *      istage_set_config 1500 500 2000 4 1 0 2
 * @endcode
 */
int gssb_istage_settings(char *fmt, char *params, int nparams);

/**
 * Reboot current inter-stage panel
 *
 * @param fmt str. Parameters format: ""
 * @param params  str. Parameters as string. Not used. ""
 * @param nparams int. Number of parameters: 0
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int gssb_istage_reboot(char *fmt, char *params, int nparams);

/**
 * Sends antenna deploy command to inter-stage panel
 *
 * @param fmt str. Parameters format: ""
 * @param params  str. Parameters as string. Not used. ""
 * @param nparams int. Number of parameters: 0
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int gssb_istage_deploy(char *fmt, char *params, int nparams);

/**
 * Unlock inter-stage panel settings
 *
 * @param fmt str. Parameters format: ""
 * @param params  str. Parameters as string. Not used. ""
 * @param nparams int. Number of parameters: 0
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int gssb_unlock(char *fmt, char *params, int nparams);

/**
 * Sends arm command to interstage, 1 for auto deploy and 0 for not armed
 *
 * @param fmt str. Parameters format: "%d"
 * @param params  str. Parameters as string. "<armed>"
 *  1: auto deploy
 *  0: not armed
 * @param nparams int. Number of parameters: 1
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int gssb_istage_arm(char *fmt, char *params, int nparams);

/**
 * Read panel voltage (coarse sun sensor) and temperature
 *
 * @param fmt str. Parameters format: ""
 * @param params  str. Parameters as string. Not used. ""
 * @param nparams int. Number of parameters: 0
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int gssb_istage_sensors(char *fmt, char *params, int nparams);

/**
 * Request interstage status
 *
 * @param fmt str. Parameters format: ""
 * @param params  str. Parameters as string. Not used. ""
 * @param nparams int. Number of parameters: 0
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int gssb_istage_status(char *fmt, char *params, int nparams);

/**
 * Update deployment related status variables:
 *  dat_dep_ant_deployed: antennas release status
 *
 * @param fmt str. Parameters format: ""
 * @param params  str. Parameters as string. Not used. ""
 * @param nparams int. Number of parameters: 0
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int gssb_istage_update_status(char *fmt, char *params, int nparams);

/**
 * Select an interstage panel, set burn parameters and send deploy command.
 * Parameters:
 *  - addr: device address (16, 17, 18, 19)
 *  - knife_on: burn time in seconds [0, 8]
 *  - knife_off: rest time in seconds [0, 10]
 *  - repeats: number of repeats [0, 10]
 *
 *  Example
 *  - console: istage_release 16 2 1 5
 *  - command: gssb_istage_antenna_release("%d %d %d %d", "16 2 1 5", 4)
 *
 *
 * @param fmt str. Parameters format: "%d %d %d %d"
 * @param params  str. Parameters as string "<addr> <knife_on> <knife_off> <repeats>
 * @param nparams int. Number of parameters: 4
 * @return CMD_OK if executed correctly or CMD_FAIL in case of errors
 */
int gssb_istage_antenna_release(char *fmt, char *params, int nparams);

#endif //NANOMIND

#endif /* CMD_OBC_H */
