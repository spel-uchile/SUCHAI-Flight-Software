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

void eskf_predict_state(double* P, double* Q, double dt);

int send_q_and_p(double P[6][6], double Q[6][6]);

int send_eskf_quat(quaternion_t q_est_no_eskf, quaternion_t q_est_eskf);

#endif //T_ADCS_H
