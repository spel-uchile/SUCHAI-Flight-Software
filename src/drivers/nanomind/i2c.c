/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2021, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
 *      Copyright 2021, Gustavo Diaz H., g.hernan.diaz@ing.uchile.cl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "suchai/i2c.h"

/**************************************************************************/
/*!
    @brief  Write n bytes over I2C
*/
/**************************************************************************/
int8_t i2c_write_n(uint8_t addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    char tx_data[len+1];
    tx_data[0] = reg_addr;
    memcpy(tx_data+1, reg_data, len);
    int rc = gs_i2c_master_transaction(2, addr, tx_data, len, NULL, 0, 500);
    return rc;
}

/**************************************************************************/
/*!
    @brief  Reads n bytes over I2C
*/
/**************************************************************************/
int8_t i2c_read_n(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len, uint8_t delay_ms)
{
    return gs_i2c_master_transaction(2, dev_id, &reg_addr, 1, reg_data, len, delay_ms);
}

/**************************************************************************/
/*!
    @brief  Reads n bytes over I2C
*/
/**************************************************************************/
int8_t i2c_read_from_n(uint8_t dev_id, uint8_t *reg_data, uint8_t len)
{
    return gs_i2c_master_transaction(2, dev_id, NULL, 0, reg_data, len, 500);
}

/**************************************************************************/
/*!
    @brief  write i2c addr and 1 byte data over I2C
*/
/**************************************************************************/
int8_t i2c_write_addr(uint8_t dev_id, uint8_t addr, uint8_t data)
{
    uint8_t tx_data[2] = {addr, data};
    return gs_i2c_master_transaction(2, dev_id, tx_data, 2, NULL, 0, 500);
}