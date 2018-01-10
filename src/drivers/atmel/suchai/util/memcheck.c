#include <asf.h>
#include <stdio.h>
#include <stdint.h>

#include "util.h"

// Hacked from Atmel's sdram example

#define LED_SDRAM_WRITE     LED0
#define LED_SDRAM_READ      LED0
#define LED_SDRAM_ERRORS    LED0
#define LED_SDRAM_OK        LED0

uint32_t memcheck(void)
{
	volatile unsigned long *sdram = SDRAM;
	unsigned long sdram_size, progress_inc, i, j, tmp, noErrors = 0;
	sdram_size = SDRAM_SIZE >> 2;
	printf("\x0CSDRAM size: %d MB\r\n", SDRAM_SIZE >> 20);

	// Determine the increment of SDRAM word address requiring an update
	// of the printed progression status.
	progress_inc = (sdram_size + 50) / 100;

	// Fill the SDRAM with the test pattern.
	for (i = 0, j = 0; i < sdram_size; i++) {
		if (i == j * progress_inc) {
			LED_Toggle(LED_SDRAM_WRITE);
			printf("\rFilling SDRAM with test pattern: %lu%%", j++);
		}
		sdram[i] = i;
		tmp = sdram[i];

		//printf("Wrote: %lu, Read: %lu\n", i, tmp);
	}
	LED_Off(LED_SDRAM_WRITE);
	printf("\rSDRAM filled with test pattern       \r\n");

	// Recover the test pattern from the SDRAM and verify it.
	for (i = 0, j = 0; i < sdram_size; i++) {
		if (i == j * progress_inc) {
			LED_Toggle(LED_SDRAM_READ);
			printf("\rRecovering test pattern from SDRAM: %lu%%",
			       j++);
		}
		tmp = sdram[i];
		if (tmp != i) {
			noErrors++;
		}
	}
	LED_Off(LED_SDRAM_READ);
	printf("\rSDRAM tested: %lu corrupted word(s)          \r\n", noErrors);

	return noErrors;
}
