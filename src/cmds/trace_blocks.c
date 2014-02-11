/**
 * @file
 * @brief
 *
 * @date 18.03.2012
 * @author Alina Kramar
 * @author Denis Deryugin
 */

#include <stdio.h>

#include <errno.h>
#include <unistd.h>
#include <embox/cmd.h>
#include <util/array.h>
#include <profiler/tracing/trace.h>

EMBOX_CMD(exec);

ARRAY_SPREAD_DECLARE(struct __trace_block *, __trace_blocks_array);

static void print_usage(void) {
	printf("Usage: trace [-h] [-s] [-e] [-i <number>] [-d <number>] [-a <number>]\n");
}

static void print_instrument_trace_block_stat(void) {
	struct __trace_block *tb = auto_profile_tb_first();
	printf("Automatic trace points:\n");

	printf("%40s %10s %20s %10s\n", "Name", "Count", "Ticks", "Time");
	if (tb) do {
		printf("%40s %10lld %20llu %10Lfs\n", tb->name,
			tb->count, tb->time,
			(tb->tc->cs) ? (long double) 1.0 * tb->time / 1000000000 : 0);

		tb = auto_profile_tb_next(tb);
	} while (tb);
}

static void print_trace_block_stat(void) {
	struct __trace_block *tb;
	int number = 0;

	printf("%2s %15s %12s %20s %11s\n", "#", "Name", "Count", "Ticks", "Time");

	array_spread_nullterm_foreach(tb, __trace_blocks_array)
	{
		if (tb->active) {
			printf("%2d %15s %12lld %20llu %10Lfs\n", number, tb->name,
				tb->count, tb->time,
				(tb->tc->cs) ? (long double) 1.0 * tb->time / 1000000000 : 0);
				/* Converting from nanoseconds to seconds */
		}
		number++;
	}

	return;
}

static void print_entered_blocks(void) {
	struct __trace_block *tb;
	int number = 0;

	printf("Currently entered blocks:\n\n");

	array_spread_nullterm_foreach(tb, __trace_blocks_array)
	{
		if (tb->is_entered) {
			printf("Trace block:%25s\nLocation:%28s, %40s:%d\n\n" , tb->name, tb->location.func,
				tb->location.at.file, tb->location.at.line);
		}
		number++;
	}

	return;
}

static void print_trace_block_stat_personal(int i) {
	struct __trace_block *tb;
	int number = 0;

	printf("%2s %7s %10s %5s\n", "№ ", "count", "time", "Act");

	array_spread_nullterm_foreach(tb, __trace_blocks_array)
	{
		if (number++ == i) {
			printf("%2d %7d %10llu %5s\n", i, tb->begin->count, tb->time, tb->active ? "yes" : "no");
			break;
		}
	}

	return;
}

bool change_block_activity(int index, bool activity) {
	struct __trace_block *tb;
	int number = 0;

	array_spread_nullterm_foreach(tb, __trace_blocks_array)
	{
		if (number++ == index) {
			tb->active = activity;
			return true;
		}
	}

	return false;
}

static int exec(int argc, char **argv) {
	int opt;
	int index;
	TRACE_BLOCK_DEF(this_is_block);

	trace_block_enter(&this_is_block);
	if (argc <= 1) {
		print_usage();
		return -EINVAL;
	}

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "ehsi:d:a:n"))) {
		printf("\n");
		switch (opt) {
		case '?':
			printf("Invalid command line option\n");
			/* FALLTHROUGH */
		case 'h':
			print_usage();
			break;
		case 's':
			print_trace_block_stat();
			break;
		case 'i':
			if (sscanf(optarg, "%d", &index) != 1) {
				print_usage();
				printf("Invalid command line option\n");
			}
			print_trace_block_stat_personal(index);
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
		case 'e':
			print_entered_blocks();
			break;
		case 'n':
			print_instrument_trace_block_stat();
			break;
		default:
			break;
		}
	}

	trace_block_leave(&this_is_block);
	return 0;
}
