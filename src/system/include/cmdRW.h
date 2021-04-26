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

//#include "drivers.h"
//#include "config.h"
//#include "globals.h"
//#include "log_utils.h"
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

//#endif /* CMD_RW_H */

#endif //SUCHAI_FLIGHT_SOFTWARE_CMDRW_H
