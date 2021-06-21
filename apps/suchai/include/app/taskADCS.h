//
// Created by javier on 09-07-20.
//

#ifndef T_ADCS_H
#define T_ADCS_H

#include <stdlib.h>
#include <stdint.h>

#include "suchai/config.h"
#include "globals.h"

#include "osQueue.h"
#include "osDelay.h"

#include "math_utils.h"

#include "repoCommand.h"
#include "igrf/igrf13.h"

void taskADCS(void *param);

void eskf_predict_state(double* P, double dt);

void calc_sun_pos_i(double jd, vector3_t * sun_dir);

double unixt_to_jd(uint32_t unix_time);

void calc_magnetic_model(double decyear, double latrad, double lonrad, double altm, double* mag);

double jd_to_dec(double jd);

#endif //T_ADCS_H
