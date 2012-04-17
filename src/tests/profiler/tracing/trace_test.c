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
	test_assert(0 == trace_block_dif(&my_trace));
}

TEST_CASE("If parameter is positive trace_block_dif should return positive number") {
	my_traced_function(100);
	test_assert(0 == trace_block_dif(&my_trace));
}
