//
// Created by carlos on 02-12-16.
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include "osQueue.h"
#include "osSemphr.h"

osQueue dispatcher_queue;    /* Commands queue */
osQueue executer_cmd_queue;   /* Executer commands queue */
osQueue executer_stat_queue;  /* Executer result queue */
osSemaphore repo_data_sem;    /* Data repository mutex */

#endif //GLOBALS_H
