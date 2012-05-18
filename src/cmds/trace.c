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
	printf("%s\n", "Usage: trace [-h] [-p] [-b] [-d <number>] [-a <number>]");
}

static void print_trace_point_stat(void) {
	struct __trace_point *tp;
	int number = 0;

	printf("%2s %25s %7s\n", "№ ", "Location function", "Count");

	array_nullterm_foreach(tp, __trace_points_array)
	{
		if (tp->active) {
			printf("%2d %25s %7d\n", number++, tp->location.func, tp->count);
		}
	}

	return;
}

static void print_trace_block_stat(void) {
	struct __trace_block *tb;
	int number = 0;

	printf("%2s %7s %10s\n", "№ ", "count", "time");

	array_nullterm_foreach(tb, __trace_blocks_array)
	{
		if (tb->active) {
			printf("%2d %7d %10d\n", number++, tb->begin->count, tb->time);
		}
	}

	return;
}

bool change_block_activity(int index, bool activity) {
	struct __trace_block *tb;
	int number = 0;

	array_nullterm_foreach(tb, __trace_blocks_array)
	{
		if (number == index) {
			tb->active = activity;
			return true;
		}
		number++;
	}

	return false;
}

static int exec(int argc, char **argv) {
	int opt;
	int index;

	if (argc <= 1) {
		print_usage();
		return -1;
	}

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "hpbd:a:"))) {
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
		case 'b':
			print_trace_block_stat();
			break;
		case 'd':
			if (sscanf(optarg, "%d", &index) != 1) {
				print_usage();
				printf("Invalid command line option\n");
			} else if (!change_block_activity(index, false)) {
				print_usage();
				printf("Invalid command line option\n");
			}
			break;
		case 'a':
			if (sscanf(optarg, "%d", &index) != 1) {
				print_usage();
				printf("Invalid command line option\n");
			} else if (!change_block_activity(index, true)) {
				print_usage();
				printf("Invalid command line option\n");
			}
			break;
		default:
			break;
		}
	}
	return 0;
}
