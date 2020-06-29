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

EMBOX_TEST_SUITE("pvalloc test suite");

TEST_CASE("CALL WITH BAD VALUE") {
    test_assert_equal(pvalloc(0), NULL);
}
TEST_CASE("TEST WITH LARGE SIZE") {
    test_assert_not_equal(pvalloc(0xF00), NULL) ;
}
TEST_CASE("TEST WITH MULTIPLE ALLOCATION") {
    int *mem[0xFFF]={NULL} ;
    uint32_t i=0;
    for (i=0;i<0xFFF;i++)mem[i] = (int*)pvalloc(0xF00);
    for (i=0;i<0xFFF;i++)test_assert_not_equal(mem[i], NULL);
}
TEST_CASE("TEST WITH MULTIPLE ALLOCATION WITH LONG SIZE") {
    int *mem[0xFFF]={NULL} ;
    uint32_t i=0;
    for (i=0;i<0xFFF;i++)mem[i] = (int*)pvalloc(i*0xF00);
    for (i=0;i<0xFFF;i++)test_assert_not_equal(mem[i], NULL);
    for (i=0;i<0xFFF;i++)free(mem[i]);
}