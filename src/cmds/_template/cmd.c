/**
 * @file
 * @brief TODO This file is derived from Embox cmd template.
 *
 * @date
 * @author TODO Your name here
 */

#include <embox/cmd.h>

EMBOX_CMD(exec);

/** One-line cmd usage. */
static void print_usage(void) {
	printf("Usage: __cmd_name ...");
}

/**
 * The cmd entry point.
 *
 * @param argc Argument count
 * @param argv Argument vector
 *
 * @return Exit code
 * @retval 0 On success
 * @retval nonzero Error code
 */
static int exec(int argc, char **argv) {
	print_usage();
	return 0;
}
