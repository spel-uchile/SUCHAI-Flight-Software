#include "init.h"

#ifndef __linux
/* This is a required to enable CPP linking */
// void *__dso_handle __attribute__((used)) = 0;
#endif

static const char *tag = "init";

void sch_a3200_init_spi0(bool decode)
{
    const gpio_map_t map = {
        {AVR32_SPI0_MOSI_2_PIN, AVR32_SPI0_MOSI_2_FUNCTION},
        {AVR32_SPI0_MISO_2_PIN, AVR32_SPI0_MISO_2_FUNCTION},
        {AVR32_SPI0_SCK_2_PIN, AVR32_SPI0_SCK_2_FUNCTION},
        {AVR32_SPI0_NPCS_0_2_PIN, AVR32_SPI0_NPCS_0_2_FUNCTION},
        {AVR32_SPI0_NPCS_1_2_PIN, AVR32_SPI0_NPCS_1_2_FUNCTION},
        {AVR32_SPI0_NPCS_2_2_PIN, AVR32_SPI0_NPCS_2_2_FUNCTION},
    };
    gpio_enable_module(map, GS_ARRAY_SIZE(map));

    gs_spi_init_device(0, decode);
}

void sch_a3200_init_spi1(void)
{
    const gpio_map_t map = {
        {AVR32_SPI1_SCK_0_1_PIN, AVR32_SPI1_SCK_0_1_FUNCTION},
        {AVR32_SPI1_MOSI_0_1_PIN, AVR32_SPI1_MOSI_0_1_FUNCTION},
        {AVR32_SPI1_MOSI_0_2_PIN, AVR32_SPI1_MOSI_0_2_FUNCTION},
        {AVR32_SPI1_MISO_0_1_PIN, AVR32_SPI1_MISO_0_1_FUNCTION},
        {AVR32_SPI1_NPCS_2_2_PIN, AVR32_SPI1_NPCS_2_2_FUNCTION},
        {AVR32_SPI1_NPCS_3_2_PIN, AVR32_SPI1_NPCS_3_2_FUNCTION},
        {AVR32_SPI1_NPCS_0_PIN, AVR32_SPI1_NPCS_0_FUNCTION},
        {AVR32_SPI1_NPCS_1_2_PIN, AVR32_SPI1_NPCS_1_2_FUNCTION},
    };
    gpio_enable_module(map, GS_ARRAY_SIZE(map));

    gs_spi_init_device(1, false);
}

void sch_a3200_init_twi0(gs_avr32_i2c_mode_t mode, uint8_t addr, uint32_t bps)
{
    const gpio_map_t map = {
        {AVR32_TWIMS0_TWCK_0_0_PIN, AVR32_TWIMS0_TWCK_0_0_FUNCTION},
        {AVR32_TWIMS0_TWD_0_0_PIN, AVR32_TWIMS0_TWD_0_0_FUNCTION}
    };
    gpio_configure_pin(AVR32_TWIMS0_TWCK_0_0_PIN, GPIO_DRIVE_HIGH);
    gpio_configure_pin(AVR32_TWIMS0_TWD_0_0_PIN, GPIO_DRIVE_HIGH);
    gpio_enable_module(map, GS_ARRAY_SIZE(map));

    gs_avr_i2c_init(0, mode, addr, bps);
}

void sch_a3200_init_twi2(void)
{
    const gpio_map_t map = {
        {AVR32_TWIMS2_TWCK_0_0_PIN, AVR32_TWIMS2_TWCK_0_0_FUNCTION},
        {AVR32_TWIMS2_TWD_0_0_PIN, AVR32_TWIMS2_TWD_0_0_FUNCTION}
    };
    gpio_enable_module(map, GS_ARRAY_SIZE(map));

    /* Init twi master controller 2 with addr 1 and 150 kHz clock */
    gs_avr_i2c_init(2, GS_AVR_I2C_MASTER, 1, 150000);
}

void sch_a3200_init_can(bool enable)
{
    /* Setup the generic clock for CAN */
    scif_gc_setup(AVR32_SCIF_GCLK_CANIF,
                  SCIF_GCCTRL_OSC0,
                  AVR32_SCIF_GC_NO_DIV_CLOCK,
                  0);
    if (enable) {
        /* Enable the generic clock */
        scif_gc_enable(AVR32_SCIF_GCLK_CANIF);

        /* Setup GPIO map for CAN connection in stack */
        const gpio_map_t map = {
            {AVR32_CANIF_RXLINE_0_0_PIN, AVR32_CANIF_RXLINE_0_0_FUNCTION},
            {AVR32_CANIF_TXLINE_0_0_PIN, AVR32_CANIF_TXLINE_0_0_FUNCTION}
        };
        gpio_enable_module(map, GS_ARRAY_SIZE(map));

        /* Initialize PA15 (CANMODE) GPIO, must be pulled low to enable CAN Transceiver TI SN65HVD230 */
        gpio_configure_pin(AVR32_PIN_PA15, GPIO_DIR_OUTPUT | GPIO_INIT_LOW);
    } else {
        /* Stop the generic clock */
        scif_stop_gclk(AVR32_SCIF_GCLK_CANIF);
        /* Ensure can output is high to disable/power down transceiver */
        gpio_configure_pin(AVR32_PIN_PA15, (GPIO_DIR_OUTPUT | GPIO_INIT_HIGH));
    }
}

gs_error_t sch_a3200_uart_init(uint8_t uart, bool enable, uint32_t bps)
{
    if (uart == 1) {
        /* UART 1 (FSI connector / GPS) */
        const gpio_map_t map = {
            {AVR32_USART1_RXD_PIN, AVR32_USART1_RXD_FUNCTION},
            {AVR32_USART1_TXD_PIN, AVR32_USART1_TXD_FUNCTION},
        };
        gpio_enable_module(map, GS_ARRAY_SIZE(map));
    } else if (uart == 2) {
    	/* UART 2 (DEBUG connector) */
        const gpio_map_t map = {
            {AVR32_USART2_RXD_1_PIN, AVR32_USART2_RXD_1_FUNCTION},
            {AVR32_USART2_TXD_1_PIN, AVR32_USART2_TXD_1_FUNCTION},
    	};
    	gpio_enable_module(map, GS_ARRAY_SIZE(map));
    } else if (uart == 4) {
    	/* UART 4 (FSI connector) */
    	const gpio_map_t map = {
            {AVR32_USART4_RXD_PIN, AVR32_USART4_RXD_FUNCTION},
            {AVR32_USART4_TXD_PIN, AVR32_USART4_TXD_FUNCTION},
    	};
    	gpio_enable_module(map, GS_ARRAY_SIZE(map));
    } else {
        return GS_ERROR_HANDLE;
    }

    gs_uart_config_t uart_config;
    gs_uart_get_default_config(&uart_config);
    uart_config.tx_queue_size = 0; // direct write
    uart_config.comm.bps = bps;

    gs_error_t error = gs_uart_init(uart, &uart_config);
    if (error) {
        log_error("Failed to initialize UART: %u, bps: %"PRIu32" - gs_uart_init() return: %s ",
                  uart, bps, gs_error_string(error));
    }
    return error;
}

static gs_error_t sch_init_rtc(void)
{
    /* 32kHz Crystal setup */
    osc_enable(OSC_ID_OSC32);
    return gs_rtc_register(&gs_fm33256b_rtc_driver, NULL);
}

gs_error_t sch_a3200_init_fram(void)
{
    const gs_spi_chip_t spi = {.handle = 1,
                               .baudrate = 8000000,
                               .bits = 8,
                               .chipselect = FM33256_SPI_CS,
                               .spi_mode = 0,
                               .stay_act = 1,
                               .trans_delay = 5};
    const gs_fm33256b_config_t fram = {.spi_slave = GS_A3200_SPI_SLAVE_FRAM};

    gs_error_t error = gs_spi_asf_master_configure_slave(fram.spi_slave, &spi);
    if (error) {
        log_error("Failed configuring SPI for FRAM (FM33256B), error: %s", gs_error_string(error));
        return error;
    }

    error = gs_fm33256b_init(0, &fram);
    if (error) {
        log_error("Failed initializing FRAM (FM33256B), error: %s", gs_error_string(error));
        return error;
    }

    return sch_init_rtc();
}

gs_error_t sch_a3200_init_flash(void)
{
    /* Turn on power */
    gs_a3200_pwr_switch_enable(GS_A3200_PWR_SD);
    /* Initialize spansion chip. Requires that the SPI device has been initialized */
    const spn_fl512s_config_t config = {
            .bps = 8000000,
            .cs_part_0 = SPN_FL512S_CS0,
            .cs_part_1 = SPN_FL512S_CS1,
            .spi_slave_part_0 = GS_A3200_SPI_SLAVE_SPN_FL512_0,
            .spi_slave_part_1 = GS_A3200_SPI_SLAVE_SPN_FL512_1,
            .spi_handle = 1,
    };
    int error = (int)spn_fl512s_init(&config);
    if (error)
        return -1;
}

void on_init_task(void * param)
{
    gs_error_t rc;
    /**
     * Setting SPI devices
     */
    /* SPI1 device drivers */
    sch_a3200_init_spi1();
    /* Init temperature sensors */
    gs_a3200_lm71_init();
    /* Init FLASH (fl512s) */
    rc = sch_a3200_init_flash();
    if(rc!=0) LOGE(tag, "FLASH not initialized!");
    /* Init FRAM and RTC (FM33256B chip) */
    rc = sch_a3200_init_fram();
    if(rc!=0) LOGE(tag, "FRAM not initialized!");
    /* SPI0 device driver - has to be called after param init */
    sch_a3200_init_spi0(false);

    /**
     * Setting I2C devices
     */
    /* Init I2C controller for gyroscope, magnetometer and GSSB devices */
    sch_a3200_init_twi2();
    /* Init gyroscope */
    gs_mpu3300_init(GS_MPU3300_BW_5, GS_MPU3300_FSR_225);
    /* Init magnetometer */
    gs_hmc5843_init();


    /**
     * Init ADC and PWM driver
     */
    /* Setup ADC channels for current measurements */
    gs_a3200_adc_channels_init();
    /* Init PWM but still disable PWM switch*/
    gs_a3200_pwm_init();
    gs_a3200_pwr_switch_disable(GS_A3200_PWR_PWM);

    /**
     * Log reset cause
     */
    /* Log latest boot- and reset-cause. */
    gs_sys_reset_cause_t reset_cause;
    gs_sys_log_causes(NULL, &reset_cause, NULL);
    gs_sys_clear_reset_cause();
    dat_set_system_var(dat_obc_last_reset, (int)reset_cause);
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
    gs_a3200_led_init();
    gs_a3200_led_on(GS_A3200_LED_CPU_OK);
    gs_a3200_led_off(GS_A3200_LED_A);

    /* Reset & start watchdog - Will be re-initialized by SWWD when started later on */
    wdt_disable();
    wdt_clear();
    wdt_opt_t wdtopt = { .us_timeout_period = SCH_WDT_PERIOD*1000000 };
    wdt_enable(&wdtopt);

    /* Resetting/Clearing all interrupts, so they are disabled when booting a RAM image -
       It is also disabled in swload, but having it here will allow a RAM image to disable it regardless 
       if the SW in flash does */
    gs_sys_avr32_reset_all_interrupt_settings();

    /* Initialize interrupt handling.
       This function call is needed, when using libasf startup.S file */
    INTC_init_interrupts();

    /* Init CLK */
    sysclk_init();

    /* Init pwr channels */
    gs_a3200_pwr_switch_init();
    /* Disable all pwr switches */
    gs_a3200_pwr_switch_disable(GS_A3200_PWR_GSSB);
    gs_a3200_pwr_switch_disable(GS_A3200_PWR_GSSB2);
    gs_a3200_pwr_switch_disable(GS_A3200_PWR_PWM);
    gs_a3200_pwr_switch_enable(GS_A3200_PWR_SD);

    /* Init SDRAM, do this before malloc is called - It is configured already when booting from RAM */
    sdramc_init(sysclk_get_cpu_hz());

    /**
     * Set heap address to the external SDRAM, so malloc calls will use the
     * 32MB external SDRAM. Otherwise only the 68kB internal SRAM are available
     * @warning Do not touch this lines. See Gomspace framework docs and AVR32
     * application notes on SDRAM and Linker Scripts usages for further details.
     */
#if 1
    extern void *heap_start, *heap_end;
    heap_start = (void *)(0xD0000000 + 0x100000);
    heap_end = (void *)(0xD0000000 + 0x2000000 - 1000);
#endif

    sch_a3200_uart_init(2, true, SCH_UART_BAUDRATE);
    sch_a3200_uart_init(4, true, SCH_UART_BAUDRATE);

    /* Enable USART for printf, scanf */
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    /**
     * Test the external SDRAM. If working properly we are able to reserve this
     * large amount of memory and will use an address higher than 0xD0000000.
     * Otherwise, the internal SRAM is not big enough then malloc will raise an
     * error and the device will be reset.
     */
#if 0
    test_sdram(100000, 1);
#endif

    /* Finish on reset, both led on */
    gs_a3200_led_on(GS_A3200_LED_CPU_OK);
    gs_a3200_led_on(GS_A3200_LED_A);
}
