/**
 * @file
 *
 * @date 18.11.2015
 * @author: Anton Bondarev
 */
#include <unistd.h>

void swab(const void *bfrom, void *bto, ssize_t n) {
  const char *from = (const char *) bfrom;
  char *to = (char *) bto;

  n &= ~((ssize_t) 1);
  while (n > 1)
    {
      const char b0 = from[--n], b1 = from[--n];
      to[n] = b0;
      to[n + 1] = b1;
    }
}
