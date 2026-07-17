/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 16.07.26
 */

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <drivers/can_dev.h>
#include <drivers/char_dev.h>

static void print_help(void) {
	printf("Usage:\n");
	printf(" canconfig <device>\n");
	printf(" canconfig <device> <options>\n");
	printf(" canconfig <device> [restart]\n");
	printf("\n");
	printf("<options>:\n");
	printf(" bitrate <value>\n");
	printf(" filter <id>:<mask>\n");
	printf(" loopback [on|off]\n");
	printf("\n");
}

static void print_config(struct can_dev_conf *conf) {
	printf("bitrate %lu\n", conf->bitrate);
	printf("filter %x:%x\n", conf->filter.can_id, conf->filter.can_mask);
	printf("loopback %s\n", conf->loopback ? "on" : "off");
}

int main(int argc, char const *argv[]) {
	struct can_dev_conf conf;
	struct can_dev *can;
	const char *opt;
	const char *arg;
	char *endptr;
	long val;
	int err;
	int i;

	if (argc < 2) {
		print_help();
		return 1;
	}

	can = can_dev_find(argv[1]);
	if (!can) {
		print_help();
		printf("No such device: %s\n", argv[1]);
		return 1;
	}

	if (argc == 2) {
		print_config(&can->conf);
		return 0;
	}

	memcpy(&conf, &can->conf, sizeof(struct can_dev_conf));

	for (i = 2; i < argc; i++) {
		if (!strcmp(argv[i], "restart")) {
			continue;
		}

		opt = argv[i++];
		arg = (i < argc) ? argv[i] : "?";
		err = 1;

		if (!strcmp(opt, "bitrate")) {
			val = (long)strtoul(arg, &endptr, 10);
			if ((val >= 0) && (endptr[0] == '\0')) {
				conf.bitrate = val;
				err = 0;
			}
		}
		else if (!strcmp(opt, "filter")) {
			val = (long)strtoul(arg, &endptr, 16);
			if ((val >= 0) && (endptr[0] == ':') && (endptr[1] != '\0')) {
				conf.filter.can_id = (uint32_t)val;
				val = (long)strtoul(endptr + 1, &endptr, 16);
				if ((val >= 0) && (endptr[0] == '\0')) {
					conf.filter.can_mask = (uint32_t)val;
					err = 0;
				}
			}
		}
		else if (!strcmp(opt, "loopback")) {
			if (!strcmp(arg, "on")) {
				conf.loopback = true;
				err = 0;
			}
			else if (!strcmp(arg, "off")) {
				conf.loopback = false;
				err = 0;
			}
		}
		else {
			print_help();
			printf("Invalid option %s\n", opt);
			return 1;
		}

		if (err) {
			print_help();
			printf("Invalid value for %s\n", opt);
			return 1;
		}
	}

	if (0 != char_dev_usage_count((struct char_dev *)can)) {
		printf("Device %s is busy\n", argv[1]);
		return 1;
	}

	memcpy(&can->conf, &conf, sizeof(struct can_dev_conf));
	can->configured = false;

	return 0;
}
