/**
 * @file  i2c.h
 * @author Gustavo Díaz H - g.hernan.diaz@ing.uchile.cl
 * @date 2019
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to i2c interface for Linux
 * (RW) features.
 */

#ifndef I2C_H
#define I2C_H

#include "osDelay.h"

#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <unistd.h>

#define BIuC_ADDR 0x11
#define MCP9808_I2CADDR_DEFAULT 0x1f ///< I2C address

/**
 * Register reaction whee related (rw) commands
 */

int i2c_write(uint8_t addr, uint8_t data1, uint8_t data2, uint8_t data3);

/**
 * i2c write master transaction to slave
 *
 * @param addr Slave address to write to
 * @param data1 first byte to send
 * @param data2 second byte to send
 * @param data3 third byte to send
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */

int i2c_read(char* buf);

/**
 * i2c read master transaction to slave
 *
 * @param buf registers to save data from slave
 * @return  CMD_OK if executed correctly or CMD_FAIL in case of errors
 */


int write16(uint8_t reg, uint16_t value);

/*!
 *    @brief  Low level 16 bit write procedures
 *    @param  reg
 *    @param  value
 */


uint16_t read16(uint8_t reg);

/*!
 *    @brief  Low level 16 bit read procedure
 *    @param  reg
 *    @return value
 */

int write8(uint8_t reg, uint8_t value);

/*!
 *    @brief  Low level 8 bit write procedure
 *    @param  reg
 *    @param  value
 */

uint8_t read8(uint8_t reg);

/*!
 *    @brief  Low level 8 bit read procedure
 *    @param  reg
 *    @return value
 */


#endif /* CMD_RW_H */