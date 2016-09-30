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

#include <drivers/gpio.h>

static void print_usage(void) {
	printf("USAGE:\npin [pin name] [pin index] [state]\n");
	// TODO: add examples
}

// See, for example, stm32f4xx_hal_gpio.h. Single pin maps to single bit.
// 15 pins are set means 0xFFFF. No pins set means 0x0000.
#define PIN_UNSPECIFIED (0)

/**
 * @brief [pin_index_of_int n] converts index [n] of pin to concrete GPIO_PIN_[n].
 *        Returns 0 on fail.
 */
static uint16_t pin_index_of_int(int n) {
	switch(n) {
	case 0:		return GPIO_PIN_0;
	case 1:		return GPIO_PIN_1;
	case 2:		return GPIO_PIN_2;
	case 3:		return GPIO_PIN_3;
	case 4:		return GPIO_PIN_4;
	case 5:		return GPIO_PIN_5;
	case 6:		return GPIO_PIN_6;
	case 7:		return GPIO_PIN_7;
	case 8:		return GPIO_PIN_8;
	case 9:		return GPIO_PIN_9;
	case 10:	return GPIO_PIN_10;
	case 11:	return GPIO_PIN_11;
	case 12:	return GPIO_PIN_12;
	case 13:	return GPIO_PIN_13;
	case 14:	return GPIO_PIN_14;
	case 15:	return GPIO_PIN_15;
	default:	return 0;
	}
}

static uint16_t pin_by_name(char *name) {
	char *end;
	int n = strtol(name, &end, 10);
	if (*end == '\0') {
		return pin_index_of_int(n);
	} else {
		fprintf(stderr, "Can't extract pin index from string '%s'\n", name);
		return PIN_UNSPECIFIED;
	}
}

/**
 * @brief gpio_by_name Parses GPIO and (maybe) pin index from input string.
 * @param name Input string to parse
 * @param pin  Output parameter. If pin is specified contains index of specified
 *             pin. If pin is not specified contains PIN_UNSPECIFIED.
 *             If function fails result value is not specified.
 * @return NULL when can't parse input and GPIO on success.
 */
static void* gpio_by_name(char *name, gpio_mask_t *pin) {
	if (!strncmp("GPIO", name, 4) || !strncmp("gpio", name, 4))
		name += 4;

	if (strlen(name) == 0) {
		return 0;
	} else if (strlen(name)>1) {
		// extract pin. atoi doesn't bother about errors so we use strtol
		char *end;
		assert(name[0] != '\0');
		int pin_index = strtol(name+1, &end, 10);
		if (*end == '\0') {
			*pin = pin_index_of_int(pin_index);
		} else {
			fprintf(stderr, "Can't extract pin index from string '%s'.\n", name+1);
			*pin = PIN_UNSPECIFIED;
			return 0;
		}
	} else {
		pin = PIN_UNSPECIFIED;
	}

	switch (name[0]) {
	case 'a':	case 'A': return GPIOA;
	case 'b':	case 'B': return GPIOB;
	case 'c':	case 'C': return GPIOC;
	case 'd':	case 'D': return GPIOD;
	case 'e':	case 'E': return GPIOE;
	case 'f':	case 'F': return GPIOF;
	}

	fprintf(stderr, "Can't convert character '%c' to GPIO. Error.", name[0]);
	return NULL;
}

int extract_level(int pos, int argc, char **argv) {
	if (pos >= argc)
		// we suppose default inspecified value is `set`
		return GPIO_PIN_SET;
	else {
		// we should check argv[pos]
		if (!strcmp("set", argv[pos]))
			return GPIO_PIN_SET;
		else if (!strcmp("reset", argv[pos]))
			return GPIO_PIN_RESET;
		else if (!strcmp("unset", argv[pos]))
			return GPIO_PIN_RESET;
		else {
			printf("Unknown pin state: \"%s\"\n", argv[pos]);
			exit(1);
		}
	}
}

static void apply(struct gpio *gpio, gpio_mask_t pin, int level) {
	gpio_set_level(gpio, pin, level);
}

int main(int argc, char **argv) {
	int opt;
	int level;
	void *gpio;
	gpio_mask_t pin;

	while (-1 != (opt = getopt(argc, argv, "Rlh"))) {
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
	
	if (argc < 2) {
		print_usage();
		return 0;
	}

	opt = 1;
	if (strlen(argv[opt]) > 1) {
		// [char]something
		gpio = gpio_by_name(argv[opt], &pin);
		if (gpio) {
			if (pin != PIN_UNSPECIFIED) {
				int level = extract_level(opt+1, argc, argv);
				apply(gpio, pin, level);
				return 0;
			} else {
				fprintf(stderr, "Can't extract pin index.");
				exit(1);
			}
		} else {
			fprintf(stderr, "Can't extract GPIO.");
			return 0;
		}
	} else {
		gpio = gpio_by_name(argv[opt], &pin);
		if (gpio) {
			opt++;
			if (opt >= argc) {
				fprintf(stderr, "Unsufficient arguments.");
				return 0;
			}
			pin = pin_by_name(argv[opt]);
			if (pin != PIN_UNSPECIFIED) {
				int level = extract_level(opt+1, argc, argv);
				apply(gpio, pin, level);
				return 0;
			} else {
				fprintf(stderr, "Can't extract pin index.");
				exit(1);
			}
		} else {
			fprintf(stderr, "Can't extract GPIO.");
			return 0;
		}
	}

	opt++;
	if (opt >= argc) {
		fprintf(stderr, "Unsufficient arguments.");
		return 0;
	}
	if (PIN_UNSPECIFIED == (pin = pin_by_name(argv[opt]))) {
		fprintf(stderr, "Pin index is unspecified.");
		return 0;
	}
	opt++;
	level = extract_level(opt, argc, argv);
	apply(gpio, pin, level);

	return 0;
}
