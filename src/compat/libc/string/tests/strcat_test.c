/**
 * @file strcat_test.c
 * @brief Test for the 'strcat' function.
 *
 * @date February, 2021
 * @author André Perez
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("string/strcat test");

TEST_CASE("strcat") {
  /* Destination string */
  char dest[34] = "Embox: ";

  /* Source string */
  const char src[] = "Real Time Operating System";

  /* The pointer returned by 'strcat' must be 'dest'. */
  test_assert_equal(strcat(dest, src), dest);

  /* After concatenation, 'dest' (as string) must be equal to
     'Embox: Real Time Operating System'. */
  test_assert_str_equal(dest, "Embox: Real Time Operating System");
}
