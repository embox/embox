/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.12.24
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <plc/core.h>

static void print_help_msg(const char *cmd) {
	fprintf(stderr, "Usage: %s [CONFIG]\n", cmd);
}

int main(int argc, char **argv) {
	const struct plc_config *config;
	int err;

	if (argc != 2) {
		print_help_msg(argv[0]);
		return -1;
	}

	plc_config_foreach(config) {
		if (!strcmp(config->name, argv[1])) {
			err = plc_start(config);
			if (err == -EBUSY) {
				printf("PLC is already running\n");
				err = 0;
			}
			return err;
		}
	}

	print_help_msg(argv[0]);
	return -1;
}
