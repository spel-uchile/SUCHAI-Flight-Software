/**
 * @file  i2c.h
 * @author Gustavo Díaz H - g.hernan.diaz@ing.uchile.cl
 * @date 2021
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to i2c interface for Linux
 * (RW) features.
 */

#ifndef SCH_I2C_H
#define SCH_I2C_H

#include "drivers.h"


int8_t i2c_write_n(uint8_t addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);

/*!
 *    @brief  Low level generic i2c write
 *    @param  addr, reg_addr, reg_data, len
 *    @return err_code
 */

int8_t i2c_read_n(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len, uint8_t delay_ms);

/*!
 *    @brief  Low level generic i2c read
 *    @param  addr, reg_addr, reg_data, len
 *    @return err_code
 */

int8_t i2c_read_from_n(uint8_t dev_id, uint8_t *reg_data, uint8_t len);

/*!
 *    @brief  Low level specific i2c write
 *    @param  addr, data
 *    @return err_code
 */

int8_t i2c_write_addr(uint8_t dev_id, uint8_t addr, uint8_t data);

#endif /* SCH_I2C_H */