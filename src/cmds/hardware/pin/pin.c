/**
 * @file pin.c
 * @brief Control pin voltage
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-02-10
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <drivers/gpio.h>

static void print_usage(void) {
	printf("USAGE:\npin <gpio> <pin> <state>\n"
		" Examples:\n"
		"    pin GPIO1 12 get         -- Get value of pin number 12 at GPIO1\n"
		"    pin GPIOA 12 set         -- Up pin number 12 at GPIOA\n"
		"    pin GPIOa 12 reset       -- Down pin number 12 at GPIOA\n"
		"    pin GPIO0 12 unset       -- Down pin number 12 at GPIO0 (GPIOA)\n"
		"    pin GPIOA 12 toggle      -- Reverts pin number 12 at GPIOA\n"
		"    pin GPIOA 12 blink       -- Toggle pin number 12 at GPIOA\n"
		"    pin GPIOA 12 irq rising  -- Test irq in RISING mode\n"
		"    pin GPIOA 12 irq falling -- Test irq in FALLING mode\n"
		"    pin GPIOA 12 irq both    -- Test irq in RISING_FALLING mode\n");
}

void pin_irq_hnd(void *data) {
	(*(int *) data)++;
}

static int gpio_by_name(const char *name) {
	int id = -1;

	if (strncmp("GPIO", name, 4) && strncmp("gpio", name, 4)) {
		return -1;
	}
	name += 4;
	if (!strlen(name)) {
		return -1;
	}
	if (toupper(name[0]) >= 'A' && toupper(name[0]) <= 'Z') {
		if (strlen(name) > 1) {
			return -1;
		}
		id = toupper(name[0]) - 'A';
	} else {
		if (1 != sscanf(name, "%d", &id)) {
			return -1;
		}
	}
	return id;
}

int main(int argc, char **argv) {
	int opt;
	int gpio;
	int arg;
	gpio_mask_t pin;

	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		default:
			printf("Unknown option\n");
			print_usage();
			return 0;
		}
	}

	if (argc < 4) {
		print_usage();
		return 0;
	}
	opt = 1;

	gpio = gpio_by_name(argv[opt]);
	if (gpio == -1) {
		fprintf(stderr, "Unknown GPIO: %s\n", argv[opt]);
		return 0;
	}
	opt++;

	if (!sscanf(argv[opt++], "%d", &arg)) {
		fprintf(stderr, "Pin is not specified.\n");
		return 0;
	}
	pin = (1 << arg);

	if (!strcmp("toggle", argv[opt])) {
		gpio_setup_mode(gpio, pin, GPIO_MODE_OUT);
		gpio_toggle(gpio, pin);
	} else if (!strcmp("set", argv[opt])) {
		gpio_setup_mode(gpio, pin, GPIO_MODE_OUT);
		gpio_set(gpio, pin, GPIO_PIN_HIGH);
	} else if (!strcmp("reset", argv[opt]) || !strcmp("unset", argv[opt])) {
		gpio_setup_mode(gpio, pin, GPIO_MODE_OUT);
		gpio_set(gpio, pin, GPIO_PIN_LOW);
	} else if (!strcmp("get", argv[opt])) {
		gpio_mask_t ret = gpio_get(gpio, pin);
		if (pin & ret) {
			printf("high\n");
		} else {
			printf("low\n");
		}
	} else if (!strcmp("blink", argv[opt])) {
		gpio_setup_mode(gpio, pin, GPIO_MODE_OUT);
		opt++;
		int sleep_time = atoi(argv[opt]);
		while (1) {
			gpio_toggle(gpio, pin);
			// sleep(1);
			sleep(sleep_time);
		}
	} else if (!strcmp("irq", argv[opt])) {
		int pin_irq_handled = 0, expected = 0;

		opt++;

		if (!strcmp("rising", argv[opt])) {
			gpio_setup_mode(gpio, pin, GPIO_MODE_INT_MODE_RISING);
			expected = 1; /* Single rising IRQ */
		} else if (!strcmp("falling", argv[opt])) {
			gpio_setup_mode(gpio, pin, GPIO_MODE_INT_MODE_FALLING);
			expected = 1; /* Single falling IRQ */
		} else if (!strcmp("both", argv[opt])) {
			gpio_setup_mode(gpio, pin, GPIO_MODE_INT_MODE_RISING_FALLING);
			expected = 2; /* One rising and one falling IRQs */
		} else {
			fprintf(stderr, "Unknown IRQ mode \"%s\"\n", argv[opt]);
			return -1;
		}

		if (0 > gpio_irq_attach(gpio, pin, pin_irq_hnd, &pin_irq_handled)) {
			fprintf(stderr, "Failed to attach IRQ handler\n");
			return -1;
		}

		while (pin_irq_handled < expected) {
			usleep(100 * 1000);
		}

		printf("GPIO interrupts handled!\n");

		gpio_irq_detach(gpio, pin);
	} else {
		fprintf(stderr, "Unknown pin state: \"%s\"\n", argv[opt]);
	}

	return 0;
}
