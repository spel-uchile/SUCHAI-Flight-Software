/**
 * @file rwdrv10987.c
 * @mainpage RW I2C Command Interface
 * @author Gustavo Diaz H.
 * @date 23 Jan 2020
 * @brief Define basic set_speed, get_speed and get_current commands
 *
 * @copyright GPL
 *
 */

#include "rwdrv10987.h"

/**************************************************************************/
/*!
    @brief  setup and initialize communication with the hardware
*/
/**************************************************************************/
int rwdrv10987_init(void)
{
    rwdrv10987_set_speed(MOTOR1_ID, 0);
    rwdrv10987_set_speed(MOTOR2_ID, 0);
    rwdrv10987_set_speed(MOTOR3_ID, 0);
    return 1;
}

/**************************************************************************/
/*!
    @brief get speed of the specified motor from the drv10987 driver
    @param motor_id:[1-3]
*/
/**************************************************************************/
uint16_t rwdrv10987_get_speed(uint8_t motor_id)
{
    //send sample speed cmd
    if(motor_id == MOTOR1_ID){
        uint8_t cmd[3] = {SAMPLE_SPEED_CODE_MOTOR1, 0x00, 0x00};
        gs_i2c_master_transaction(2, BIuC_ADDR, cmd, 3, NULL, 0, 1000);
    }
    else if(motor_id == MOTOR2_ID){
        uint8_t cmd[3] = {SAMPLE_SPEED_CODE_MOTOR2, 0x00, 0x00};
        gs_i2c_master_transaction(2, BIuC_ADDR, cmd, 3, NULL, 0, 1000);
    }
    else if(motor_id == MOTOR3_ID){
        uint8_t cmd[3] = {SAMPLE_SPEED_CODE_MOTOR3, 0x00, 0x00};
        gs_i2c_master_transaction(2, BIuC_ADDR, cmd, 3, NULL, 0, 1000);
    }
    else{
        printf("[RWDRV10987:Error]: Bad id");
        return -1;
    }
    osDelay(200);
    //read data regs
    uint8_t res[2];
    gs_error_t rc = gs_i2c_master_transaction(2, BIuC_ADDR, NULL, 0, res, 2, 1000);
    if(rc == GS_OK) {
        uint16_t speed = (res[0] << 8) | res[1];
        return speed;
    }
    else{
        printf("[RWDRV10987:Error i2c]: %d", rc);
        return 0;
    }
}

/**************************************************************************/
/*!
    @brief get current of the specified motor from the drv10987 driver
    @param motor_id:[1-3]
*/
/**************************************************************************/
float rwdrv10987_get_current(uint8_t motor_id)
{
    //send sample current cmd
    if(motor_id == MOTOR1_ID){
        uint8_t cmd[3] = {SAMPLE_CURRENT_CODE_MOTOR1, 0x00, 0x00};
        gs_i2c_master_transaction(2, BIuC_ADDR, cmd, 3, NULL, 0, 1000);
    }
    else if(motor_id == MOTOR2_ID){
        uint8_t cmd[3] = {SAMPLE_CURRENT_CODE_MOTOR2, 0x00, 0x00};
        gs_i2c_master_transaction(2, BIuC_ADDR, cmd, 3, NULL, 0, 1000);
    }
    else if(motor_id == MOTOR3_ID){
        uint8_t cmd[3] = {SAMPLE_CURRENT_CODE_MOTOR3, 0x00, 0x00};
        gs_i2c_master_transaction(2, BIuC_ADDR, cmd, 3, NULL, 0, 1000);
    }
    else{
        printf("[RWDRV10987:Error]: Bad id");
        return -1;
    }
    osDelay(200);
    //read data regs
    uint8_t res[2] = {0, 0};
    gs_error_t rc = gs_i2c_master_transaction(2, BIuC_ADDR, NULL, 0, res, 2, 1000);

    if(rc == GS_OK) {
        uint16_t current_aux = ((res[0] & 0x07) << 8) | res[1];
        float current = 0;
        if(current_aux >= 1023)
            current = 3000 * (current_aux - 1023) / 2048.0; //[mA]
        else
            current = 3000 * (current_aux) / 2048.0; //[mA]
        return current;
    }
    else{
        printf("[RWDRV10987:Error i2c]: %d)", rc);
        return -1.0;
    }

}

/**************************************************************************/
/*!
    @brief set speed of the specified motor through the drv10987 driver
    @param motor_id:[1-3]
*/
/**************************************************************************/
int8_t rwdrv10987_set_speed(uint8_t motor_id, uint16_t speed)
{
    gs_error_t result_cmd;
    if(motor_id == MOTOR1_ID)
    {
        uint8_t cmd[3] = {SET_SPEED_CODE_MOTOR1, speed & 0xff, speed >> 8};
        result_cmd = gs_i2c_master_transaction(2, BIuC_ADDR, cmd, 3, NULL, 0, 1000);
    }
    else if(motor_id == MOTOR2_ID)
    {
        uint8_t cmd[3] = {SET_SPEED_CODE_MOTOR2, speed & 0xff, speed >> 8};
        result_cmd = gs_i2c_master_transaction(2, BIuC_ADDR, cmd, 3, NULL, 0, 1000);
    }
    else if(motor_id == MOTOR3_ID)
    {
        uint8_t cmd[3] = {SET_SPEED_CODE_MOTOR3, speed & 0xff, speed >> 8};
        result_cmd = gs_i2c_master_transaction(2, BIuC_ADDR, cmd, 3, NULL, 0, 1000);
    }
    else
    {
        printf("[RWDRV10987:Error]: Bad id");
        return -1;
    }
    osDelay(200);  // Avoid activate another motor immediately
    return result_cmd;
}
