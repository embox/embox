/**
 * @file
 *
 * @author Anton Bondarev
 * @date 13.06.24
 */

#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <drivers/gpio/gpio.h>

#include <framework/mod/options.h>

#define IN_PORT       OPTION_GET(NUMBER,in_port)
#define IN_PIN_MASK  (1 << OPTION_GET(NUMBER,in_pin))
#define OUT_PORT      OPTION_GET(NUMBER,out_port)
#define OUT_PIN_MASK (1 << OPTION_GET(NUMBER,out_pin))

static void irq_hnd(void *done) {
	gpio_set(OUT_PORT, OUT_PIN_MASK, GPIO_PIN_HIGH);
	(*(volatile int *) done) = 1;
}

int main(void) {
	volatile int done;

	gpio_setup_mode(IN_PORT, IN_PIN_MASK, GPIO_MODE_INT_MODE_RISING);
	if (0 > gpio_irq_attach(IN_PORT, IN_PIN_MASK, irq_hnd, (void *)&done)) {
		printf("couldn't attach IRQ for port(%d) pin(%d)\n",
							IN_PORT, OPTION_GET(NUMBER,in_pin) );
		return -EBUSY;
	}

	gpio_setup_mode(OUT_PORT, OUT_PIN_MASK, GPIO_MODE_OUT);
	gpio_set(OUT_PORT, OUT_PIN_MASK, GPIO_PIN_LOW);
	usleep(200000);

	printf("Start waiting rising on port(%d) pin(%d)\n", 
						IN_PORT, OPTION_GET(NUMBER,in_pin));
	printf("Output is on port(%d) pin(%d)\n",
						OUT_PORT, OPTION_GET(NUMBER,out_pin));

	done = 0;
	while (!done) {
		usleep(100);
	}

	usleep(200000); /* 0.2 Sec */
	gpio_set(OUT_PORT, OUT_PIN_MASK, GPIO_PIN_LOW);

	printf("Test finished\n");
	gpio_irq_detach(IN_PORT, IN_PIN_MASK);

	return 0;
}
