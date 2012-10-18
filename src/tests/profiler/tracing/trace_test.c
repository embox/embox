/**
 * @file
 *
 * @date 09.04.12
 * @author Kramar Alina
 */

#include <embox/test.h>
#include <profiler/tracing/trace.h>

EMBOX_TEST_SUITE("Trace point counting test");

TEST_CASE("trace point count should be equals count of loop's iterations") {
	static TRACE_POINT_DEF(tp, "test_loop");

	for (int i = 0; i < 10; ++i) {
		trace_point_set(&tp);
	}

	test_assert_equal(trace_point_get_value(&tp), 10);
}

TRACE_BLOCK_DEF(my_trace);

static void my_traced_function(int n) {

	trace_block_enter(&my_trace);
	if (n > 0) {
		my_traced_function(n - 1);
	}
	trace_block_leave(&my_trace);
}

TEST_CASE("Value of trace block end and trace block begin should be equals") {
	my_traced_function(-1);
	test_assert(0 == trace_block_diff(&my_trace));
}

TRACE_BLOCK_DEF(loop_time_block)

static void trace_loop_func(int iteration_count) {
	volatile int i;

	trace_block_enter(&loop_time_block);

	for (i = 0; i < iteration_count; ++i) {
		;
	}

	trace_block_leave(&loop_time_block);
}

TEST_CASE(""){
	int count_first = 0;
	int count_second = 0;

	trace_loop_func(100);
	count_first = trace_block_get_time(&loop_time_block);

	trace_loop_func(10000);
	count_second = trace_block_get_time(&loop_time_block);

	test_assert(count_first < count_second);
}
