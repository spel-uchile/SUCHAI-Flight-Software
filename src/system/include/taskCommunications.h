/**
 * @file  taskCommunications.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This task implements a client that reads remote commands from TRX. Also
 * works as the CSP server to process common services and custom ports.
 *
 */

#ifndef T_COMMUNICATIONS_H
#define T_COMMUNICATIONS_H

#include <stdlib.h>

#include "config.h"
#include "globals.h"

#include <csp/csp.h>
#include <csp/csp_types.h>
#include <csp/csp_endian.h>

#include "osQueue.h"
#include "osDelay.h"

#include "repoCommand.h"
#include "cmdTM.h"

void taskCommunications(void *param);

#endif //T_COMMUNICATIONS_H
