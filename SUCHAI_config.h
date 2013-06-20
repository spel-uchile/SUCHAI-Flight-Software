/**
 * @file  SUCHAI_config.h
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com 
 * @author Carlos Gonzalez C - carlgonz@ug.uchile.cl
 * @date 2013
 * @copyright GNU GPL v3
 *
 * The purpose of this header is configure several options of the application
 * by a series of defines. Configurations detailed here defines the behavior of
 * the flight software, by example, enabling or not debug options, available
 * hardware, and subsystem's configuration parameters.
 */

#ifndef SUCHAI_CONFIG_H
#define	SUCHAI_CONFIG_H

/* General includes */
#include <stdlib.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "cmdIncludes.h"

/* General system configurations */

/* Data repository configurations */
#define SCH_STATUS_REPO_MODE    0   ///< Status repository location. (0) Internal, (1) Single external.

/* System verbose configurations */
#define SCH_GRL_VERBOSE         1   ///< Activates verbose debug mode


#endif	/* SUCHAI_CONFIG_H */
