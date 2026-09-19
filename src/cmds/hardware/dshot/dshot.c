/**
 * @file
 * @brief 
 *
 * @date 11.06.2025
 * @author Anton Bondarev
 */

#include <errno.h>

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include <drivers/pwm.h>
#include <drivers/motor.h>

#include "dshot.h"

ARRAY_SPREAD_DECLARE(struct motor_dev *, __motor_device_registry);

static uint16_t dshot_prepare_packet(uint16_t value, int telemetry) {
	uint16_t frame;
	uint16_t crc;

	frame = (value << 1) | (telemetry ? 1 : 0);

	crc = (frame ^ (frame >> 4) ^ (frame >> 8)) & 0x0F;
	frame = (frame << 4) | crc;

	return frame;
}

static void print_usage(void) {
	printf("Usage:\n");
	printf("\tdshot [-h] - print usage\n");
	printf("\tdshot -i - Print info of dshot devices\n");
	printf("\tdshot <command> <data (11 + 1 bits)> -m motor_id\n");	
}

static int dshot_beep1_send(struct motor_dev *motor_dev) {
	uint16_t frame;
	struct motor_msg msg = {0};

	frame = dshot_prepare_packet(DSHOT_CMD_BEEP1, 0);
	msg.mm_buf = &frame;
	msg.mm_len = 2;

	motor_send_msg(motor_dev, &msg);

	return 0;
}

static void print_pwm_list(void) {
	struct motor_dev *dev;

	printf("motor's list:\n");
	array_spread_foreach(dev, __motor_device_registry) {
		printf("motor(%d): pwm(%d) chan(%d) period(%d nS)\n", dev->md_id,
						dev->md_pwm_id, dev->md_pwm_chan, dev->md_period);
	}
}


int main(int argc, char **argv) {
	int opt;
	struct motor_dev *motor_dev;
	int id = -1;

	if (argc == 1) {
		print_usage();
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "him:"))) {
		switch (opt) {
		case '?':
			printf("Invalid command line option\n");
			/* FALLTHROUGH */
		case 'h':
			print_usage();
			return 0;
		case 'i':
			print_pwm_list();
			return 0;
		case 'm':
			id = atoi(optarg);
			break;
		default:
			print_usage();
			return 0;
		}
	}

	if (id == -1) {
		printf("Setup motor (PWM ID)\n");
		print_usage();
		return 0;
	}

	motor_dev = motor_dev_by_id(id);
	if (motor_dev == NULL) {
		printf("Invalid PWM ID %d\n", id);
		print_pwm_list();
		return -EINVAL;
	}

	if (!motor_dev->md_pwm_dev->pwmd_period) {
		printf("Setup PWM period for %d\n", motor_dev->md_pwm_id);
		return 0;
	}

	if (strcmp(argv[2], "beep1")) {
		dshot_beep1_send(motor_dev);
	}

	return 0;
}
