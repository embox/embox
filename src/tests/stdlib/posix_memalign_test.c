/**
 * @file
 * @brief
 *
 * @date 04.07.2020
 * @author KOUAM Ledoux
 */

#include <embox/test.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

const int pm_test = 0xFFF ;

EMBOX_TEST_SUITE("POSIX MEMALIGN test");

TEST_CASE("call posix_memalign with bad values") {
    void *mem = NULL;
    test_assert_equal(posix_memalign(&mem, 12, 4), EINVAL);
    free(mem);
}

TEST_CASE("call posix_memalign with bad values") {
    void *mem = NULL;
    test_assert_equal(posix_memalign(&mem, 0, 0), EINVAL);
    free(mem);
}

TEST_CASE("Call posix_memalign with bad values") {
    void *mem = NULL;
    test_assert_equal(posix_memalign(&mem, 32, 0), EINVAL);
    free(mem);
}

TEST_CASE("Call the function with a large number") {
    void *mem = NULL;
    test_assert_equal(posix_memalign(&mem, 32, pm_test), 0);
    free(mem);
}
TEST_CASE("call the function with good parameter") {
    void *mem = NULL ;
    test_assert_equal(posix_memalign(&mem, 64, 0xF00), 0);
    free(mem);
}
TEST_CASE("call a multiple allocation with little size") {
    void *mem[pm_test];
    int i = 0;
    for(;i < pm_test;i++) {
        posix_memalign(mem[i], 64, 0xF00);
    }
    for(i = 0;i < pm_test;i++) {
        test_assert_not_equal(mem[i], NULL);
    }
    for(i = 0;i < pm_test;i++) {
        free(mem[i]);
    }
}
TEST_CASE("Call a multiple allocation with long size") {
    void *mem[pm_test];
    int i = 0;
    for(;i < pm_test;i++) {
        posix_memalign(mem[i], 64, 0xF00);
    }
    for(i = 0;i < pm_test;i++) {
        test_assert_not_equal(mem[i], NULL);
    }
    for(i = 0;i < pm_test;i++) {
        free(mem[i]);
    }
}