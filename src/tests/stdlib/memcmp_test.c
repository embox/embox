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
 */

char buf1[] = "SWgaOtP43df2";
char buf2[] = "SWGaOTP12df0";

int n;

TEST_CASE("Test that buf1 > buf2")
{
  int n = memcmp(buf1,buf2,sizeof(buf1));
  test_assert()
}
