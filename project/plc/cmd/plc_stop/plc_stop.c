/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.12.24
 */

#include <stdio.h>

#include <plc/core.h>

static void print_help_msg(const char *cmd) {
	fprintf(stderr, "Usage: %s\n", cmd);
}

int main(int argc, char **argv) {
	if (argc != 1) {
		print_help_msg(argv[0]);
		return -1;
	}

	return plc_stop();
}
