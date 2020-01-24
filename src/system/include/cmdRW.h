/**
 * @file  cmdRW.h
 * @author Gustavo Díaz H - g.hernan.diaz@ing.uchile.cl
 * @date 2019
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
//#include "utils.h"
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
int rw_get_speed(char *fmt, char *params, int nparams);
int rw_get_current(char *fmt, char *params, int nparams);
int rw_set_speed(char *fmt, char *params, int nparams);

//#endif /* CMD_RW_H */

#endif //SUCHAI_FLIGHT_SOFTWARE_CMDRW_H
