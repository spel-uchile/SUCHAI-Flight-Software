/**
 * @file  commandRepoitory.h
 * @author Tomas Opazo T - tomas.opazo.t@gmail.com
 * @author Carlos Gonzalez C - carlgonz@ug.uchile.cl
 * @date 2012
 * @copyright GNU GPL v3
 *
 * @id 0x1101
 *
 * This task implements a listener, that send commands at periodical times.
 */

#ifndef T_HOUSEKEEPING_H
#define T_HOUSEKEEPING_H

#include "../../OS/include/osQueue.h"
#include "../../OS/include/osDelay.h"

#include "../../SUCHAI_config.h"

#include "repoCommand.h"

void taskHousekeeping(void *param);

#endif //T_HOUSEKEEPING_H




