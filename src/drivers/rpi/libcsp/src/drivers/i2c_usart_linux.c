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

#include <i2c_usart_linux.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>

#include <csp/csp.h>
#include <sys/time.h>
#include <csp_if_i2c_uart.h>

static int fd = -1;
static i2c_usart_callback_t i2c_usart_callback = NULL;

static void *serial_rx_thread(void *vptr_args);

static void i2c_usart_close_fd(void) {

	if (fd >= 0) {
		close(fd);
	}
	fd = -1;
}

int i2c_usart_init(const struct i2c_usart_conf * conf) {

	int brate = 0;
	switch(conf->baudrate) {
		case 4800:    brate=B4800;    break;
		case 9600:    brate=B9600;    break;
		case 19200:   brate=B19200;   break;
		case 38400:   brate=B38400;   break;
		case 57600:   brate=B57600;   break;
		case 115200:  brate=B115200;  break;
		case 230400:  brate=B230400;  break;
#ifndef CSP_MACOSX
		case 460800:  brate=B460800;  break;
		case 500000:  brate=B500000;  break;
		case 576000:  brate=B576000;  break;
		case 921600:  brate=B921600;  break;
		case 1000000: brate=B1000000; break;
		case 1152000: brate=B1152000; break;
		case 1500000: brate=B1500000; break;
		case 2000000: brate=B2000000; break;
		case 2500000: brate=B2500000; break;
		case 3000000: brate=B3000000; break;
		case 3500000: brate=B3500000; break;
		case 4000000: brate=B4000000; break;
#endif
		default:
			csp_log_error("%s: Unsupported baudrate: %u", __FUNCTION__, conf->baudrate);
			return CSP_ERR_INVAL;
	}

	fd = open(conf->device, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0) {
		csp_log_error("%s: failed to open device: [%s], errno: %s", __FUNCTION__, conf->device, strerror(errno));
		return CSP_ERR_INVAL;
	}

	struct termios options;
	tcgetattr(fd, &options);
	cfsetispeed(&options, brate);
	cfsetospeed(&options, brate);
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
	options.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
	options.c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OPOST);
	options.c_cc[VTIME] = 0;
	options.c_cc[VMIN] = 1;
	/* tcsetattr() succeeds if just one attribute was changed, should read back attributes and check all has been changed */
	if (tcsetattr(fd, TCSANOW, &options) != 0) {
		csp_log_error("%s: Failed to set attributes on device: [%s], errno: %s", __FUNCTION__, conf->device, strerror(errno));
		i2c_usart_close_fd();
		return CSP_ERR_DRIVER;
	}
	fcntl(fd, F_SETFL, 0);

	/* Flush old transmissions */
	if (tcflush(fd, TCIOFLUSH) != 0) {
		csp_log_error("%s: Error flushing device: [%s], errno: %s", __FUNCTION__, conf->device, strerror(errno));
		i2c_usart_close_fd();
		return CSP_ERR_DRIVER;
	}

	pthread_t rx_thread;
	if (pthread_create(&rx_thread, NULL, serial_rx_thread, NULL) != 0) {
		csp_log_error("%s: pthread_create() failed to create Rx thread for device: [%s], errno: %s", __FUNCTION__, conf->device, strerror(errno));
		i2c_usart_close_fd();
		return CSP_ERR_NOMEM;
	}

	return CSP_ERR_NONE;
}

void i2c_usart_set_callback(i2c_usart_callback_t callback) {
	i2c_usart_callback = callback;
}

void i2c_usart_insert(char c, void * pxTaskWoken) {
	printf("%c", c);
}

void i2c_usart_putstr(const char * buf, int len) {
	if (write(fd, buf, len) != len)
		return;
}

void i2c_usart_putc(char c) {
	if (write(fd, &c, 1) != 1)
		return;
}

char i2c_usart_getc(void) {
	char c;
	if (read(fd, &c, 1) != 1) return 0;
	return c;
}

static void *serial_rx_thread(void *vptr_args) {
	uint8_t * cbuf = malloc(sizeof(i2c_uart_frame_t));

	// Receive loop
	while (1) {
        memset(cbuf, 0, sizeof(i2c_uart_frame_t));
		int nbytes = 0;

		/* SYNC */
        read(fd, cbuf, 1);
        read(fd, cbuf+1, 1);
        while(cbuf[0] != 'O' && cbuf[1] != 'K')
        {
            cbuf[0] = cbuf[1];
            read(fd, cbuf+1, 1); putchar(cbuf[1]);

        }
        nbytes += 2;

        /* Read the rest of the packet */
		while(nbytes < sizeof(i2c_uart_frame_t))
        {
		    nbytes += read(fd, cbuf+nbytes, sizeof(i2c_uart_frame_t)-2);
        }
		if (nbytes <= 0) {
			csp_log_error("%s: read() failed, returned: %d", __FUNCTION__, nbytes);
			exit(1);
		}
		if (i2c_usart_callback) {
			i2c_usart_callback(cbuf, nbytes, NULL);
		}
	}
	return NULL;
}
