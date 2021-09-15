/**
 * @file  i2c.h
 * @author Carlos Gonzalez C. - carlgonz@uchile.cl
 * @date 2021
 * @copyright GNU GPL v3
 *
 * This header have definitions of functions related to specific CPU
 */

#ifndef SCH_CPU_H
#define SCH_CPU_H

#include <stdint.h>

int64_t get_unixtime(void);

int set_unixtime(int64_t time);

#endif //SCH_CPU_H