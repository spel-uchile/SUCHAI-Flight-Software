/**
 * @file  cmdSensors.h
 * @author Gustavo Díaz H - g.hernan.diaz@ing.uchile.cl
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Camilo Rojas - camrojas@uchile.cl
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to the PlantSat payload
 * sensors
 */

#ifndef _CMD_SENS_H
#define _CMD_SENS_H

#include <stdint.h>
#include <unistd.h>

#include "config.h"

#include "os/os.h"
#include "osDelay.h"

#include "repoCommand.h"
#include "taskSensors.h"


void cmd_sensors_init(void);

/**
 * Control sensors state machine task
 * @param fmt "%u %u %d"
 * @param params <action> <step> <nsamples>
 * action: Next action ACT_PAUSE= 0, ACT_START=1, ACT_STAND_BY=2,
 * step: Seconds between sates update (or samples)
 * nsamples: Number of samples to take before go to ACT_STAND_BY. Use -1
 *           to run for ever.
 * @param nparams 3
 * @code
 *
 * //Start machine, every 2 seconds, 10 samples max.
 * set_state 1 2 10
 * //Start machine, every 1 seconds, for ever
 * set_state 1 1 -1
 * //Stpo machine [keep 2 seconds, 10 samples]
 * set_state 2 2 10
 *
 * @return
 */
int set_state(char *fmt, char *params, int nparams);

int init_dummy_sensor(char *fmt, char *params, int nparams);

#endif /* _CMD_SENS_H */
