/**
 * @file
 * @brief TODO --Alina.
 *
 * @date 18.03.2012
 * @author Alina Kramar
 */

#include <stdio.h>

#include <getopt.h>
#include <embox/cmd.h>
#include <util/array.h>
#include <profiler/tracing/trace.h>

EMBOX_CMD(exec);

extern struct __trace_point * const __trace_points_array[];

static void print_usage(void) {
	printf("%s\n", "Usage: trace [-h] [-p] [-b]");
}

static void print_trace_point_stat(void) {
	struct __trace_point *tp;

	array_nullterm_foreach(tp, __trace_points_array)
	{
		printf("%15s %25s %7d\n", tp->name, tp->location.func, tp->count);
	}

	return;
}

static void print_trace_block_stat(void) {
	struct __trace_block *tb;

	printf("%25s %7s %7s %10s\n", "begin location", "begin", "end", "time");

	array_nullterm_foreach(tb, __trace_blocks_array)
	{
		printf("%25s %7d %7d %10d\n", tb->begin->location.func,
				tb->begin->count, tb->end->count, tb->time);
	}

	return;
}

static int exec(int argc, char **argv) {
	int opt;

	if (argc <= 1) {
		print_usage();
		return -1;
	}

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "hpb"))) {
		printf("\n");
		switch (opt) {
		case '?':
			printf("Invalid command line option\n");
			/* FALLTHROUGH */
		case 'h':
			print_usage();
			break;
		case 'p':
			print_trace_point_stat();
			break;
		case 'b': {
			print_trace_block_stat();
			break;
		}
		default:
			break;
		}
	}

	return 0;
}
