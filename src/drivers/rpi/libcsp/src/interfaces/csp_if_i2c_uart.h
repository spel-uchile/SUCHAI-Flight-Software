//
// Created by carlos on 03-04-21.
//

#ifndef _CSP_IF_I2C_UART_H
#define _CSP_IF_I2C_UART_H

extern csp_iface_t csp_if_i2c_uart;

/**
 * Maximum transfer length on I2C
 */
#define I2C_MTU 	255
/**
 * Sync bytes
 */
#define I2C_UART_SYNC (0x4F4B)

typedef struct __attribute__((packed)) i2c_uart_frame_s {
    //! Not used by CSP - sync UART frames
    uint16_t sync;
    //! Not used by CSP - padding
    uint8_t padding[3];
    //! Not used by CSP - total bytes to send by UART
    uint16_t len;
    //! Not used by CSP - Destination address
    uint8_t addr;
    //! Total bytes to send by I2C
    uint16_t len_tx;
    //! CSP data (data+header)
    uint8_t data[I2C_MTU];
} i2c_uart_frame_t;

/**
 * Capture I2C RX events for CSP
 * @param opt_addr local i2c address
 * @param handle which i2c device to use
 * @param speed interface speed in kHz (normally 100 or 400)
 * @return csp_error.h code
 */
int csp_i2c_uart_init(uint8_t opt_addr, int handle, int speed);

void csp_i2c_uart_rx(uint8_t *buf, int len, void *pxTaskWoken);

#endif //_CSP_IF_I2C_UART_H
