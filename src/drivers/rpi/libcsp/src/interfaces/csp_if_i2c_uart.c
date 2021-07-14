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
#include <stdint.h>
#include <assert.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <csp/csp_interface.h>
#include <csp/csp_error.h>
#include <csp/arch/csp_semaphore.h>

#include "i2c_usart_linux.h"
#include "csp_if_i2c_uart.h"

static int i2c_uart_lock_init = 0;
static csp_bin_sem_handle_t i2c_uart_lock;
static csp_bin_sem_handle_t i2c_uart_ans_lock;

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
    uint8_t i2c_addr;
    int csp_data_len =  packet->length + sizeof(packet->id);
    int csp_padding_len =  sizeof(packet->padding)+sizeof(packet->length);
    assert(csp_data_len < I2C_MTU);

    /* Insert destination node into the i2c destination field */
    if (csp_rtable_find_mac(packet->id.dst) == CSP_NODE_MAC)
        i2c_addr = (uint8_t)(packet->id.dst);
    else
        i2c_addr = csp_rtable_find_mac(packet->id.dst);

    /* Pack CSP header to network endianness */
    packet->id.ext = csp_hton32(packet->id.ext);

    /* Create a frame and copy data to sent */
    /* Packet can be size variable, frame is fixed size (filled with zeros) */
    i2c_uart_frame_t frame;
    memset(&frame, 0, sizeof(i2c_uart_frame_t));
    memcpy(&frame, packet, csp_padding_len+csp_data_len);

    /* Fill extra frame data */
    frame.addr = i2c_addr;
    frame.len = csp_padding_len+csp_data_len; //The full CSP packet len
    frame.len_tx = csp_data_len; // Only header and data len (to send via i2c)
    frame.sync = csp_hton16(I2C_UART_SYNC);

    /** DEBUG **
    printf("[OUT] Total len: %d. Fram sync: %#X, Frame len: %d. Iface len: %d, Frame Add: %d|\n", (int)sizeof(i2c_uart_frame_t), frame.sync, frame.len, frame.len_tx, frame.addr);
    printf("[OUT] CSP From: %d. To: %d. Len: %d, \n", packet->id.src, packet->id.dst, packet->length);
    printf("[OUT] id: %#X (%d, %d)\n", packet->id.ext, packet->id.src, packet->id.dst);
    packet->id.ext = csp_hton32(packet->id.ext);
    printf("[OUT] id: %#X (%d, %d)\n", packet->id.ext, packet->id.src, packet->id.dst);
    */

    /* enqueue the frame */
    char *buff = (char *)&frame;
    /** DEBUG **
    print_buff(buff, sizeof(i2c_uart_frame_t));
    print_buff((char *)frame->data, frame->len_tx);
    */
    int rc;
    rc = csp_bin_sem_wait(&i2c_uart_lock, 2000);
    if(rc != CSP_SEMAPHORE_OK)
        return CSP_ERR_DRIVER;

    // Transmmit
    i2c_usart_putstr(buff, sizeof(i2c_uart_frame_t));

    // Wait confirmation
    /** printf("[I2C_UART] Waiting lock...\n");*/
    rc = csp_bin_sem_wait(&i2c_uart_ans_lock, 2000);
    /** printf("[I2C_UART] Released? %d\n", rc); */
    csp_bin_sem_post(&i2c_uart_lock);

    // Confirmation was not received
    if(rc != CSP_SEMAPHORE_OK)
    {
        csp_log_error("Error sending packet %p (%d)", packet, rc);
        return CSP_ERR_DRIVER;
    }

    // Free the packet if the transmission was successful
    csp_buffer_free(packet);
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

    i2c_uart_frame_t *frame = csp_buffer_get(I2C_MTU); //Also allocate CSP_BUFFER_PACKET_OVERHEAD
    if (frame == NULL)
        return;

    memcpy(frame, buf, len);

    /** DEBUG **
    csp_packet_t *packet2 = (csp_packet_t *) frame;
    printf("[IN] id: %#X (%d, %d)\n", packet2->id.ext, packet2->id.src, packet2->id.dst);
    packet2->id.ext = csp_ntoh32(packet2->id.ext);
    printf("[IN] id: %#X (%d, %d)\r\n", packet2->id.ext, packet2->id.src, packet2->id.dst);
    printf("[IN ] Total len: %d. Fram sync: %#X, Frame len: %d. Iface len: %d, Frame Add: %d|\r\n", len, frame->sync, frame->len, frame->len_tx, frame->addr);
    printf("[IN ] CSP From: %d. To: %d. Len: %d, \r\n", packet2->id.src, packet2->id.dst, packet2->length);
    //print_buff(buf, len);
    print_buff(frame->data, frame->len_tx);
    */

    if ((frame->len_tx < 4) || (frame->len_tx > I2C_MTU)) {
        csp_if_i2c_uart.frame++;
        csp_buffer_free_isr(frame);
        return;
    }

    /* Strip the CSP header off the length field before converting to CSP packet */
    frame->len_tx -= sizeof(csp_id_t);

    /* Convert the packet from network to host order */
    csp_packet_t *packet = (csp_packet_t *) frame;
    packet->id.ext = csp_ntoh32(packet->id.ext);

    /* Receive the packet in CSP */
    csp_qfifo_write(packet, &csp_if_i2c_uart, pxTaskWoken);

}

int csp_i2c_uart_init(uint8_t addr, int handle, int speed) {

    /* Init lock only once */
    if (i2c_uart_lock_init == 0) {
        csp_bin_sem_create(&i2c_uart_lock);
        csp_bin_sem_create(&i2c_uart_ans_lock);
        csp_bin_sem_wait(&i2c_uart_ans_lock, CSP_INFINITY); // Just decrement the semaphore to star locked;
        i2c_uart_lock_init = 1;
    }

    /* Create uart */
    struct i2c_usart_conf conf;
    conf.baudrate = speed;
    conf.device = "/dev/serial0";
    conf.uart_ans_lock = &i2c_uart_ans_lock;
    if(i2c_usart_init(&conf) != CSP_ERR_NONE)
    {
        printf("Error initializing USART\n");
        return CSP_ERR_DRIVER;
    }
    i2c_usart_set_callback(csp_i2c_uart_rx);

    /* Register interface */
    csp_iflist_add(&csp_if_i2c_uart);

    return CSP_ERR_NONE;

}

/** Interface definition */
csp_iface_t csp_if_i2c_uart = {
        .name = "I2C_UART",
        .nexthop = csp_i2c_uart_tx,
};
