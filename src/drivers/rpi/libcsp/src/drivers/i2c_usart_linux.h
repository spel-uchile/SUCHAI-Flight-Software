/*
Cubesat Space Protocol - A small network-layer protocol designed for Cubesats
Copyright (C) 2012 GomSpace ApS (http://www.gomspace.com)
Copyright (C) 2012 AAUSAT3 Project (http://aausat3.space.aau.dk)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef USART_H_
#define USART_H_

/**
   @file

   USART driver.

   @note This interface implementation only support ONE open UART connection.
*/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
   Usart configuration, to be used with the usart_init call.
*/
struct i2c_usart_conf {
    //! USART device.
    const char *device;
    //! bits per second.
    uint32_t baudrate;
    //! Number of data bits.
    uint8_t databits;
    //! Number of stop bits.
    uint8_t stopbits;
    //! Parity setting.
    uint8_t paritysetting;
    //! Enable parity checking (Windows only).
    uint8_t checkparity;
};

/**
   Initialise an UART device.

   On success, a pthread is created/started to read data from the device.

   @param[in] conf UART configuration
   @return #CSP_ERR_NONE on success
*/
int i2c_usart_init(const struct i2c_usart_conf *conf);

/**
   Callback for returning data to application.

   @param[in] buf data received.
   @param[in] len data length (number of bytes in \a buf).
   @param[out] pxTaskWoken can be set, if context switch is required due to received data.
*/
typedef void (*i2c_usart_callback_t) (uint8_t *buf, int len, void *pxTaskWoken);

/**
   Set callback for receiving data.

   @param[in] callback callback. Only one callback is supported.
*/
void i2c_usart_set_callback(i2c_usart_callback_t callback);

/**
   Insert a character to the RX buffer of the usart

   @param[in] c character to insert
   @param[out] pxTaskWoken can be set, if context switch is required due to received data.
*/
void i2c_usart_insert(char c, void *pxTaskWoken);

/**
   Polling putchar (stdin).

   @param[in] c Character to transmit
*/
void i2c_usart_putc(char c);

/**
   Send char buffer on UART (stdout).

   @param[in] buf Pointer to data
   @param[in] len Length of data
*/
void i2c_usart_putstr(const char *buf, int len);

/**
   Buffered getchar (stdin).

   @note Unsafe, because i2c_usart_init() creates a Rx thread, which also reads from the device.

   @return Character received
*/
char i2c_usart_getc(void);

#ifdef __cplusplus
}
#endif
#endif /* USART_H_ */
