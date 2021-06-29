/**
 * @file  cmdAPP.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2021
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to user application.
 */

#ifndef CMD_APP_H
#define CMD_APP_H

#include "suchai/config.h"

#include "suchai/repoCommand.h"
#include "suchai/repoData.h"

/**
 * Register on board computer related (OBC) commands
 */
void cmd_app_init(void);

/**
 * Read OBC sensors and save values
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int obc_get_sensors(char *fmt, char *params, int nparams);

/**
 * Read OBC sensors, print the results and save the values to status repository.
 *
 * @param fmt Str. Parameters format ""
 * @param params Str. Parameters as string ""
 * @param nparams Int. Number of parameters 0
 * @return  CMD_OK if executed correctly, CMD_ERROR in case of failures, or CMD_ERROR_SYNTAX in case of parameters errors
 */
int obc_update_status(char *fmt, char *params, int nparams);

#endif /* CMD_APP_H */
