/**
 * @file
 * @brief
 *
 * @author	Denis Deryugin
 * @date    04.03.2014
 */

#include <embox/cmd.h>
#include <time.h>
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
	clock_t begin = clock(), end;
	initialize_hashtable();
	cyg_profiling = true;
	printf("Executing command\n");
	// exec cmd
	cyg_profiling = false;
	end = clock();
	printf("Program exited with code %d. Time: %0.3lfs. Profiling finished.\n",
			res, 1. * (end - begin) / CLOCKS_PER_SEC);
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
