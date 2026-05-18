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

ARRAY_SPREAD_DECLARE(const struct pwm_device, __pwm_device_registry);

static void print_usage(void) {
	printf("Usage:\n");
	printf("\tpwm [-h] - print usage\n");
	printf("\tpwm - Print list of PWM devices\n");
	printf("\tpwm <pwm_id> <period> <duty_circle> - start PWM\n");
	printf("\tpwm <pwm_id> -p - stop PWM\n");
}

static void print_pwm_list(void) {
	struct pwm_device *pwm_dev;

	printf("PWM's list:\n");
	array_spread_foreach_ptr(pwm_dev, __pwm_device_registry) {
		int i;
		printf("\tPWM id(%d): base_addr(0x%" PRIxPTR ") period(%d)\n",
			pwm_dev->pwmd_id, pwm_dev->pwmd_base_addr, pwm_dev->pwmd_period);
		for (i = 0; i < pwm_dev_max_chan(pwm_dev); i++) {
			if (pwm_dev->pwmd_avail_chan_mask & (1 << i)) {
				printf("\t\tchan(%d): duty(%d) out_pin(PORT%d.%d)\n",
						i, pwm_dev->pwmd_duty[i],
						pwm_dev->pwmd_pin[i].pd_port, pwm_dev->pwmd_pin[i].pd_pin);
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

	if (argc == 1) {
		print_pwm_list();
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "hpl"))) {
		switch (opt) {
		case '?':
			printf("Invalid command line option\n");
			/* FALLTHROUGH */
		case 'h':
			print_usage();
			return 0;
		case 'l':
			print_pwm_list();
			return 0;
		case 'p':
			stop = 1;
			break;
		default:
			break;
		}
	}

	id = atoi(argv[1]);
	pwm_dev = pwm_dev_by_id(id);
	if (pwm_dev == NULL) {
		printf("Invalid PWM ID %d\n", id);
		print_pwm_list();
		return -EINVAL;
	}

	if (stop) {
		pwm_disable(pwm_dev, 0xFF);
		printf("Diasbled PWM ID %d\n", id);
	} else {
		int period;
		int duty;

		period = atoi(argv[2]);
		duty = atoi(argv[3]);

		if (period  < 1 || duty < 2 || period < duty) {
			printf("Invalid args period(%d) duty(%d)\n", period, duty);
			return -EINVAL;
		}

		pwm_disable(pwm_dev, 0xFF);

		err = pwm_set_period(pwm_dev, period);
		if (err) {
			return err;
		}
		err = pwm_set_duty(pwm_dev, 0, duty);
		if (err) {
			return err;
		}

		pwm_enable(pwm_dev, 0xFF);

		printf("Enabled PWM ID %d period(%d) duty(%d)\n", id, period, duty);
	}

	return 0;
}
