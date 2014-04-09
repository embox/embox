/**
 * @file
 * @brief
 *
 * @author	Denis Deryugin
 * @date    04.03.2014
 */

#include <embox/cmd.h>
#include <framework/cmd/api.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <profiler/tracing/trace.h>
EMBOX_CMD(tbprof_main);

void initialize_hashtable() {
}

void print_data_to_file(FILE *out) {
}

void run_cmd(const struct cmd *cmd, int argc, char *argv[], FILE *out) {
	/* This function is for launching program that is beging tested with monitoring
	 * of all function calls and exits.
	 *	Args:	*cmd	-	pointer to the command that will be launched
	 *			argc	-	the number of arguments passed to pragram
	 *			argv	-	array of arguments
	 *			out		-	output file to write profiling info
	 */
	int res = 0;
	clock_t begin, end;
	initialize_hashtable();

	printf("Initialization...\n");
	trace_block_hashtable_init();
	printf("Executing command\n");
	begin = clock();
	cyg_profiling = true;
	res = cmd_exec(cmd, argc, argv);
	cyg_profiling = false;

	end = clock();
	printf("Program exited with code %d. Time: %0.3lfs. Profiling finished.\n",
			res, 1. * (end - begin) / CLOCKS_PER_SEC);
	//trace_block_hashtable_destroy();
	argc++;
	cmd = NULL;
	print_data_to_file(out);
}

static int tbprof_main (int argc, char *argv[]) {
	/* parse params */
	const struct cmd *c_cmd;
	int c_argc = argc;
	FILE *out = NULL;
	c_cmd = cmd_lookup(argv[1]);
	run_cmd(c_cmd, c_argc - 1, argv + 1, out);

	return 0;
}
