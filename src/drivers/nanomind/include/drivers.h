/**
 * @file drivers.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @date 2019
 * @copyright GNU GPL v3
 *
 * Include all platform specific drivers.
 */

#ifndef _DRIVERS_H
#define _DRIVERS_H
//SPEL
#include "TLE.h"

// MCU Drivers
#include <board.h>
#include <wdt.h>
#include <gpio.h>
#include <sysclk.h>
#include <spi.h>
#include <sdramc.h>
#include <scif_uc3c.h>

// A3200 Drivers
#include <gs/a3200/led.h>
#include <gs/a3200/lm71.h>
#include <gs/a3200/hmc5843.h>
#include <gs/a3200/mpu3300.h>
#include <gs/a3200/pwr_switch.h>
#include <gs/a3200/pwm.h>
#include <gs/a3200/adc_channels.h>
#include <gs/a3200/spi_slave.h>
#include <gs/a3200/uart.h>
#include <gs/thirdparty/fram/fm33256b.h>
#include <gs/thirdparty/flash/spn_fl512s.h>

// BUS sub-systems drivers
// Embed
#include <gs/embed/stdio.h>
#include <gs/embed/drivers/sys/reset.h>
#include <gs/embed/drivers/uart/uart.h>
#include <gs/embed/asf/drivers/spi/master.h>
#include <gs/embed/asf/drivers/i2c/i2c.h>
#include <gs/embed/asf/avr32/drivers/sys/avr32_reset_all_interrupt_settings.h>
// Util
#include <gs/util/rtc.h>
#include <gs/util/time.h>
#include <gs/util/thread.h>
#include <gs/util/clock.h>
#include <gs/util/timestamp.h>
#include <gs/util/error.h>
#include <gs/util/drivers/i2c/master.h>
// CSP
#include <csp/csp.h>
#include <csp/csp_types.h>
#include <csp/csp_endian.h>
#include <csp/drivers/i2c.h>
#include <csp/interfaces/csp_if_i2c.h>
// Param
#include <gs/param/rparam.h>
#include <deprecated/param/param_client.h>
// AX100
#include "ax100_param.h"
#include "ax100_param_radio.h"
// EPS
#include <io/nanopower2.h>
#include <gs/bpx/io.h>
// GSSB
#include <gs/gssb/gssb_all_devices.h>
#include <gs/gssb/gssb_autodeploy.h>

#endif
