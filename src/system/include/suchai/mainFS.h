/**
 * @file  main.h
 *
 * @author Carlos Gonzalez C
 * @author Camilo Rojas M
 * @author Tomas Opazo T
 * @author Tamara Gutierrez R
 * @author Matias Ramirez M
 * @author Ignacio Ibanez A
 * @author Diego Ortego P
 * @author Elias Obreque S
 * @author Javier Morales R
 * @author Luis Jimenez V
 *
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This header contains the main's includes
 */

#ifndef SUCHAI_FLIGHT_SOFTWARE_MAIN_H
#define SUCHAI_FLIGHT_SOFTWARE_MAIN_H

#include "suchai/config.h"
#include "globals.h"

/* std includes */
#include <stdio.h>
#include <signal.h>

/* Global includes */
#include "drivers.h"
#include "suchai/init.h"
#include "suchai/log_utils.h"

/* OS includes */
#include "suchai/osThread.h"
#include "suchai/osScheduler.h"
#include "suchai/osQueue.h"
#include "suchai/osSemphr.h"
#include "os/os.h"

/* system includes */
#include "suchai/repoData.h"
#include "suchai/repoCommand.h"

/* Task includes */
#include "suchai/taskDispatcher.h"
#include "suchai/taskExecuter.h"
#include "suchai/taskInit.h"
#include "suchai/taskWatchdog.h"

int suchai_main(void);

#endif //SUCHAI_FLIGHT_SOFTWARE_MAIN_H
