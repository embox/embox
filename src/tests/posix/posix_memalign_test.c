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

const uint64_t _const_size_  0XFFF ;

EMBOX_TEST_SUITE("POSIX MEMALIGN test");

TEST_CASE("call posix_memalign with bad values") {
    int **mem;
    test_assert_equal(posix_memalign(*mem, 12, 4), EINVAL);
    free(*mem);
}

TEST_CASE("Call posix_memalign with bad values") {
    int **mem;
    test_assert_equal(posix_memalign(*mem, 32, 0), EINVAL);
    free(*mem);
}

TEST_CASE("Call the function with a large number") {
    int **mem;
    test_assert_equal(posix_memalign(*mem, 32, _const_size_ ), 0);
    free(*mem);
}
TEST_CASE("call the function with good parameter") {

    int **mem ;
    test_assert_equal(posix_memalign(*mem, 64, _const_size_ ), 0);
    free(*mem);
}
TEST_CASE("call a multiple allocation with little size") {
    int *mem[_const_size_ ] = {NULL};
    int i = 0;
    for(i;i < _const_size_ ; i++) {
        posix_memalign(mem[i], 64, _const_size_ );
    }
    for(i = 0;i < _const_size_ ; i++) {
        test_assert_not_equal(mem[i], NULL);
    }
    for(i = 0 ;i < _const_size_ ; i++) {
        free(mem[i]);
    }
}
TEST_CASE("Call a multiple allocation with long size") {
    int *mem[_const_size_ ] = {NULL};
    int i = 0;
    for(i; i < _const_size_ ; i++) {
        posix_memalign(mem[i], 64, _const_size_ );
    }
    for(i = 0; i < _const_size_ ; i++) {
        test_assert_not_equal(mem[i], NULL);
    }
    for(i = 0; i < _const_size_ ; i++) {
        free(mem[i]);
    }
}