/**
 * @file
 * @brief Controlling leds on Olimex Debug board
 *
 * @date 26.09.10
 * @author Anton Kozlov
 */

#include <hal/reg.h>
#include <types.h>
#include <drivers/at91sam7_pio.h>
#include <embox/unit.h>

#if 0
#define PIO_PER	  0xfffff400
#define PIO_PDR	  0xfffff404
#define PIO_OER	  0xfffff410
#define PIO_SODR  0xfffff430
#define PIO_CODR  0xfffff434
#endif

EMBOX_UNIT_INIT(led_init);

static int led_init(void) {
	REG_STORE(AT91C_PIOA_PER, AT91C_PIO_PA17 | AT91C_PIO_PA18);
	REG_STORE(AT91C_PIOA_OER, AT91C_PIO_PA17 | AT91C_PIO_PA18);
	REG_STORE(AT91C_PIOA_SODR, AT91C_PIO_PA17 | AT91C_PIO_PA18);

#if 0
	*AT91C_PIOA_OWDR = AT91C_PA17_TD;
	*AT91C_PIOA_MDDR = AT91C_PA17_TD;
	//*AT91C_PIOA_PPUDR = AT91C_PA17_TD;
	//*AT91C_PIOA_IFDR = AT91C_PA17_TD;
	//*AT91C_PIOA_CODR = AT91C_PA17_TD;
	*AT91C_PIOA_IDR = AT91C_PA17_TD;
#endif
	return 0;
}

void led1_on(void) {
	REG_STORE(AT91C_PIOA_CODR, AT91C_PIO_PA17);
}

void led1_off(void) {
	REG_STORE(AT91C_PIOA_SODR, AT91C_PIO_PA17);
}

void led2_on(void) {
	REG_STORE(AT91C_PIOA_CODR, AT91C_PIO_PA18);
}

void led2_off(void) {
	REG_STORE(AT91C_PIOA_SODR, AT91C_PIO_PA18);
}

/* funny functions reperesents ints binary by leds */
#if 0
void delay(int n) {
	int i = n;
	while (i) {
		i -= 1;
	}
}

/* 'print' a, assuming it takes not more than n binary digits */
void print(int n, int s) {
	int a = n;
	int i;
	for (i = 0; i < s; i++) {
		if (a & 1) {
			led1_on();
		} else {
			led1_off();
		}
		led2_on();
		delay(0xf0000);
		led2_off();
		delay(0x200000);
		a >>= 1;
	}
}
#endif
