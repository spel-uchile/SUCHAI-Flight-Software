//
// Created by javier on 09-07-20.
//

#ifndef T_ADCS_H
#define T_ADCS_H

#include <stdlib.h>
#include <stdint.h>

#include "config.h"
#include "globals.h"

#include "osQueue.h"
#include "osDelay.h"

#include "math_utils.h"

#include "repoCommand.h"

void taskADCS(void *param);

void eskf_predict_state(double* P, double dt);

#endif //T_ADCS_H
