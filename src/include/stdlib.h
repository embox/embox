/**
 * @file
 *
 * @date 20.11.09
 * @author Nikolay Korotky
 */
#ifndef STDLIB_H_
#define STDLIB_H_

#include <types.h>

/**
 * convert digit character to integer
 * @param digit character for converting
 * @param base for converting
 * @return converted symbol
 * @return -1 if error
 */
extern int ch_to_digit(char ch, int base);

/**
 * Convert a string to a floating-point number.
 */
extern double atof(const char *nptr);

/**
 * Convert a string to an integer.
 */
extern int atoi(const char *nptr);

/**
 * Convert an integer to a string.
 */
extern void itoa(char *buf, int base, int d);

/**
 * Convert a string to a long integer.
 */
extern long int atol(const char *nptr);

/**
 * Converts the initial part of the string in @a nptr to a long integer value
 * according to the given @a base, which must be between 2 and 36 inclusive,
 * or be the special value 0.
 *
 * The remainder of the string is converted to a @code long int @endcode value
 * in the obvious manner, stopping at the first character which is not a valid
 * digit in the given @a base.
 *
 * @param nptr
 *   The string to parse. It may begin with an arbitrary amount of white space
 *   (determined by #isspace()) followed by a single optional '+' or '-' sign.
 *   The string is interpreted until the first invalid character is
 *   encountered.
 * @param endptr
 *   If @a endptr is not @c NULL, the address of the first invalid character is
 *   stored in @c *endptr.
 * @param base
 *   The base of interpreted number.
 *   If @a base is zero or 16, the string may include a @c 0x prefix, and the
 *   number will be read in base 16 (hex).
 *   Otherwise, a zero base is taken as 10 (decimal) unless the next character
 *   is @c '0', in which case it is taken as 8 (octal).
 * @return
 *   The parsed number bounded to @code [LONG_MIN..LONG_MAX] @endcode or zero
 *   if there were no valid characters representing a number in the given
 *   @a base.
 */
extern long int strtol(const char *nptr, char **endptr, int base);

/** Allocate and free dynamic memory */
extern void *calloc(size_t nmemb, size_t size);
extern void *malloc(size_t size);
extern void free(void *ptr);
extern void *realloc(void *ptr, size_t size);

#endif /* STDLIB_H_ */
