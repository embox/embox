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

/*
   #define TRACE_BLOCK_DEF(tb) \
	struct trace_block tb = { }

static void my_traced_function(int n) {
	static TRACE_BLOCK_DEF(my_trace);

	trace_block_enter(&my_trace);
	if (n > 0) {
		my_traced_function(n - 1);
	}
	trace_block_leave(&my_trace);
} */

