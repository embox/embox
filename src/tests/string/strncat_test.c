/**
 * @file strncat_test.c
 * @brief Test for the 'strncat' function.
 *
 * @date February, 2021
 * @author André Perez
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("string/strncat test");

TEST_CASE("strncat") {
  /* Destination string */
  char dest[17] = "Embox: ";

  /* Source string */
  const char src[] = "Real Time Operating System";

  /* The pointer returned by 'strncat' must be 'dest'. */
  test_assert_equal(strncat(dest, src, 9), dest);

  /* After concatenation, 'dest' (as string) must be equal to
     'Embox: Real Time'. */
  test_assert_str_equal(dest, "Embox: Real Time");
}
