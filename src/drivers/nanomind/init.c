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

static const char *tag = "on_reset";

void init_spi1(void) {
    /* Setting up the GPIO map for SPI on the board */
    gpio_map_t spi_piomap = {
            {AVR32_SPI1_SCK_0_1_PIN, AVR32_SPI1_SCK_0_1_FUNCTION},
            {AVR32_SPI1_MOSI_0_1_PIN, AVR32_SPI1_MOSI_0_1_FUNCTION},
            {AVR32_SPI1_MOSI_0_2_PIN, AVR32_SPI1_MOSI_0_2_FUNCTION},
            {AVR32_SPI1_MISO_0_1_PIN, AVR32_SPI1_MISO_0_1_FUNCTION},
            {AVR32_SPI1_NPCS_2_2_PIN, AVR32_SPI1_NPCS_2_2_FUNCTION},
            {AVR32_SPI1_NPCS_3_2_PIN, AVR32_SPI1_NPCS_3_2_FUNCTION},
            {AVR32_SPI1_NPCS_0_PIN, AVR32_SPI1_NPCS_0_FUNCTION},
            {AVR32_SPI1_NPCS_1_2_PIN, AVR32_SPI1_NPCS_1_2_FUNCTION},
    };
    gpio_enable_module(spi_piomap, 8);

    /* Setup of SPI controller one */
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

void init_can(int enable_can) {

    /* Setup the generic clock for CAN */
    scif_gc_setup(AVR32_SCIF_GCLK_CANIF, SCIF_GCCTRL_OSC0, AVR32_SCIF_GC_NO_DIV_CLOCK, 0);
    if (enable_can) {
        /* Enable the generic clock */
        scif_gc_enable(AVR32_SCIF_GCLK_CANIF);

        /* Setup GPIO map for CAN connection in stack */
        static const gpio_map_t CAN_GPIO_MAP = {
                {AVR32_CANIF_RXLINE_0_0_PIN, AVR32_CANIF_RXLINE_0_0_FUNCTION},
                {AVR32_CANIF_TXLINE_0_0_PIN, AVR32_CANIF_TXLINE_0_0_FUNCTION}
        };
        /* Assign GPIO to CAN */
        gpio_enable_module(CAN_GPIO_MAP, sizeof(CAN_GPIO_MAP) / sizeof(CAN_GPIO_MAP[0]));

        /* Initialize PA15 (CANMODE) GPIO, must be pulled low to enable CAN Transceiver TI SN65HVD230 */
        gpio_configure_pin(AVR32_PIN_PA15, GPIO_DIR_OUTPUT | GPIO_INIT_LOW);
    } else {
        /* Stop the generic clock */
        scif_stop_gclk(AVR32_SCIF_GCLK_CANIF);
        /* Ensure can output is low to disable/power down transceiver */
        gpio_configure_pin(AVR32_PIN_PA15, (GPIO_DIR_OUTPUT | GPIO_INIT_HIGH));
    }
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

/**
 * Log reset cause
 * @param reset reset cause
 */
void log_reset_cause(int reset) {
    switch (reset) {
        case RESET_CAUSE_WDT:
            LOGI(tag, "Reset cause: Watchdog timeout (%d)", reset);
            break;
        case RESET_CAUSE_SOFT:
            LOGI(tag, "Reset cause: Software reset (%d)", reset);
            break;
        case RESET_CAUSE_SLEEP:
            LOGI(tag, "Reset cause: Wake from Shutdown sleep mode (%d)", reset);
            break;
        case RESET_CAUSE_EXTRST:
            LOGI(tag, "Reset cause: External reset (%d)", reset);
            break;
        case RESET_CAUSE_CPU_ERROR:
            LOGI(tag, "Reset cause: CPU Error (%d)", reset);
            break;
        case RESET_CAUSE_BOD_CPU:
            LOGI(tag, "Reset cause: Brown-out detected on CPU power domain (%d)", reset);
            break;
        case RESET_CAUSE_POR:
            LOGI(tag, "Reset cause: Power-on-reset (%d)", reset);
            break;
        case RESET_CAUSE_JTAG:
            LOGI(tag, "Reset cause: JTAG (%d)", reset);
            break;
        case RESET_CAUSE_OCD:
            LOGI(tag, "Reset cause: On-chip debug system (%d)", reset);
            break;
        case RESET_CAUSE_BOD_IO:
            LOGI(tag, "Reset cause: Brown-out detected on I/O power domain (%d)", reset);
            break;
        default:
            LOGI(tag, "Reset cause: Undefined! (%d)", reset);
    }
}

/**
 * Init I2C for OBC and PC104 stack
 */
void twi_init(void) {
    /* I2C2 Pins for OBC sensors */
    const gpio_map_t TWIM_GPIO_MAP = {
            {AVR32_TWIMS2_TWCK_0_0_PIN, AVR32_TWIMS2_TWCK_0_0_FUNCTION},
            {AVR32_TWIMS2_TWD_0_0_PIN, AVR32_TWIMS2_TWD_0_0_FUNCTION}
    };
    gpio_enable_module(TWIM_GPIO_MAP, sizeof(TWIM_GPIO_MAP) / sizeof(TWIM_GPIO_MAP[0]));

    /* I2C0 Pins for PC104 connector*/
    const gpio_map_t TWIM_GPIO_MAP2 = {
            {AVR32_TWIMS0_TWCK_0_0_PIN, AVR32_TWIMS0_TWCK_0_0_FUNCTION},
            {AVR32_TWIMS0_TWD_0_0_PIN, AVR32_TWIMS0_TWD_0_0_FUNCTION}
    };
    gpio_enable_module(TWIM_GPIO_MAP2, sizeof(TWIM_GPIO_MAP2) / sizeof(TWIM_GPIO_MAP2[0]));

    /* Init I2C2 controller with addr 1 and 100 kHz clock */
    i2c_init_master(2, 1, 100);

    /* I2C0 initialized with LibCSP in TaskInit
     * Ex: csp_i2c_init(node, handle, speed_khz);
     */
}

/**
 * Test SDRAM or SRAM. Reserves with malloc a buffer is size @size words (in the
 * AVR21 words are 4 bytes), print the reserved memory pointer address and run a
 * test over the memory. The this write and reads a pattern value in the RAM and
 * checks the patterns matches.
 *
 * @param size Int. Size of the allocated buffer in words (aka. 4 bytes)
 * @param do_free Int. If 1 the frees the buffer, else do not.
 */
void test_sdram(int size, int do_free) {
    int *data = (int *)malloc(size*sizeof(int));
    if(data == NULL){
        printf("---- malloc failed! ----\n\n");
    }
    else{
        printf("data @ %p\n", data);
    }


    printf("Writing to SDRAM...\r\n");

    for(int * p = data; p < (int *) data+size; p++) {
        *p = (uintptr_t) p;
    }

    printf("Reading from SDRAM...\r\n");

    for(int * p = data; p < (int *) data+size; p++) {
        if ((uintptr_t) p != (uintptr_t) *p) {
            printf("SDRAM ERROR!\r\n");
            return;
        }
    }

    printf("SDRAM OK\r\n");

    if(do_free)
        free(data);

}

void on_reset(void)
{
    /* Init LED */
    led_init();
    led_on(LED_CPUOK);
    led_off(LED_A);

    /* Reset watchdog */
    wdt_disable();
    wdt_clear();
    wdt_opt_t wdtopt = { .us_timeout_period = SCH_WDT_PERIOD*1000000 };
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

    /**
     * Set heap address to the external SDRAM, so malloc calls will use the
     * 32MB external SDRAM. Otherwise only the 68kB internal SRAM are available
     * @warning Do not touch this lines. See Gomspace framework docs and AVR32
     * application notes on SDRAM and Linker Scripts usages for further details.
     */
#if 1
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
    gpio_enable_module(USART_GPIO_MAP, sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));
    usart_init(USART_CONSOLE, sysclk_get_peripheral_bus_hz(USART), SCH_UART_BAUDRATE);
    usart_stdio_id = USART_CONSOLE;

    /**
     * Test the external SDRAM. If working properly we are able to reserve this
     * large amount of memory and will use and address higher than 0xD0000000.
     * Otherwise, the internal SRAM is not big enough then malloc will raise an
     * error and the device will be reset.
     */
#if 0
    test_sdram(100000, 1);
#endif

    /* Finish on reset, both led on */
    led_on(LED_CPUOK);
    led_on(LED_A);
}
