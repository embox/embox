/**
 * @file
 * @brief timer test -- blinking led
 *
 * @date 01.10.2010
 * @author Anton Kozlov
 */

#include <embox/test.h>
//#include <drivers/at91_olimex_debug_led.h>
#include <drivers/at91sam7s256.h>
#include <unistd.h>

EMBOX_TEST(blinking_led);

/**
 * The test itself.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
#define DELAY 0x0;
#define INC 0x1;

void delay(int d) {
	int i = 0;
	while (i < d) {
		i += 1;
	}
}

extern void led1_on(void);
extern void led1_off(void);

static int blinking_led(void) {
	volatile int del = DELAY;

	while (1) {
		led1_on();
		delay(del);
		//sleep(1);
		led1_off();
		delay(del);
		//sleep(1);
		del += INC;
	}

	return 0;
}
