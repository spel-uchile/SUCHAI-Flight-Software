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
//#define RPI_I2C_DEBUG

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
            int res_tmp = write(i2cHandle, wbuf, 3);
            printf("write_fd_res: %d\n", res_tmp);
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
//            printf("[bmp3 i2c read] res_tmp: %d\n", res_tmp);
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
            int res_tmp = write(i2cHandle, wbuf, 2);
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

/**************************************************************************/
/*!
    @brief  Write n bytes over I2C
*/
/**************************************************************************/
int8_t i2c_write_n(uint8_t addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    int i2cHandle;
    int res_tmp;
    if ((i2cHandle = open(deviceName, O_RDWR)) < 0)
    {
        printf("error opening I2C\n");
        return 1;
    }
    else
    {
        if (ioctl(i2cHandle, I2C_SLAVE, addr) < 0)
        {
            printf("Error at ioctl\n");
            // Close the i2c device bus
            close(*deviceName);
            return 1;
        }
        else
        {
            uint8_t wbuf[len+1];
            #ifdef RPI_I2C_DEBUG
                printf("data to write: [");
                for (int i = 0; i < len; i++) {
                    printf("%d,", reg_data[i]);
                }
                printf("]\n");
            #endif
//            printf("addr = %d\n", addr);
//            printf("reg_addr = %d\n", reg_addr);
//            printf("reg_data[0] = %d\n", reg_data[0]);
            memcpy(wbuf, &reg_addr, sizeof(uint8_t));
            memcpy(wbuf+1, reg_data, len*sizeof(uint8_t));
            res_tmp = write(i2cHandle, wbuf, len+1);
            #ifdef RPI_I2C_DEBUG
                printf("addr+data to write: [");
                for (int i = 0; i < len+1; i++) {
                    printf("%d,", wbuf[i]);
                }
                printf("]\n");
                printf("[rpi i2c_write] res_tmp: %d\n", res_tmp);
            #endif
            if (res_tmp != len+1) {
                printf("[rpi i2c_write]Fail to write %d bytes\n", len+1 - res_tmp);
                return 1;
            }
        }
    }
    // Close the i2c device bus
    close(*deviceName);
    return 0;
}

/**************************************************************************/
/*!
    @brief  Reads n bytes over I2C
*/
/**************************************************************************/
int8_t i2c_read_n(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len, uint8_t delay_ms)
{
    int i2cHandle;
    if ((i2cHandle = open(deviceName, O_RDWR)) < 0)
    {
        printf("error opening I2C\n");
        return 1;
    }
    else
    {
        if (ioctl(i2cHandle, I2C_SLAVE, dev_id) < 0) {
            printf("Error at ioctl\n");
            // Close the i2c device bus
            close(*deviceName);
            return 1;
        }
        else
        {
            memset(reg_data, 0, len);
            char wbuf[1];
            wbuf[0] = reg_addr;
            int res_tmp = write(i2cHandle, wbuf, 1);
            usleep(delay_ms*1000);
            uint8_t bytes_r = read(i2cHandle, reg_data, len);
            #ifdef RPI_I2C_DEBUG
                printf("[rpi i2c_read] res_tmp: %d\n", res_tmp);
                printf("rpi i2c_read: [");
                for (int i = 0; i < len; i++) {
                    printf("%d,", reg_data[i]);
                }
                printf("]\n");
            #endif
            if (bytes_r != len)
            {
                printf("bytes_r = %d, len = %d\n", bytes_r, len);
                perror("Failed to read from the i2c bus");
                return 1;
            }
        }
        // Close the i2c device bus
        close(*deviceName);
        return 0;
    }
}

/**************************************************************************/
/*!
    @brief  Reads n bytes over I2C
*/
/**************************************************************************/
int8_t i2c_read_from(uint8_t dev_id, uint8_t *reg_data)
{
    int i2cHandle;
    if ((i2cHandle = open(deviceName, O_RDWR)) < 0)
    {
        printf("error opening I2C\n");
        return 1;
    }
    else
    {
        if (ioctl(i2cHandle, I2C_SLAVE, dev_id) < 0) {
            printf("Error at ioctl\n");
            // Close the i2c device bus
            close(*deviceName);
            return 1;
        }
        else
        {
            uint8_t bytes_r = read(i2cHandle, reg_data, 1);
            #ifdef RPI_I2C_DEBUG
                printf("rpi i2c_read_from: [");
                for (int i = 0; i < 1; i++) {
                    printf("%d,", reg_data[i]);
                }
                printf("]\n");
            #endif
            if (bytes_r != 1)
            {
                printf("bytes_r = %d, len = %d\n", bytes_r, 1);
                perror("Failed to read from the i2c bus");
                return 1;
            }
        }
        // Close the i2c device bus
        close(*deviceName);
        return 0;
    }
}

/**************************************************************************/
/*!
    @brief  write i2c addr and 1 byte data over I2C
*/
/**************************************************************************/
int8_t i2c_write_addr(uint8_t addr, uint8_t data)
{
    int i2cHandle;
    int res_tmp;
    if ((i2cHandle = open(deviceName, O_RDWR)) < 0)
    {
        printf("error opening I2C\n");
        return 1;
    }
    else
    {
        if (ioctl(i2cHandle, I2C_SLAVE, addr) < 0)
        {
            printf("Error at ioctl\n");
            // Close the i2c device bus
            close(*deviceName);
            return 1;
        }
        else
        {
            uint8_t wbuf[1];
            #ifdef RPI_I2C_DEBUG
                printf("data to write: [%d]", data);
            #endif
            wbuf[0] = data;
            res_tmp = write(i2cHandle, wbuf, 1);
            if (res_tmp != 1) {
                printf("[rpi i2c_write]Fail to write %d bytes\n", 1 - res_tmp);
                return 1;
            }
        }
    }
    // Close the i2c device bus
    close(*deviceName);
    return 0;
}