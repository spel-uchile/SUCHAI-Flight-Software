/**
 * @file  taskSensors.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Camilo Rojas - camrojas@uchile.cl
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This task implements a listener, that sends commands at periodical times.
 */
#ifndef _TASKSENSORS_H
#define _TASKSENSORS_H

#include "config.h"
#include "globals.h"

#include "osQueue.h"
#include "osDelay.h"

#include "repoCommand.h"

void taskSensors(void *param);

int is_payload_active(int payload, int active_payloads, int n_payloads);
#endif //_TASKSENSORS_H
