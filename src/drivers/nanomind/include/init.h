#ifndef _INIT_H
#define _INIT_H

#include "config.h"
#include "drivers.h"
#include "repoData.h"

void sch_a3200_init_spi0(bool decode);
void sch_a3200_init_spi1(void);
void sch_a3200_init_twi0(gs_avr32_i2c_mode_t mode, uint8_t addr, uint32_t bps);
void sch_a3200_init_twi2(void);
void sch_a3200_init_can(bool enable);
gs_error_t sch_a3200_uart_init(uint8_t uart, bool enable, uint32_t bps);
static gs_error_t sch_init_rtc(void);
gs_error_t sch_a3200_init_fram(void);
gs_error_t sch_a3200_init_flash(void);
void test_sdram(int size, int do_free);
void on_init_task(void * param);
void on_reset(void);

#endif
