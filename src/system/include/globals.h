/**
 * @file  globals.h
 * @author Carlos Gonzalez C
 * @author Diego Ortego P
 * @date 2019
 * @copyright GNU GPL v3
 *
 * This header contains global variables
 */
#ifndef GLOBALS_H
#define GLOBALS_H

#include "osQueue.h"
#include "osSemphr.h"

osQueue dispatcher_queue;         ///< Commands queue
osQueue executer_cmd_queue;       ///< Executer commands queue
osQueue executer_stat_queue;      ///< Executer result queue
osSemaphore repo_data_sem;        ///< Status repository mutex
osSemaphore repo_data_fp_sem;     ///< Flight plan repository mutex
osSemaphore repo_cmd_sem;         ///< Command repository mutex

#endif //GLOBALS_H
