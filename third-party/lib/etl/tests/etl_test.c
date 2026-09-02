#include <embox/test.h>
#include <etl/vector.h>

EMBOX_TEST_SUITE("ETL basic test");

TEST_CASE("Simple vector push and read") {
    etl::vector<int, 5> v;
    v.push_back(42);
    test_assert(v[0] == 42);
}
