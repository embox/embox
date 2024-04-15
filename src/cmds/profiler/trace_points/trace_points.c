/**
 * @file
 * @brief TODO --Alina.
 *
 * @date 23.05.2012
 * @author Alina Kramar
 * @author Denis Deryugin
 */

#include <stdio.h>
#include <errno.h>

#include <unistd.h>
#include <lib/libds/array.h>
#include <profiler/tracing/trace.h>

ARRAY_SPREAD_DECLARE(struct __trace_point *, __trace_points_array);

static void print_usage(void) {
	printf(	"Usage: trace_point [ARGS]\n%-60s\n%-60s\n%-60s\n%-60s\n%-60s\n",
			"\t[-h] print usage\n",
			"\t[-s] all points: (#, NAME, LOCATION, COUNT, ACT)\n",
			"\t[-i <number>] one point: (#, NAME, LOCATION, COUNT, ACT)\n",
			"\t[-d <number>] change point activity to FALSE\n",
			"\t[-a <number>] change point activity to TRUE\n");
}

static void print_trace_point_stat_with_location(int i) {
	struct __trace_point *tp;
	int number = 0;

	printf("%2s %15s %25s %7s %5s\n",
			"#", "Name", "Location function", "Count", "Act");

	array_spread_nullterm_foreach(tp, __trace_points_array)
	{
		if (number++ == i) {
			printf("%2d %15s %25s %7d %5s\n", i, tp->name,
					tp->location.func, tp->count, tp->active ? "yes" : "no");
			break;
		}
	}

	return;
}

static void print_trace_point_stat(void) {
	struct __trace_point *tp;
	int number = 0;

	printf("%2s %15s %25s %7s %5s\n",
			"#", "Name", "Location function", "Count", "Act");


	array_spread_nullterm_foreach(tp, __trace_points_array)
	{
		printf("%2d %15s %25s %7d %5s\n", number, tp->name,
					tp->location.func, tp->count, tp->active ? "yes" : "no");

		/*if (tp->active) {
			printf("%2d %7d\n", number, tp->count);
		}*/
		number++;
	}

	return;
}

bool change_point_activity(int index, bool activity) {
	struct __trace_point *tp;
	int number = 0;

	array_spread_nullterm_foreach(tp, __trace_points_array)
	{
		if (number++ == index) {
			tp->active = activity;
			printf("Success! Trace point #%d now is %sactive.\n",
					index, (activity)?"":"not ");
			return true;
		}
	}
	printf("Failed to change state: trace point #%d not found!\n",
			index);
	return false;
}

int main(int argc, char **argv) {
	int opt;
	int index;

	if (argc <= 1) {
		print_usage();
		return 0;
	}


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
