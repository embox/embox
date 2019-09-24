/**
 * @file pin.c
 * @brief Control pin voltage
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-02-10
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <drivers/gpio/gpio.h>

static void print_usage(void) {
	printf("USAGE:\npin <gpio> <pin> <state>\n"
		" Examples:\n"
		"    pin GPIO1 12 get    -- Get value of pin number 12 at GPIO1\n"
		"    pin GPIOA 12 set    -- Up pin number 12 at GPIOA\n"
		"    pin GPIOa 12 reset  -- Down pin number 12 at GPIOA\n"
		"    pin GPIO0 12 unset  -- Down pin number 12 at GPIO0 (GPIOA)\n"
		"    pin GPIOA 12 toggle -- Reverts pin number 12 at GPIOA\n"
		"    pin GPIOA 12 blink  -- Toggle pin number 12 at GPIOA\n");
}

static int gpio_by_name(char *name) {
	if (!strncmp("GPIO", name, 4) || !strncmp("gpio", name, 4)) {
		name += 4;
	}

	if (strlen(name) == 0) {
		return -1;
	}

	switch (name[0]) {
	case '0': case 'a': case 'A': return GPIO_PORT_A;
	case '1': case 'b': case 'B': return GPIO_PORT_B;
	case '2': case 'c': case 'C': return GPIO_PORT_C;
	case '3': case 'd': case 'D': return GPIO_PORT_D;
	case '4': case 'e': case 'E': return GPIO_PORT_E;
	case '5': case 'f': case 'F': return GPIO_PORT_F;
	default:
		fprintf(stderr, "Can't convert character '%c' to GPIO. Error.",
			name[0]);
	}

	return -1;
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

	gpio = gpio_by_name(argv[opt++]);
	if (gpio == -1) {
		fprintf(stderr, "Can't extract GPIO.\n");
		return 0;
	}

	if (!sscanf(argv[opt++], "%d", &arg)) {
		fprintf(stderr, "Pin is not specified.\n");
		return 0;
	}
	pin = (1 << arg);

	if (!strcmp("toggle", argv[opt])) {
		gpio_setup_mode(gpio, pin, GPIO_MODE_OUTPUT);
		gpio_toggle(gpio, pin);
	} else if (!strcmp("set", argv[opt])) {
		gpio_setup_mode(gpio, pin, GPIO_MODE_OUTPUT);
		gpio_set(gpio, pin, GPIO_PIN_HIGH);
	} else if (!strcmp("reset", argv[opt]) || !strcmp("unset", argv[opt])) {
		gpio_setup_mode(gpio, pin, GPIO_MODE_OUTPUT);
		gpio_set(gpio, pin, GPIO_PIN_LOW);
	} else if (!strcmp("get", argv[opt])) {
		gpio_mask_t ret = gpio_get(gpio, pin);
		if (pin & ret) {
			printf("high\n");
		} else {
			printf("low\n");
		}
	} else if (!strcmp("blink", argv[opt])) {
		gpio_setup_mode(gpio, pin, GPIO_MODE_OUTPUT);
		while (1) {
			gpio_toggle(gpio, pin);
			sleep(1);
		}
	} else {
		fprintf(stderr, "Unknown pin state: \"%s\"\n", argv[opt]);
	}

	return 0;
}
