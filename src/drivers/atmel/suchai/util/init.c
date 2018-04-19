#include <asf.h>

#include "init.h"
#include "conf_usart_serial.h"
#include "conf_spi_master.h"
#include "sysclk.h"

static const char *tag = "Initializer";

void serial_init(void)
{
	// This is a fairly stereotypical "ASF pattern" for enabling a peripheral.

	// Setup the UART Options
	static const usart_serial_options_t UART_SERIAL_OPTS = {
		.baudrate = UART_BAUDRATE,
		.charlength = UART_CHAR_LENGTH,
		.paritytype = UART_PARITY,
		.stopbits = UART_STOP_BIT
	};

	// You could do this with one function call per pin, but this is "tidier"
	static const gpio_map_t UART_GPIO_MAP = {
		{UART_RX_PIN, UART_RX_FUNC},
		{UART_TX_PIN, UART_TX_FUNC}
	};

	// Enable the clock, module pins, and configure the module.
	// Note that the module config also sets up stdin/stdout for you.
	sysclk_enable_peripheral_clock(UART_PORT);
	gpio_enable_module(UART_GPIO_MAP,
			   sizeof(UART_GPIO_MAP) / sizeof(UART_GPIO_MAP[0]));
	stdio_serial_init(UART_PORT, &UART_SERIAL_OPTS);
}

void spi_init(void)
{
    static const gpio_map_t DATA_OUT_SPI_GPIO_MAP = {
            {DATA_OUT_SPI_SCK_PIN,  DATA_OUT_SPI_SCK_FUNCTION },
            {DATA_OUT_SPI_MISO_PIN, DATA_OUT_SPI_MISO_FUNCTION},
            {DATA_OUT_SPI_MOSI_PIN, DATA_OUT_SPI_MOSI_FUNCTION},
            {DATA_OUT_SPI_NPCS0_PIN, DATA_OUT_SPI_NPCS0_FUNCTION},
    };
    
    gpio_enable_module(DATA_OUT_SPI_GPIO_MAP, sizeof(DATA_OUT_SPI_GPIO_MAP) / sizeof(DATA_OUT_SPI_GPIO_MAP[0]));


    spi_options_t my_spi_options={
            // The SPI channel to set up : Memory is connected to CS1
            DATA_OUT_SPI_NPCS,
            // Preferred baudrate for the SPI.
            DATA_OUT_SPI_BAUDRATE,
            // Number of bits in each character (8 to 16).
            8,
            // Delay before first clock pulse after selecting slave (in PBA clock periods).
            0,
            // Delay between each transfer/character (in PBA clock periods).
            0,
            // Sets this chip to stay active after last transfer to it.
            1,
            // Which SPI mode to use when transmitting.
            SPI_MODE_0,
            // Disables the mode fault detection.
            // With this bit cleared, the SPI master mode will disable itself if another
            // master tries to address it.
            1
    };

    //Init SPI module as master
    spi_initMaster(DATA_OUT_SPI, &my_spi_options);

    //Setup configuration for chip connected to CS1
    spi_setupChipReg(DATA_OUT_SPI, &my_spi_options, sysclk_get_pba_hz());

    //Allow the module to transfer data
    spi_enable(DATA_OUT_SPI);
}

void ssc_init(void *buffer, void *buffer2, __int_handler handler)
{
    //Mapping GPIO
    static const gpio_map_t SSC_GPIO_MAP =
            {
                    {AVR32_SSC_RX_CLOCK_0_0_PIN, AVR32_SSC_RX_CLOCK_0_0_FUNCTION},           // AVR32_PIN_PA28  GPIO3 J3.4
                    {AVR32_SSC_RX_DATA_0_0_PIN, AVR32_SSC_RX_DATA_0_0_FUNCTION},             // AVR32_PIN_PA27 GPIO4 J3.5
                    {AVR32_SSC_RX_FRAME_SYNC_0_0_PIN, AVR32_SSC_RX_FRAME_SYNC_0_0_FUNCTION}  // AVR32_PIN_PA20 ADC4  J2.5
            };

    volatile avr32_ssc_t *ssc = &AVR32_SSC;

    // Assign GPIO to SSC.
    gpio_enable_module(SSC_GPIO_MAP, sizeof(SSC_GPIO_MAP) / sizeof(SSC_GPIO_MAP[0]));

    /* Software reset SSC */
    ssc->cr = AVR32_SSC_CR_SWRST_MASK;

//    ssc->cmr = AVR32_SSC_CMR_DIV_NOT_ACTIVE << AVR32_SSC_CMR_DIV_OFFSET;  // Clock divider disabled
    ssc->cmr = 2 << AVR32_SSC_CMR_DIV_OFFSET;  // Clock divider (CLK_PBA / (2*CMR) => 24MHz / 2*4 = 3 MHz)

    ssc->tcmr = 0;  // Transmit clock mode, don't care
    ssc->tfmr = 0;  // Transmit frame mode, don't care
    /* Set receive clock mode:
           *  CKS - Uses RK pin, or generate a DIV clock
           *  CKO - No clock output,
           *  CKI - shift data on falling edge (0) or in rising edge (1),
           *  CKG - No clock output,
           *  START - Start with RF rising or fallin edge
           *  STTDLY - Cycles before start
           *  PERIOD - No FS generation
           */
    ssc->rcmr = (AVR32_SSC_RCMR_CKS_DIV_CLOCK      << AVR32_SSC_RCMR_CKS_OFFSET) |
//                (AVR32_SSC_RCMR_CKS_RK_PIN      << AVR32_SSC_RCMR_CKS_OFFSET) |
                (AVR32_SSC_RCMR_CKO_CONTINOUS_CLOCK_OUTPUT  << AVR32_SSC_RCMR_CKO_OFFSET) |
//                (AVR32_SSC_RCMR_CKO_INPUT_ONLY  << AVR32_SSC_RCMR_CKO_OFFSET) |
                (1                              << AVR32_SSC_RCMR_CKI_OFFSET) |
                (AVR32_SSC_RCMR_CKG_NONE        << AVR32_SSC_RCMR_CKG_OFFSET) |
                (AVR32_SSC_RCMR_START_DETECT_FALLING_RF << AVR32_SSC_RCMR_START_OFFSET) |
                (1                             << AVR32_SSC_RCMR_STTDLY_OFFSET)|
                (0                             << AVR32_SSC_RCMR_PERIOD_OFFSET);

    /* Set receive frame mode:
     *  DATLEN -TLEN: The bit stream contains (DATLEN + 1) data bits.
     *  LOOP - no loopback
     *  MSBF - msb first
     *  DATNB -  Number of words per frame equal to (DATNB + 1).
     *  FSLEN - no generate framesync
     *  FSOS - do not generate framesync
     *  FSEDGE - detect frame sync positive edge
     */
    ssc->rfmr =
            ((24-1)                         << AVR32_SSC_RFMR_DATLEN_OFFSET)|
            (0                              << AVR32_SSC_RFMR_LOOP_OFFSET)|
            (0                              << AVR32_SSC_RFMR_MSBF_OFFSET)|
            ((1-1)                          << AVR32_SSC_RFMR_DATNB_OFFSET)|
            (0                              << AVR32_SSC_RFMR_FSLEN_OFFSET)|
            (AVR32_SSC_RFMR_FSOS_INPUT_ONLY << AVR32_SSC_RFMR_FSOS_OFFSET)|
            (0                              << AVR32_SSC_RFMR_FSEDGE_OFFSET);

    cpu_irq_disable();
    INTC_register_interrupt(handler, AVR32_SSC_IRQ, AVR32_INTC_INT0);
    cpu_irq_enable();

    /* PDAC channel settings */
    const pdca_channel_options_t PDCA_OPTIONS = {
            /* Select peripheral - data is transmitted on SSC RX line */
            .pid = AVR32_PDCA_PID_SSC_RX,
            /* Select size of the transfer */
            .transfer_size = PDCA_TRANSFER_SIZE_WORD,

            /* Memory address */
            .addr = buffer,
            /* Transfer counter */
            .size = 1000,

            /* Next memory address */
            .r_addr = buffer2,
            /* Next transfer counter */
            .r_size = 1000,
    };

//    /* Initialize the PDCA channel with the requested options. */
//    pdca_init_channel(AVR32_PDCA_CH0EN, &PDCA_OPTIONS);
//
//    /* Register PDCA IRQ interrupt. */
//    cpu_irq_disable();
//    INTC_register_interrupt(handler, AVR32_PDCA_IRQ_0, AVR32_INTC_INT0);
//
//
//    /* Enable PDCA interrupt each time the reload counter reaches zero, i.e.
//     * each time half of the ASCII animation (either anim1 or anim2) is
//     * transferred. */
//    pdca_disable_interrupt_transfer_error(AVR32_PDCA_CH0EN);
//    pdca_disable_interrupt_reload_counter_zero(AVR32_PDCA_CH0EN);
//    pdca_enable_interrupt_transfer_complete(AVR32_PDCA_CH0EN);
//    cpu_irq_enable();
//
//    /* Enable now the transfer. */
//    pdca_enable(AVR32_PDCA_CH0EN);
}

void ssc_start(void)
{
    volatile avr32_ssc_t *ssc = &AVR32_SSC;
    ssc->cr = AVR32_SSC_CR_RXEN_MASK;
    ssc->ier = AVR32_SSC_IDR_RXRDY_MASK;
}

__attribute__((__interrupt__)) void rtc_irq(void)
{
    //Checking the interrupt execution
    LED_Toggle(LED2);
    // Increment the minutes counter
    dat_update_time();
    // clear the interrupt flag
    rtc_clear_interrupt(&AVR32_RTC);

    // specify that an interrupt has been raised
    //print_sec = 1;
}

void twi_init(void) {
    sysclk_init();
    /* GPIO map setup */
    const gpio_map_t TWIM_GPIO_MAP = {
            {AVR32_TWIMS0_TWCK_0_0_PIN, AVR32_TWIMS0_TWCK_0_0_FUNCTION},
            {AVR32_TWIMS0_TWD_0_0_PIN, AVR32_TWIMS0_TWD_0_0_FUNCTION}
    };
    gpio_enable_module(TWIM_GPIO_MAP,
                       sizeof(TWIM_GPIO_MAP) / sizeof(TWIM_GPIO_MAP[0]));

    /* Init twi master controller 2 with addr 5 and 100 kHz clock */
    i2c_init_master(0, 5, 100);
}

void on_close(int signal)
{
    dat_repo_close();

    LOGI(tag, "Exit system!");
    exit(signal);
}

void on_reset(void)
{
    /* LED's init */
    serial_init();
    LED_On(LED0);
    LED_On(LED1);
    LED_On(LED2);
    LED_On(LED3);

    /* Interrupts init */
    // Disable all interrupts. */
    Disable_global_interrupt();

    // The INTC driver has to be used only for GNU GCC for AVR32.

    // Initialize interrupt vectors.
    INTC_init_interrupts();

    // Register the RTC interrupt handler to the interrupt controller.
    INTC_register_interrupt(&rtc_irq, AVR32_RTC_IRQ, AVR32_INTC_INT0);

    /* RTC init */
    rtc_init(&AVR32_RTC, RTC_OSC_32KHZ, RTC_PSEL_32KHZ_1HZ);
    // Set top value to 0 to generate an interrupt every seconds */
    rtc_set_top_value(&AVR32_RTC, 0);
    // Enable the interrupts
    rtc_enable_interrupt(&AVR32_RTC);
    // Enable the RTC
    rtc_enable(&AVR32_RTC);

    // Enable global interrupts
    Enable_global_interrupt();
}
