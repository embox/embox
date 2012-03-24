/**
 * @file
 * @brief TODO --Alina.
 *
 * @date 18.03.2012
 * @author Alina Kramar
 */

#include <stdio.h>
#include <embox/cmd.h>
#include <util/array.h>
#include <util/location.h>
#include <profiler/tracing/trace.h>

EMBOX_CMD(exec);

extern struct __tp * const __tracepoints_array[];

//static void print_usage(void) {
//	printf("%s\n", "Usage: tpprint");
//}

static int exec(int argc, char **argv) {

	struct __tp *tp;

	array_nullterm_foreach(tp, __tracepoints_array) {
		printf("%10s - %d\n", tp->name, tp->count);
	}

	return 0;
}
