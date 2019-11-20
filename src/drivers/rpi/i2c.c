/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2018, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
 *      Copyright 2018, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *      Copyright 2018, Matias Ramirez Martinez, nicoram.mt@gmail.com
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

#include "i2c.h"

char *deviceName = (char*)"/dev/i2c-1";

int i2c_write(uint8_t addr, uint8_t data1, uint8_t data2, uint8_t data3)
{
    int i2cHandle;
    if ((i2cHandle = open(deviceName, O_RDWR)) < 0)
    {
        printf("error opening I2C\n");
        return 0;
    }
    else
    {
        if (ioctl(i2cHandle, I2C_SLAVE, addr) < 0) {
            printf("Error at ioctl\n");
            return 0;
        } else {
            char wbuf[3];
            wbuf[0] = data1;
            wbuf[1] = data2;
            wbuf[2] = data3;
            int res_tmp = write(i2cHandle, wbuf, sizeof(wbuf));
            //printf("write_fd_res: %d", res_tmp);
        }
        // Close the i2c device bus
        close(*deviceName);
        return 1;
    }
}

int i2c_read(char buf[])
{
    int i2cHandle;
    //size_t buf_len;
    uint8_t buf_len;
    if ((i2cHandle = open(deviceName, O_RDWR)) < 0)
    {
        printf("error opening I2C\n");
        return 0;
    }
    else
    {
        if (ioctl(i2cHandle, I2C_SLAVE, BIuC_ADDR) < 0) {
            printf("Error at ioctl\n");
            return 0;
        } else {
            buf_len = sizeof(buf);
            memset(buf, 0, buf_len);
            uint8_t bytes_r = read(i2cHandle, buf, buf_len);
            if (bytes_r != buf_len)
            {
                perror("Failed to read from the i2c bus");
                return 0;
            }
            /*printf("buf[0] = %d\n", buf[0]);
            printf("buf[1] = %d\n", buf[1]);
            printf("buf[2] = %d\n", buf[2]);*/
        }
        // Close the i2c device bus
        close(*deviceName);
        return 1;
    }
}

/*!
 *    @brief  Low level 16 bit write procedures
 *    @param  reg
 *    @param  value
 */
int write16(uint8_t reg, uint16_t value) {
    return i2c_write(MCP9808_I2CADDR_DEFAULT, reg, value>>8, value & 0xFF);
}

/*!
 *    @brief  Low level 16 bit read procedure
 *    @param  reg
 *    @return value
 */
uint16_t read16(uint8_t reg) {
    uint16_t val = 0xFFFF;
    int i2cHandle;
    uint8_t buf_len = 2;
    char buf[buf_len];
    if ((i2cHandle = open(deviceName, O_RDWR)) < 0)
    {
        printf("error opening I2C\n");
        return 0;
    }
    else
    {
        if (ioctl(i2cHandle, I2C_SLAVE, MCP9808_I2CADDR_DEFAULT) < 0) {
            printf("Error at ioctl\n");
            return 0;
        } else {
//            buf_len = sizeof(buf);
            memset(buf, 0, buf_len);
            char wbuf[1] = {reg};
            int res_tmp = write(i2cHandle, wbuf, 1);
            uint8_t bytes_r = read(i2cHandle, buf, buf_len);
            if (bytes_r != buf_len)
            {
                perror("Failed to read from the i2c bus");
                return 0;
            }
            /*printf("buf[0] = %d\n", buf[0]);
            printf("buf[1] = %d\n", buf[1]);
            printf("buf[2] = %d\n", buf[2]);*/
        }
        // Close the i2c device bus
        close(*deviceName);
        val = buf[0]<<8 | buf[1];
        return val;
    }
}

/*!
 *    @brief  Low level 8 bit write procedure
 *    @param  reg
 *    @param  value
 */
int write8(uint8_t reg, uint8_t value)
{
    int i2cHandle;
    if ((i2cHandle = open(deviceName, O_RDWR)) < 0)
    {
        printf("error opening I2C\n");
        return 0;
    }
    else
    {
        if (ioctl(i2cHandle, I2C_SLAVE, MCP9808_I2CADDR_DEFAULT) < 0) {
            printf("Error at ioctl\n");
            return 0;
        } else {
            char wbuf[2];
            wbuf[0] = reg;
            wbuf[1] = value;
            int res_tmp = write(i2cHandle, wbuf, sizeof(wbuf));
            //printf("write_fd_res: %d", res_tmp);
        }
        // Close the i2c device bus
        close(*deviceName);
        return 1;
    }
}

/*!
 *    @brief  Low level 8 bit read procedure
 *    @param  reg
 *    @return value
 */
uint8_t read8(uint8_t reg) {
    uint8_t val = 0xFF;
    int i2cHandle;
    uint8_t buf_len = 1;
    char buf[buf_len];
    if ((i2cHandle = open(deviceName, O_RDWR)) < 0) {
        printf("error opening I2C\n");
        return 0;
    } else {
        if (ioctl(i2cHandle, I2C_SLAVE, MCP9808_I2CADDR_DEFAULT) < 0) {
            printf("Error at ioctl\n");
            return 0;
        } else {
            memset(buf, 0, buf_len);
            char wbuf[1] = {reg};
            int res_tmp = write(i2cHandle, wbuf, 1);
            uint8_t bytes_r = read(i2cHandle, buf, buf_len);
            if (bytes_r != buf_len) {
                perror("Failed to read from the i2c bus");
                return 0;
            }
            /*printf("buf[0] = %d\n", buf[0]);
            printf("buf[1] = %d\n", buf[1]);
            printf("buf[2] = %d\n", buf[2]);*/
        }
        // Close the i2c device bus
        close(*deviceName);
        val = buf[0];
        return val;
    }
}