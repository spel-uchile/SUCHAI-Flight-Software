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

void on_reset(void);
void init_spi(void);
void init_spn_fl512(void);
void init_rtc(void);
void print_rst_cause(int reset_cause);
void twi_init(void);

#endif //_INIT_H
