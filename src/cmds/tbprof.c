/**
 * @file
 * @brief
 *
 * @author	Denis Deryugin
 * @date    04.03.2014
 */

#include <embox/cmd.h>

#include <stdlib.h>
#include <stdio.h>
#include <profiler/tracing/trace.h>
EMBOX_CMD(tbprof_main);

void initialize_hashtable() {
}

void print_data_to_file(FILE *out) {
}

void run_cmd(struct cmd *cmd, int argc, char *argv[], FILE *out) {
	int res = 0;
	initialize_hashtable();
	cyg_profiling = true;
	printf("Executing command\n");
	// exec cmd
	cyg_profiling = false;
	printf("Program exited with code %d. Profiling finished.\n", res);
	argc++;
	cmd = NULL;
	print_data_to_file(out);
}

static int tbprof_main (int argc, char *argv[]) {
	/* parse params */
	struct cmd *c_cmd = NULL;
	int c_argc = 0;
	char *c_argv[0];
	FILE *out = NULL;

	run_cmd(c_cmd, c_argc, c_argv, out);

	return 0;
}
