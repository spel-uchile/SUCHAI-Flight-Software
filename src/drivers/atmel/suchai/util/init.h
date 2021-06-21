#ifndef __INIT_H
#define __INIT_H

/* system includes */
#include "repoData.h"
#include "repoCommand.h"

#include <stdint.h>
#include <stdint.h>
#include "suchai/log_utils.h"
#include <stdio.h>
#include <signal.h>
#include "asf.h"
#include <time.h>
#include <avr32/io.h>
#include "intc.h"
#include "board.h"
#include "compiler.h"
#include "rtc.h"
#include "usart.h"
#include "pm.h"



void serial_init(void);
void spi_init(void);
void ssc_init(void *buffer, void *buffer2, __int_handler handler);
void ssc_start(void);

uint32_t memcheck(void);
void hexdump(char *desc, void *addr, int len);

__attribute__((__interrupt__)) void rtc_irq(void);
void on_close(int signal);
void on_reset(void);

void twi_init(void);

#endif //__INIT_H
