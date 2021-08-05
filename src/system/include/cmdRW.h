/**
 * @file  cmdRW.h
 * @author Gustavo Díaz H - g.hernan.diaz@ing.uchile.cl
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to reaction wheel driver
 * (RW) features.
 */

#ifndef SUCHAI_FLIGHT_SOFTWARE_CMDRW_H
#define SUCHAI_FLIGHT_SOFTWARE_CMDRW_H

#include "drivers.h"
#include "config.h"
#include "globals.h"
#include "log_utils.h"
#include "rwdrv10987.h"

//#include "config.h"
//#include "osDelay.h"

#ifdef LINUX
    #include <stdint.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
    #include <linux/i2c-dev.h>
    #include <errno.h>
    #include <unistd.h>
#endif

#include "repoCommand.h"
#include "os/os.h"

typedef enum upper_istage_cmd_enum
{
    IS2_START_SENSORS_TEMP=30,
    IS2_STOP_SENSORS_TEMP, //31
    IS2_GET_TEMP,  //32
    IS2_READ_SW_FACE,  //33
    IS2_BURN_FACE, //34
    IS2_SET_BURN,  //35
    IS2_SENSORS_TEMP_STATUS, //36
    I2S_SAMPLE_TEMP //37
}upper_istage_cmd_t;

#define ISTAGE_UPPER_ADD 0x40
#define ISTAGE_GSSB_TWI_HANDLER 2

/**
 * Register reaction whee related (rw) commands
 */
void cmd_rw_init(void);

/**
 * Get RW speed.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return 1 if executed correctly
 */
int rw_get_speed(char *fmt, char *params, int nparams);

/**
 * Get RW current.
 * @param fmt Str. Parameters format ""
 * @param param Str. Parameters as string: ""
 * @param nparams Int. Number of parameters 0
 * @return 1 if executed correctly
 */
int rw_get_current(char *fmt, char *params, int nparams);

/**
 * Set RW speed.
 * @param fmt Str. Parameters format "%d %d"
 * @param param Str. Parameters as string: "<motor_id> <speed>"
 * @param nparams Int. Number of parameters 2
 * @return 1 if executed correctly
 */
int rw_set_speed(char *fmt, char *params, int nparams);


/** UPPER ISTAGE COMMANDS **/
#ifdef SCH_USE_ISTAGE2
int istage2_get_temp(char *fmt, char *params, int nparams);
int istage2_get_state_panel(char *fmt, char *params, int nparams);
int istage2_deploy_panel(char *fmt, char *params, int nparams);
int istage2_set_deploy(char *fmt, char *params, int nparams);
int istage2_get_sensors_status(char *fmt, char *params, int nparams);
#endif

#endif //SUCHAI_FLIGHT_SOFTWARE_CMDRW_H
