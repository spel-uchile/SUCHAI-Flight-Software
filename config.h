/**
 * @file  SUCHAI_config.h
 * @author Carlos Gonzalez C
 * @author Tomas Opazo T
 * @autor Ignacio Ibanez A
 * @date 2017
 * @copyright GNU Public License.
 *
 * This header contains system wide settings to customize different submodules
 */

#ifndef SUCHAI_CONFIG_H
#define	SUCHAI_CONFIG_H

/* General includes */
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "osQueue.h"

/* System debug configurations */
#define LOG_LEVEL     LOG_LVL_INFO   	///< Define debug levels
#define SCH_RUN_TESTS 0                 ///< Enable or disable tests

/* General system configurations */


/* Data repository configurations */
#define SCH_STATUS_REPO_MODE    	1   ///< Status repository location. (0) Internal, (1) Single external.
#define SCH_STORAGE_FILE            "/tmp/suchai.db"

#endif	/* SUCHAI_CONFIG_H */
