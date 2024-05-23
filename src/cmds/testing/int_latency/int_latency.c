#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <drivers/diag.h>
#include <drivers/gpio/gpio.h>

#define INT_PORT OPTION_GET(NUMBER,int_port)
#define INT_PIN (1 << OPTION_GET(NUMBER,int_pin))
#define OUT_PORT OPTION_GET(NUMBER,out_port)
#define OUT_PIN (1 << OPTION_GET(NUMBER,out_pin))

static void irq_hnd(void *data) {
	gpio_set(OUT_PORT, OUT_PIN, GPIO_PIN_HIGH);
	(*(int *) data) = 1;
}

int main() {
	int irq_handled = 0, irq_count = 0;

	gpio_setup_mode(OUT_PORT, OUT_PIN, GPIO_MODE_OUT);
	gpio_set(OUT_PORT, OUT_PIN, GPIO_PIN_LOW);

	gpio_setup_mode(INT_PORT, INT_PIN, GPIO_MODE_INT_MODE_RISING);
	if (0 > gpio_irq_attach(INT_PORT, INT_PIN, irq_hnd, &irq_handled)) {
		fprintf(stderr, "Failed to attach IRQ handler\n");
		return -1;
	}

	do {
		while (!irq_handled) {
			usleep(100);
		}
		irq_handled = 0;
		usleep(200000);
		gpio_set(OUT_PORT, OUT_PIN, GPIO_PIN_LOW);
		printf("%d\n", ++irq_count);
	} while (!diag_kbhit());

	gpio_irq_detach(INT_PORT, INT_PIN);

	return 0;
}
