/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2018, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "init.h"

void init_spi(void) {
    gpio_map_t spi_piomap = {
            {AVR32_SPI1_SCK_0_1_PIN, AVR32_SPI1_SCK_0_1_FUNCTION},
            {AVR32_SPI1_MOSI_0_1_PIN, AVR32_SPI1_MOSI_0_1_FUNCTION},
            {AVR32_SPI1_MOSI_0_2_PIN, AVR32_SPI1_MOSI_0_2_FUNCTION},
            {AVR32_SPI1_MISO_0_1_PIN, AVR32_SPI1_MISO_0_1_FUNCTION},
            {AVR32_SPI1_NPCS_2_2_PIN, AVR32_SPI1_NPCS_2_2_FUNCTION},
            {AVR32_SPI1_NPCS_3_2_PIN, AVR32_SPI1_NPCS_3_2_FUNCTION},
#ifdef ENABLE_SPANSION
    {AVR32_SPI1_NPCS_0_PIN, AVR32_SPI1_NPCS_0_FUNCTION},
		{AVR32_SPI1_NPCS_1_2_PIN, AVR32_SPI1_NPCS_1_2_FUNCTION},
#endif
    };
#ifdef ENABLE_SPANSION
    gpio_enable_module(spi_piomap, 8);
#else
    gpio_enable_module(spi_piomap, 6);
#endif

    /* SPI1 setup */
    sysclk_enable_pba_module(SYSCLK_SPI1);
    spi_reset(&AVR32_SPI1);
    spi_set_master_mode(&AVR32_SPI1);
    spi_disable_modfault(&AVR32_SPI1);
    spi_disable_loopback(&AVR32_SPI1);
    spi_set_chipselect(&AVR32_SPI1, (1 << AVR32_SPI_MR_PCS_SIZE) - 1);
    spi_disable_variable_chipselect(&AVR32_SPI1);
    spi_disable_chipselect_decoding(&AVR32_SPI1);
    spi_enable(&AVR32_SPI1);
}

void init_spn_fl512(void) {
    spn_fl512s_init((unsigned int) 0);
}

void init_rtc(void) {

    /* Setup RTC */
    uint8_t cmd[] = {FM33_WRPC, 0x18, 0x3D};
    fm33256b_write(cmd, 3);

    /* RTC */
    fm33256b_clock_resume();

    /* 32kHz Crystal setup */
    osc_enable(OSC_ID_OSC32);

}

void print_rst_cause(int reset_cause) {
    printf("Reset cause ");
    switch (reset_cause) {
        case RESET_CAUSE_WDT:
            printf("WDT\r\n");
            break;
        case RESET_CAUSE_SOFT:
            printf("SOFT\r\n");
            break;
        case RESET_CAUSE_SLEEP:
            printf("SLEEP\r\n");
            break;
        case RESET_CAUSE_EXTRST:
            printf("EXT RST\r\n");
            break;
        case RESET_CAUSE_CPU_ERROR:
            printf("CPU ERROR\r\n");
            break;
        case RESET_CAUSE_BOD_CPU:
            printf("BOD CPU\r\n");
            break;
        case RESET_CAUSE_POR:
            printf("POR\r\n");
            break;
        case RESET_CAUSE_JTAG:
            printf("JTAG\r\n");
            break;
        case RESET_CAUSE_OCD:
            printf("OCD\r\n");
            break;
        case RESET_CAUSE_BOD_IO:
            printf("BOD IO\r\n");
            break;
        default:
            printf("UNKNOWN\r\n");
    }
}

/* Init I2C */
void twi_init(void) {
    /* GPIO map setup */
    const gpio_map_t TWIM_GPIO_MAP = {
            {AVR32_TWIMS2_TWCK_0_0_PIN, AVR32_TWIMS2_TWCK_0_0_FUNCTION},
            {AVR32_TWIMS2_TWD_0_0_PIN, AVR32_TWIMS2_TWD_0_0_FUNCTION}
    };
    gpio_enable_module(TWIM_GPIO_MAP,
                       sizeof(TWIM_GPIO_MAP) / sizeof(TWIM_GPIO_MAP[0]));

    /* Init twi master controller 2 with addr 5 and 100 kHz clock */
    i2c_init_master(2, 5, 100);
}

void on_reset(void)
{
    /* Reset watchdog */
    wdt_disable();
    wdt_clear();
    wdt_opt_t wdtopt = { .us_timeout_period = 5000000 };
    wdt_enable(&wdtopt);

    /* Initialize interrupt handling.
     * This function call is needed, when using libasf startup.S file */
    INTC_init_interrupts();

    /* Init CLK */
    sysclk_init();

    /* Init pwr channels */
    pwr_switch_init();

    /* At this point we only power the interstages.
     * MLX and finesunsensors are powered on in init task */
    pwr_switch_disable(PWR_GSSB);
    pwr_switch_disable(PWR_GSSB2);

    /* Init SDRAM, do this before malloc is called */
    sdramc_init(sysclk_get_cpu_hz());

    /* We have to trust that the SDRAM is working by now */
#if 0
    extern void *heap_start, *heap_end;
	heap_start = (void *) 0xD0000000 + 0x100000;
	heap_end = (void *) 0xD0000000 + 0x2000000 - 1000;
#endif

    /* Init USART for debugging */
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    static const gpio_map_t USART_GPIO_MAP = {
            {USART_RXD_PIN, USART_RXD_FUNCTION},
            {USART_TXD_PIN, USART_TXD_FUNCTION},
    };
    gpio_enable_module(USART_GPIO_MAP,
                       sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));
    usart_init(USART_CONSOLE, sysclk_get_peripheral_bus_hz(USART), 500000);
    usart_stdio_id = USART_CONSOLE;

//    LOGV(tag, "\tSetting SPI devices...");
    init_spi();
    lm70_init();
    fm33256b_init();

    /* Init LED */
    led_init();
    led_on(LED_CPUOK);
    led_on(LED_A);
}