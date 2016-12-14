/**
 * @file  SUCHAI_config.h
 * @author Tomas Opazo T
 * @author Carlos Gonzalez C
 * @autor Ignacio Ibañez A
 * @date 04-01-2013
 * @copyright GNU Public License.
 *
 * El proposito de este header file es configurar las ditintas opciones de SUCHAI
 * en un solo archivo a traves de una serie de defines que son utilzadas por
 * el pre-procesador del compilador. Las configuraciones descritas aca, afectan
 * la forma en que funciona el sistema de vuelo del satelite SUCHAI, por ejemplo
 * activando o desactivando opciones de debug, hardware disponible y parametros
 * generales del sistema.
 */

#ifndef SUCHAI_CONFIG_H
#define	SUCHAI_CONFIG_H

/* General includes */
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "System/include/cmdIncludes.h"
#include "OS/include/osQueue.h"

#if __linux__
    #define portMAX_DELAY (uint16_t) 0xffff
	#define pdPASS				     1
    #define configMINIMAL_STACK_SIZE 1
 	#define portBASE_TYPE	short
 	#define ClrWdt(){};
#else
 	#include "FreeRTOS.h"
    #include "task.h"
 	#include "queue.h"
 	#include "semphr.h"
    #include "timers.h"
#endif

/* General system configurations */

/* Data repository configurations */
#define SCH_STATUS_REPO_MODE    	0   ///< Status repository location. (0) Internal, (1) Single external.
#define SCH_CHECK_IF_EXECUTABLE_SOC 0   ///< Check if a command is executable using energy level (SOC)

/* System verbose configurations */
#define SCH_GRL_VERBOSE     1   ///< Activates verbose debug mode

#endif	/* SUCHAI_CONFIG_H */
