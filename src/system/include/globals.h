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

extern osQueue dispatcher_queue;         ///< Commands queue
extern osQueue executer_cmd_queue;       ///< Executer commands queue
extern osQueue executer_stat_queue;      ///< Executer result queue
extern osSemaphore repo_data_sem;        ///< Data repository mutex
extern osSemaphore repo_data_fp_sem;     ///< Flight plan repository mutex
extern osSemaphore repo_machine_sem;     ///< State machine repository mutex
extern osSemaphore repo_cmd_sem;         ///< Command repository mutex

#endif //GLOBALS_H
