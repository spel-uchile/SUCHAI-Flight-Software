/**
 * @file  i2c_comm.h
 * @author Carlos Gonzalez C
 * @date 2013
 * @copyright GNU LGPL v3
 */

#ifndef I2C_COMM_H
#define I2C_COMM_H

#include <i2c.h>

//#define I2C_EEPROM_IDW	0xA0
//#define I2C_EEPROM_IDR	0xA1
#define I2C_EEPROM_ID 0b01010000

/*---------------------------------
 *      I2C MODULES CODING TABLE
 *---------------------------------*/
#define I2C_MOD1    1
#define I2C_MOD2    2
#define I2C_MOD3    3

/*---------------------------------
 *      I2C SLAVE STATUS TABLE
 *---------------------------------*/
#define I2C_SLV_IDLE    0
#define I2C_SLV_ADDR    1
#define I2C_SLV_DATA    2
#define I2C_SLV_READ    3

/*---------------------------------
 *      I2C CLOCK RATES TABLE
 *---------------------------------
 * Req.Fr -  FCY  - BRG
 * 100kHz - 16MHz - 157
 * 100kHz - 08MHz - 78
 * 400kHz - 16MHz - 37
 * 001MHz - 16MHz - 13
 *--------------------------------*/

void i2c1_open(unsigned int BRG, char address);
int i2c1_master_fputs(const char *data, int len, char *address, int addlen);
int i2c1_master_fgets(char *data, int len, char *address, int addlen);

#endif
