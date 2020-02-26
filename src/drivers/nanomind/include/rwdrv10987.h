/**
* @file rwdrv10987.h
* @mainpage RW I2C Command Interface
* @author Gustavo Diaz H.
* @date 23 Jan 2020
* @brief This header have definitions of cmds related to the DRV10987 motor driver
*
* @copyright GPL
*
*/

#ifndef SUCHAI_FLIGHT_SOFTWARE_RWDRV10987_H
#define SUCHAI_FLIGHT_SOFTWARE_RWDRV10987_H

#include <stdint.h>
#include "drivers.h"
#include "globals.h"
#include "utils.h"

#include "osDelay.h"

#define OBC_ADDR 0x10
#define BIuC_ADDR 0x11

#define SAMPLE_SPEED_CODE_MOTOR1 21
#define SAMPLE_CURRENT_CODE_MOTOR1 22
#define SET_SPEED_CODE_MOTOR1 23

#define SAMPLE_SPEED_CODE_MOTOR2 24
#define SAMPLE_CURRENT_CODE_MOTOR2 25
#define SET_SPEED_CODE_MOTOR2 26

#define SAMPLE_SPEED_CODE_MOTOR3 27
#define SAMPLE_CURRENT_CODE_MOTOR3 28
#define SET_SPEED_CODE_MOTOR3 29

#define MOTOR1_ID 1
#define MOTOR2_ID 2
#define MOTOR3_ID 3


/**************************************************************************/
/*!
    @brief  setup and initialize communication with the hardware
*/
/**************************************************************************/
int rwdrv10987_init(void);

/**************************************************************************/
/*!
    @brief get speed of the specified motor from the drv10987 driver
    @param motor_id:[1-3]
*/
/**************************************************************************/
uint16_t rwdrv10987_get_speed(uint8_t motor_id);

/**************************************************************************/
/*!
    @brief get current of the specified motor from the drv10987 driver
    @param motor_id:[1-3]
*/
/**************************************************************************/
float rwdrv10987_get_current(uint8_t motor_id);

/**************************************************************************/
/*!
    @brief set speed of the specified motor through the drv10987 driver
    @param motor_id:[1-3]
*/
/**************************************************************************/
int8_t rwdrv10987_set_speed(uint8_t motor_id, uint16_t speed);

#endif //SUCHAI_FLIGHT_SOFTWARE_RWDRV10987_H
