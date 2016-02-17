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

#include <drivers/gpio.h>

static void print_usage(void) {
	printf("USAGE:\npin [pin name] [state]\n");
}

static void *gpio_by_name(char *name) {
	if (!strncmp("GPIO", name, 4) || !strncmp("gpio", name, 4))
		name += 4;

	if (name[0] == 'a' || name[0] == 'A')
		return GPIOA;
	else if (name[0] == 'b' || name[0] == 'B')
		return GPIOB;
	else if (name[0] == 'c' || name[0] == 'C')
		return GPIOC;
	else if (name[0] == 'd' || name[0] == 'D')
		return GPIOD;
	else if (name[0] == 'e' || name[0] == 'E')
		return GPIOE;
	else if (name[0] == 'f' || name[0] == 'F')
		return GPIOF;
	
	return NULL;
}

static uint16_t pin_by_name(char *name) {
	int n = atoi(name);
	switch(n) {
	case 0:
		return GPIO_PIN_0;
	case 1:
		return GPIO_PIN_1;
	case 2:
		return GPIO_PIN_2;
	case 3:
		return GPIO_PIN_3;
	case 4:
		return GPIO_PIN_4;
	case 5:
		return GPIO_PIN_5;
	case 6:
		return GPIO_PIN_6;
	case 7:
		return GPIO_PIN_7;
	case 8:
		return GPIO_PIN_8;
	case 9:
		return GPIO_PIN_9;
	case 10:
		return GPIO_PIN_10;
	case 11:
		return GPIO_PIN_11;
	case 12:
		return GPIO_PIN_12;
	case 13:
		return GPIO_PIN_13;
	case 14:
		return GPIO_PIN_14;
	case 15:
		return GPIO_PIN_15;
	default:
		return 0;
	}
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
	
	if (argc < 3) {
		print_usage();
		return 0;
	}

	gpio = gpio_by_name(argv[1]);

	if (gpio == NULL) {
		printf("Wrong port name: %s\n", argv[1]);
		return 0;
	}

	pin = pin_by_name(argv[2]);

	if (argc < 4) {
		level = 1;
	} else {
		if (!strcmp("set", argv[3]))
			level = 1;
		else if (!strcmp("reset", argv[3]))
			level = 0;
		else {
			printf("Unknown pin state: %s\n", argv[3]);
		}
	}
	

	gpio_set_level(gpio, pin, level);

	return 0;
}
