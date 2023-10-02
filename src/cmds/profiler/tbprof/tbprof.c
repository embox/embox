/**
 * @file
 * @brief
 *
 * @author	Denis Deryugin
 * @date    04.03.2014
 */

#include <framework/cmd/api.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <profiler/tracing/trace.h>

static int run_count = 1;

void initialize_hashtable() {
	trace_block_hashtable_init();
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
	int res = 0, run = 0;
	clock_t begin, end;
	set_profiling_mode(DISABLED);
	initialize_hashtable();

	printf("Initialization...\n");
	/*trace_block_hashtable_init();*/
	printf("Executing command\n");
	begin = clock();
	for (run = 0; run < run_count; run++) {
		printf ("TBPROF RUN #%d\n", run);
		set_profiling_mode(CYG_PROFILING);
		res = cmd_exec(cmd, argc, argv);
		set_profiling_mode(DISABLED);
	}
	end = clock();
	printf("Program exited with code %d. Time: %0.3lfs. Profiling finished.\n",
			res, 1. * (end - begin) / CLOCKS_PER_SEC);

	argc++;
	cmd = NULL;
	print_data_to_file(out);
}

int main(int argc, char *argv[]) {
	/* parse params */
	const struct cmd *c_cmd;
	int c_argc = argc, opt, argnum = 1;
	FILE *out = NULL;


	while (-1 != (opt = getopt(argc, argv, "chn:ed"))) {
		argnum++;
		switch (opt) {
			case 'c':
				/* execute command */
				c_cmd = cmd_lookup(argv[argnum]);
				run_cmd(c_cmd, c_argc - argnum, argv + argnum, out);
				return 0;
				break;
			case 'n':
				sscanf(optarg, "%d", &run_count);
				argnum++;
				break;
			case 'e':
				initialize_hashtable();
				set_profiling_mode(CYG_PROFILING);
				printf("cyg_profiling enabled.\n");
				return 0;
				break;
			case 'd':
				set_profiling_mode(DISABLED);
				printf("cyg_profiling disabled.\n");
				return 0;
				break;
			case 'h':
				printf("Example: tbprof [-n count] -c [cmd]\nUse man tbprof for more info.\n");
				return 0;
				break;
		}
	}
	printf("Wrong arguments. tbprof -h for usage.\n");
	return 0;
}
