/**
 * \file *********************************************************************
 *
 * \brief Spi Master configuration template file
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_SPI_MASTER_H_INCLUDED
#define CONF_SPI_MASTER_H_INCLUDED

/** \name SPI Connections
 * @{
 */
#define DATA_OUT_SPI                 (&AVR32_SPI0)
#define DATA_OUT_SPI_NPCS            3
#define DATA_OUT_SPI_BAUDRATE        10000000
#define DATA_OUT_SPI_SCK_PIN         AVR32_SPI0_SCK_0_0_PIN
#define DATA_OUT_SPI_SCK_FUNCTION    AVR32_SPI0_SCK_0_0_FUNCTION
#define DATA_OUT_SPI_MISO_PIN        AVR32_SPI0_MISO_0_0_PIN
#define DATA_OUT_SPI_MISO_FUNCTION   AVR32_SPI0_MISO_0_0_FUNCTION
#define DATA_OUT_SPI_MOSI_PIN        AVR32_SPI0_MOSI_0_0_PIN
#define DATA_OUT_SPI_MOSI_FUNCTION   AVR32_SPI0_MOSI_0_0_FUNCTION
#define DATA_OUT_SPI_NPCS0_PIN       AVR32_SPI0_NPCS_3_0_PIN
#define DATA_OUT_SPI_NPCS0_FUNCTION  AVR32_SPI0_NPCS_3_0_FUNCTION
/* Possibility to change low-level configurations here */

//! Default Config Spi Master Delay BCS
// #define CONFIG_SPI_MASTER_DELAY_BCS            0

//! Default Config Spi Master Bits per Transfer Definition
// #define CONFIG_SPI_MASTER_BITS_PER_TRANSFER    8

//! Default Config Spi Master Delay BCT
// #define CONFIG_SPI_MASTER_DELAY_BCT            0

//! Default Config Spi Master Delay BS
// #define CONFIG_SPI_MASTER_DELAY_BS             0

//! Default Config Spi Master Dummy Field
// #define CONFIG_SPI_MASTER_DUMMY                0xFF

#endif /* CONF_SPI_MASTER_H_INCLUDED */
