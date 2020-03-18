/**
 * @file
 * @brief Test unit for string/memcmp
 *
 * @date March 18,2020
 * @author Sha Ahammed Roze
 */

 #include <embox/test.h>
 #include <strings.h>

 EMBOX_TEST_SUITE("Test suite for memcmp()");

 /**
 * Test arrays
 * buf1 > buf2
 * buf4 > buf3
 * buf5 = buf4
 */

char buf1[] = "SWgaOtP43df2";
char buf2[] = "SWGaOTP12df0";
char buf3[] = "Hello";
char buf4[] = "Welcome";
char buf5[] = "Welcome";

int n;

TEST_CASE("Test that buf1 > buf2")
{
  int n = memcmp(buf1,buf2,sizeof(buf1));
  test_assert(n > 0)
}

TEST_CASE("Test that buf3 < buf4")
{
  int n = memcmp(buf3,buf4,sizeof(buf3));
  test_assert(n < 0);
}

TEST_CASE("Test's that two arrays are equal")
{
  int n = memcmp(buf4,buf5,sizeof(buf4));
  test_assert(n = 0);
}
