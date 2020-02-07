/**
 * @file
 * @brief
 *
 * @date    29.03.2017
 * @author  Alex Kalmuk
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <drivers/sensors/gy_30.h>

static void print_lx(int i2c_nr, uint8_t mode) {
	uint16_t level;

	printf("GY-30 running\n");

	if (mode == BH1750_CONTINUOUS_HIGH_RES_MODE) {
		gy_30_setup_mode(i2c_nr, mode);
	}

	while (1) {
		if (mode == BH1750_ONE_TIME_HIGH_RES_MODE) {
			gy_30_setup_mode(i2c_nr, mode);
		}
		level = gy_30_read_light_level(i2c_nr);
		printf("level: %d lx\n", level);
		sleep(1);
	}
}

static void print_usage(void) {
	printf("Usage: gy_30 [-h] -s <i2c_nr> -m <mode>\n"
			"Example:\n"
			"  gy_30 -s 1 -m continuous_hres\n"
			"  gy_30 -s 1 -m onetime_hres\n");
}

int main(int argc, char *argv[]) {
	int opt;
	int i2c_nr, mode;
	char mode_str[32];

	while (-1 != (opt = getopt(argc, argv, "hs:m:"))) {
		printf("\n");
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 's':
			if ((optarg == NULL) || (!sscanf(optarg, "%d", &i2c_nr))) {
				print_usage();
				return -EINVAL;
			}
			break;
		case 'm':
			if ((optarg == NULL) || (!sscanf(optarg, "%s", mode_str))) {
				print_usage();
				return -EINVAL;
			}
			if (!strcmp(mode_str, "continuous_hres")) {
				mode = BH1750_CONTINUOUS_HIGH_RES_MODE;
			} else if (!strcmp(mode_str, "onetime_hres")) {
				mode = BH1750_ONE_TIME_HIGH_RES_MODE;
			} else {
				print_usage();
				return -EINVAL;
			}
			break;
		default:
			print_usage();
			return -EINVAL;
		}
	}

	if (argc < 5) {
		print_usage();
		return 0;
	}

	print_lx(i2c_nr, mode);

	return 0;
}
