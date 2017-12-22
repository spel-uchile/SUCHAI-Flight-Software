/**
 * @file  taskCommunications.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2017
 * @copyright GNU GPL v3
 *
 *
 * This task implements a listener that reads remote commands from TRX. Also
 * works as the CSP server to process common services and custom ports.
 *
 */

#ifndef T_COMMUNICATIONS_H
#define T_COMMUNICATIONS_H

#include "stdlib.h"

#include "os.h"
#include "csp/csp.h"

#include "config.h"
#include "globals.h"

#include "repoCommand.h"

void taskCommunications(void *param);

#endif //T_COMMUNICATIONS_H
