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
#include <unistd.h>
#include <stdio.h>

#include <drivers/pwm.h>

ARRAY_SPREAD_DECLARE(struct pwm_device *, __pwm_device_registry);

static void print_usage(void) {
	printf("Usage:\n");
	printf("\tpwm [-h] - print usage\n");
	printf("\tpwm -i - Print info of PWM devices\n");
	printf("\tpwm -p <pwm_id> <period> - setup period\n");
	printf("\tpwm -d <pwm_id> <chan id> <duty> - setup duty and start\n");
	printf("\tpwm -D <pwm_id> - stop PWM\n");
}

static void print_pwm_list(void) {
	struct pwm_device *pwm_dev;

	printf("PWM's list:\n");
	array_spread_foreach(pwm_dev, __pwm_device_registry) {
		int i;
		const struct pwm_desc *pwmd_desc;
	
		pwmd_desc = pwm_dev->pwmd_desc;
	
		printf("\tPWM id(%d): base_addr(0x%" PRIxPTR ") period(%d)\n",
			pwm_dev->pwmd_id, pwmd_desc->pwmd_base_addr, pwm_dev->pwmd_period);
		for (i = 0; i < pwm_dev_max_chan(pwm_dev); i++) {
			if (pwmd_desc->pwmd_avail_chan_mask & (1 << i)) {
				printf("\t\tchan(%d): duty(%d) out_pin(PORT%d.%d)\n",
						i, pwm_dev->pwmd_duty[i],
						pwmd_desc->pwmd_pin[i].pd_port,
						pwmd_desc->pwmd_pin[i].pd_pin);
			}

		}
	}
}

int main(int argc, char **argv) {
	int opt;
	struct pwm_device *pwm_dev;
	int id;
	int stop = 0;
	int err;
	int period = 0;
	int duty = 0;

	if (argc == 1) {
		print_pwm_list();
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "hpipdD"))) {
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
		case 'D':
			stop = 1;
			break;
		case 'p':
			period = 1;
			break;
		case 'd':
			duty = 1;
			break;
		default:
			print_usage();
			return 0;
		}
	}

	id = atoi(argv[2]);
	pwm_dev = pwm_dev_by_id(id);
	if (pwm_dev == NULL) {
		printf("Invalid PWM ID %d\n", id);
		print_pwm_list();
		return -EINVAL;
	}

	if (stop) {
		pwm_disable(pwm_dev, 0xFF);
		printf("Diasbled PWM ID %d\n", id);
		return 0;
	}

	if (period) {
		period = atoi(argv[3]);


		err = pwm_set_period(pwm_dev, period);
		if (err) {
			return err;
		}

		printf("Set period PWM ID %d period(%d)\n", id, period);

		return 0;
	}

	if (duty) {
		int chan =  atoi(argv[3]);

		duty = atoi(argv[4]);


		pwm_disable(pwm_dev, 0xFF);
	
		err = pwm_set_duty(pwm_dev, chan, duty);
		if (err) {
			return err;
		}

		pwm_enable(pwm_dev, 0xFF);

		printf("Enabled PWM ID %d chan(%d) duty(%d)\n", id, chan, duty);
	}

	return 0;
}
