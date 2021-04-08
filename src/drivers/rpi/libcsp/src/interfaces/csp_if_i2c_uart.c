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

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <assert.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <csp/csp_interface.h>
#include <csp/csp_error.h>
#include <csp/drivers/usart.h>
#include <csp/arch/csp_semaphore.h>

#include "csp_if_i2c_uart.h"

static int i2c_uart_lock_init = 0;
static csp_bin_sem_handle_t i2c_uart_kiss_lock;

/**
 * Receive a CSP packet and cast to a I2C_UART frame, ready to be transmitted
 * by UART to the I2C-UART controller.
 * @param interface out interface
 * @param packet data to transmit
 * @param timeout passed to the driver
 * @return CSP_ERR_NONE or CSP_ERR_DRIVER
 */
int csp_i2c_uart_tx(csp_iface_t * interface, csp_packet_t * packet, uint32_t timeout) {

    /* Cast the CSP packet buffer into an i2c frame */
    assert(packet->length + sizeof(packet->id) < I2C_MTU);
    uint8_t data_len =  (uint8_t)(packet->length + sizeof(packet->id));
    i2c_uart_frame_t * frame = (i2c_uart_frame_t *)packet;

    /* Insert destination node into the i2c destination field */
    if (csp_rtable_find_mac(packet->id.dst) == CSP_NODE_MAC)
        frame->addr = (uint8_t)(packet->id.dst);
    else
        frame->addr = csp_rtable_find_mac(packet->id.dst);

    /* Save the outgoing id in the buffer */
    packet->id.ext = csp_hton32(packet->id.ext);

    /* Add the CSP header to the I2C length field */
    frame->len = data_len+10; //Padding+len+addr+len_tx
    frame->len_tx = data_len;

    /* enqueue the frame */
    printf("%X|%d|%s|\n", frame->addr, frame->len_tx, frame->data);
    csp_bin_sem_wait(&i2c_uart_kiss_lock, 1000);
    usart_putstr((const char *)frame, frame->len_tx);
    csp_bin_sem_post(&i2c_uart_kiss_lock);
    return CSP_ERR_NONE;
}

/**
 * When a frame is received, cast it to a csp_packet
 * and send it directly to the CSP new packet function.
 * Context: ISR only
 * @param frame
 */
void csp_i2c_uart_rx(uint8_t *buf, int len, void *pxTaskWoken) {
    if(len > sizeof(i2c_uart_frame_t))
    {
        csp_log_error("I2C_UART_RX Buffer overrun\n");
        return;
    }

    i2c_uart_frame_t *frame = csp_buffer_get(sizeof(i2c_uart_frame_t));
    if (frame == NULL)
        return;

    memcpy(frame, buf, len);
    printf("%X|%d|%s|\n", frame->addr, frame->len_tx, frame->data);

    if ((frame->len < 4) || (frame->len > I2C_MTU)) {
        csp_if_i2c_uart.frame++;
        csp_buffer_free_isr(frame);
        return;
    }

    /* Strip the CSP header off the length field before converting to CSP packet */
    frame->len -= sizeof(csp_id_t);

    /* Convert the packet from network to host order */
    csp_packet_t *packet = (csp_packet_t *) frame;
    packet->id.ext = csp_ntoh32(packet->id.ext);

    /* Receive the packet in CSP */
    csp_qfifo_write(packet, &csp_if_i2c_uart, pxTaskWoken);

}

int csp_i2c_uart_init(uint8_t addr, int handle, int speed) {

    /* Init lock only once */
    if (i2c_uart_lock_init == 0) {
        csp_bin_sem_create(&i2c_uart_kiss_lock);
        i2c_uart_lock_init = 1;
    }

    /* Create uart */
    struct usart_conf conf;
    conf.baudrate = speed;
    conf.device = "/dev/ttyS0";
    if(usart_init(&conf) != CSP_ERR_NONE)
    {
        printf("Error initializing USART\n");
        return CSP_ERR_DRIVER;
    }
    usart_set_callback(csp_i2c_uart_rx);

    /* Register interface */
    csp_iflist_add(&csp_if_i2c_uart);

    return CSP_ERR_NONE;

}

/** Interface definition */
csp_iface_t csp_if_i2c_uart = {
        .name = "I2C_UART",
        .nexthop = csp_i2c_uart_tx,
};