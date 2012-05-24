/**
 * @file
 * @brief TODO --Alina.
 *
 * @date 23.05.2012
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
	printf(
			"%s\n",
			"Usage: trace_point [-h] [-s] [-i <number>] [-d <number>] [-a <number>]");
}

static void print_trace_point_stat_with_location(int i) {
	struct __trace_point *tp;
	int number = 0;

	printf("%2s %25s %7s %5s\n", "№ ", "Location function", "Count", "Act");

	array_nullterm_foreach(tp, __trace_points_array)
	{
		if (number++ == i) {
			printf("%2d %25s %7d %5s\n", i, tp->location.func, tp->count,
					tp->active ? "yes" : "no");
		}
	}

	return;
}

static void print_trace_point_stat(void) {
	struct __trace_point *tp;
	int number = 0;

	printf("%2s %7s\n", "№ ", "count");

	array_nullterm_foreach(tp, __trace_points_array)
	{
		if (tp->active) {
			printf("%2d %7d\n", number, tp->count);
		}
		number++;
	}

	return;
}

bool change_point_activity(int index, bool activity) {
	struct __trace_point *tp;
	int number = 0;

	array_nullterm_foreach(tp, __trace_points_array)
	{
		if (number == index) {
			tp->active = activity;
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

	while (-1 != (opt = getopt(argc, argv, "hsi:d:a:"))) {
		printf("\n");
		switch (opt) {
		case '?':
			printf("Invalid command line option\n");
			/* FALLTHROUGH */
		case 'h':
			print_usage();
			break;
		case 'i':
			if (sscanf(optarg, "%d", &index) != 1) {
				print_usage();
				printf("Invalid command line option\n");
			}
			print_trace_point_stat_with_location(index);
			break;
		case 's':
			print_trace_point_stat();
			break;
		case 'd':
			if (sscanf(optarg, "%d", &index) != 1) {
				print_usage();
				printf("Invalid command line option\n");
			} else if (!change_point_activity(index, false)) {
				print_usage();
				printf("Invalid command line option\n");
			}
			break;
		case 'a':
			if (sscanf(optarg, "%d", &index) != 1) {
				print_usage();
				printf("Invalid command line option\n");
			} else if (!change_point_activity(index, true)) {
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

