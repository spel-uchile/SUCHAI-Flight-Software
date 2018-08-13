/**
 * @file  init.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2018
 * @copyright GNU GPL v3
 *
 * This header has nanomind A3200 initialization functions
 */
#ifndef _INIT_H
#define _INIT_H

#include <stdio.h>
#include <string.h>
#include <conf_a3200.h>

#include <gpio.h>
#include <sysclk.h>
#include <os.h>

#include <wdt.h>
#include <spi.h>
#include <gpio.h>
#include <board.h>
#include <sdramc.h>
#include <reset_cause.h>
#include <conf_access.h>
#include <dev/usart.h>
#include <dev/i2c.h>
#include <dev/cpu.h>

#include <lm70.h>
#include <led.h>
#include <mpu3300.h>
#include <hmc5843.h>
#include <fm33256b.h>
#include <pwr_switch.h>
#include <spn_fl512s.h>
#include <adc_channels.h>
#include <scif_uc3c.h>

#include "utils.h"
#include "config.h"

void on_reset(void);
void init_spi1(void);
void init_rtc(void);
void log_reset_cause(int reset);
void twi_init(void);
void init_can(int enable_can);
void test_sdram(int size, int do_free);

#endif //_INIT_H
