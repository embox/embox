/**
 * @file
 *
 * @date April 17, 2024
 * @author Kamil Mardanshin
 */



#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("atan() tests");

bool is_close(double x, double y) {
    return fabs(x - y) <= 1e-9 * fmin(fabs(x), fabs(y));
}


TEST_CASE("Test for atan at simple points") {
    test_assert(is_close(atan(0.0), 0.0));
    test_assert(is_close(atan(1.0), M_PI_4));
    test_assert(is_close(atan(-1.0), -M_PI_4));
}

TEST_CASE("Test for tan(atan)") {
    double from = -1e2;
    double to = 1e2;
    double step = 1;

    for (double x = from; x < to; x += step) {
        test_assert(is_close(tan(atan(x)), x));
    }
}

TEST_CASE("Test for atan(tan)") {
    double from = -M_PI_2;
    double to = M_PI_2;
    double step = 1e-2;

    for (double x = from; x < to; x += step) {
        test_assert(is_close(atan(tan(x)), x));
    }
}

TEST_CASE("Test for atan corner cases") {
    test_assert(is_close(atan(INFINITY), M_PI_2));
    test_assert(is_close(atan(-INFINITY), -M_PI_2));
    test_assert(isnan(atan(NAN)));
}
