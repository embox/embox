/**
 * @file
 * @brief
 *
 * @date 26.02.13
 * @author Ilia Vaprol
 */

#ifndef STRINGS_H_
#define STRINGS_H_


#include <defines/size_t.h>

#include <sys/cdefs.h>
__BEGIN_DECLS

#if 0
/* POSIX.1-2008 removes the specification of bcmp()
 * memcmp() is preferred over this function.*/
int    bcmp(const void *, const void *, size_t);
/*
 * For portability to implementations conforming to earlier versions of this
 * specification, memmove() is preferred over this function.
 */
void   bcopy(const void *, void *, size_t);
/* memset() is preferred over this function. */
void   bzero(void *, size_t);
/* strchr() is preferred over this function. */
char   *index(const char *, int);
/* strrchr() is preferred over this function. */
char   *rindex(const char *, int);
#endif

/**
 * The  ffs()  function  returns  the position of the first (least significant)
 * bit set in the word i.  The least significant bit is position 1 and the most
 * significant position is, for example, 32 or 64.
 */
extern int ffs(int i);

/**
 * The strcasecmp() function compares, while ignoring differences in case, the
 * string pointed to by s1 to the string pointed to by s2.
 *
 * @param s1
 * @param s2
 * @return Upon completion, strcasecmp() returns an integer greater than, equal
 * to or less than 0, if the string pointed to by s1 is, ignoring case, greater
 * than, equal to or less than the string pointed to by s2 respectively.
 */
extern int strcasecmp(const char *s1, const char *s2);
/**
 * The strncasecmp() function compares, while ignoring differences in case, not
 * more than n bytes from the string pointed to by s1 to the string pointed to
 * by s2.
 *
 * @param s1
 * @param s2
 * @param n
 * @return Upon successful completion, strncasecmp() returns an integer greater
 * than, equal to or less than 0, if the possibly null-terminated array pointed
 * to by s1 is, ignoring case, greater than, equal to or less than the possibly
 * null-terminated array pointed to by s2 respectively.
 */
extern int strncasecmp(const char *s1, const char *s2, size_t n);

__END_DECLS

#endif /* STRINGS_H_ */
