/**
 * @file
 * @brief ISO C99 Standard: 7.21 String handling.
 * @details Contains declarations of functions used for C-style
 *          strings handling and operating with memory blocks.
 *
 * @date 02.12.08
 * @author Anton Bondarev
 *         - Initial implementation
 * @author Nikolay Korotky
 *         - Major rework
 * @author Eldar Abusalimov
 *         - Documenting and library code cleanup
 */

#ifndef COMPAT_LIBC_STRING_H_
#define COMPAT_LIBC_STRING_H_

#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

/**
 * Calculates the length of the specified null-terminated string.
 *
 * @param str
 *   C-style null-terminated string.
 * @return
 *   String length (the offset of the terminating null character within the
 *   array).
 */
extern size_t strlen(const char *str);

/**
 * Copies a null-terminated string from one location to another.
 *
 * @param dst
 *   Destination buffer.
 * @param src
 *   The string being copied.
 * @return
 *   Pointer to the destination buffer.
 *
 * @see memcpy()
 */
extern char *strcpy(char *dst, const char *src);

/**
 * Copies up to @a n bytes of a string from one location to another adding
 * zeros to the end of the copy if the source string length is less than @a n.
 *
 * @param dst
 *   Destination buffer of length @a n.
 * @param src
 *   The string being copied.
 * @param n
 *   The number of bytes to fill in the destination buffer.
 * @return
 *   Pointer to the destination buffer.
 *
 * @note
 *   Despite the name this function is not a bounded version of #strcpy(),
 *   it does not guarantee that the result is a null-terminated string.
 */
extern char *strncpy(char *dst, const char *src, size_t n);

/**
 * @brief Copy string from src to dest 
 *
 * @param dest
 * @param src
 *
 * @note  Arrays should not overlap
 * @note  This functions differs from strcpy() in return value
 *
 * @return Pointer to terminating NULL of the string dest
 */
extern char *stpcpy(char *dest, const char *src);

/**
 * Appends a null-terminated string to the end of another.
 *
 * @param dst
 *   Destination null-terminated string to append the @a src to.
 * @param src
 *   The string being appended.
 * @return
 *   Pointer to the destination string.
 */
extern char *strcat(char *dst, const char *src);

/**
 * Appends no more than @a n  bytes of a null-terminated string to the end of
 * another.
 * This function is a bounded version of #strcat().
 *
 * @param dst
 *   Destination null-terminated string to append the @a src to.
 * @param src
 *   The string being appended.
 * @param n
 *   Maximum number of symbols to copy.
 * @return
 *   Pointer to the destination string.
 */
extern char *strncat(char *dst, const char *src, size_t n);

/**
 * Compares two strings lexicographically.
 *
 * @param str1
 *   The first string.
 * @param str2
 *   The second string.
 * @return
 *   Comparison result according to the lexicographical order.
 * @retval 0
 *   If the strings are equal.
 * @retval positive
 *   If @a str1 is greater than @a str2.
 * @retval negative
 *   If @a str1 is less than @a str2.
 *
 * @see strncmp()
 */
extern int strcmp(const char *str1, const char *str2);

/**
 * Compares up to @c n bytes of two strings lexicographically.
 * This function is a bounded version of #strcmp().
 *
 * @param str1
 *   The first string.
 * @param str2
 *   The second string.
 * @param n
 *   Maximum number of symbols to compare.
 * @return
 *   Comparison result according to the lexicographical order.
 * @retval 0
 *   If the strings are equal.
 * @retval positive
 *   If @a str1 is greater than @a str2.
 * @retval negative
 *   If @a str1 is less than @a str2.
 *
 * @see strcmp()
 */
extern int strncmp(const char *str1, const char *str2, size_t n);

/** 
 * Compare two strings using C locale.
 * @param s1
 *   The first string.
 * @param s2
 *   The second string.
 * @return
 *   Comparison result using LC_COLLATE=C.
 * @retval 0
 *   If the strings are equal.
 * @retval positive
 *   If @a s1 is greater than @a s2.
 * @retval negative
 *   If @a s1 is less than @a s2.
 * Throw assert if character is not ASCII.
 */
extern int strcoll(const char *s1, const char *s2);

/**
 * Copy no more than n symbols from src to dest in such way that
 * strcmp with modified strings works in the same way as strcoll.
 * @param dest
 *   Destination string.
 * @param s2
 *   Source string.
 * @return
 *   Number of bytes required to store the transformed string in
 *   dest excluding the terminating null byte ('\0').
 * @retval 0
 * Throw assert if character is not ASCII.
 */
extern size_t strxfrm(char *dest, const char *src, size_t n);

/**
 * Finds the first occurrence of the character @a ch (converted to a @c char)
 * in the given null-terminated string.
 *
 * @param str
 *   The null-terminated string to search for @a ch occurrence.
 * @param ch
 *   The character to search.
 * @return
 *   Pointer to the located character in the specified string (if any).
 * @retval NULL
 *   If no match was found.
 */
extern char *strchr(const char *str, int ch);

/**
 * Finds the first occurrence of the character @a ch (converted to a @c char)
 * in the given null-terminated string.
 *
 * @param str
 *   The null-terminated string to search for @a ch occurrence.
 * @param ch
 *   The character to search.
 * @return
 *   Pointer to the located character in the specified string (if any).
 * @retval pointer to null-terminator '\0'
 *   If no match was found.
 */
extern char *strchrnul(const char *str, int ch);

/**
 * Finds the last occurrence of the character @a ch in the given string.
 * This function is a reversed version of #strchr().
 *
 * @param str
 *   The null-terminated string to search for @a ch occurrence.
 * @param ch
 *   The character to search.
 * @return
 *   Pointer to the located character in the specified string (if any).
 * @retval NULL
 *   If no match was found.
 *
 * @see strchr()
 */
extern char *strrchr(const char *str, int ch);

/**
 * Finds the first occurrence of the substring @a needle in
 * the given null-terminated string @a haystack.
 *
 * @param haystack
 *   The null-terminated string to search for @a needle occurrence.
 * @param needle
 *   The substring to search.
 * @return
 *   Pointer into the @a haystack that is the first character of the located
 *   substring (if any). Returns @a haystack if @a needle is empty.
 * @retval NULL
 *   If no match was found.
 */
extern char *strstr(const char *haystack, const char *needle);

/**
 * Finds the first occurrence of the substring @a needle in
 * the given null-terminated string @a haystack, but ignores case of both
 * arguments.
 *
 * @param haystack
 *   The null-terminated string to search for @a needle occurrence.
 * @param needle
 *   The substring to search.
 * @return
 *   Pointer into the @a haystack that is the first character of the located
 *   substring (if any). Returns @a haystack if @a needle is empty.
 * @retval NULL
 *   If no match was found.
 */
extern char *strcasestr(const char *haystack, const char *needle);

/**
 * Get error message string.
 *
 * @param errnum
 *   The error code.
 * @return
 *   C-style string, containing a descriptive error message
 */
extern char *strerror(int errnum);

/**
 * Get error message string.
 *
 * @param errno
 *   The error code.
 * @param strerrbuf
 *   Pointer to error message buffer.
 * @param buflen
 *   Length of error message buffer.
 * @retval 0
 *   If successful.
 * @retval -1 (ERANGE)
 *   Insufficient storage was supplied to contain error message string.
 */
extern int strerror_r(int errnum, char *strerrbuf, size_t buflen);

/**
 * Copies no more than @a n bytes from memory area @a src
 * to memory area @a dest,
 * stopping when the character @a c is found.
 * 
 * @param dest
 *  Destination buffer
 * @param src
 *  The object in memory being copied
 * @param n
 *  The number of bytes to copy
 * @param c
 *  Character to stop copying when found.
 * @return
 *  Pointer to the destination buffer.
 * 
 * @note
 *  If the memory areas overlap, the results are undefined.
 * @see memcpy()
 *  It does the same thing, but it doesn't stop at c.
 */
extern void *memccpy(void *dest, const void *src, int c, size_t n);

/**
 * Copies @a n bytes from @a src to @a dst which must be two non-overlapping
 * buffers.
 *
 * @param dst
 *   Destination buffer.
 * @param src
 *   The object in memory being copied.
 * @param n
 *   The number of bytes to copy.
 * @return
 *   Pointer to the destination buffer.
 *
 * @note
 *   The behavior is undefined if the two arrays overlap.
 * @see memmove()
 *   Use it instead if overlapping of @a src and @a dst is possible.
 */
extern void *memcpy(void *dst, const void *src, size_t n);

/**
 * Copies @a n bytes from @a src to @a dst handling possible memory
 * overlapping.
 *
 * @param dst
 *   Destination buffer.
 * @param src
 *   The object in memory being copied.
 * @param n
 *   The number of bytes to copy.
 * @return
 *   Pointer to the destination buffer.
 */
extern void *memmove(void *dst, const void *src, size_t n);

/**
 * Fills @a n bytes of memory starting at given @a ptr with the value of @a c
 * converted to @code unsigned char @endcode.
 *
 * @param ptr
 *   Pointer to the memory being filled.
 * @param c
 *   The pattern
 * @param n
 *   The number of bytes to fill.
 * @return
 *   Pointer to the buffer.
 */
extern void *memset(void *ptr, int c, size_t n);

/**
 * Compares the @c n bytes of memory beginning at @a ptr1 against the @c n
 * bytes of memory beginning at @a ptr2.
 *
 * @param ptr1
 *   Pointer to the first memory block.
 * @param ptr2
 *   Pointer to the second memory block.
 * @param n
 *   Number of bytes to compare.
 * @return
 *   The difference between the first differing pair of bytes (if any).
 * @retval 0
 *   If the memory blocks are equal.
 * @retval positive
 *   If the first memory block is greater than the second.
 * @retval negative
 *   If the first memory block is less than the second.
 */
extern int memcmp(const void *ptr1, const void *ptr2, size_t n);

/**
 * Scans the first n bytes of the memory area pointed to by
 * s for the character c.
 *
 * @param s
 *   Pointer to the memory
 * @param c
 *   The character to search
 * @param n
 *   Number of bytes to scan
 * @return
 *   Pointer to the matching byte
 * @retval NULL the character doesn't occur in the given memory area
 */
extern void *memchr(const void *s, int c, size_t n);

/**
 * Scans the memory area pointed to by s in backward for the character c.
 *
 * @param s
 *   Pointer to the memory
 * @param c
 *   The character to search
 * @param n
 *   Number of bytes to scan
 * @return
 *   Pointer to the matching byte
 * @retval NULL the character doesn't occur in the given memory area
 */
extern void *memrchr(const void *s, int c, size_t n);

/**
 * map upper-case characters in a string to lower-case.
 *
 * @param str
 *   C-style null-terminated string.
 * @return
 *   C-style null-terminated string.
 */
extern char *strlwr(char *str);

/**
 * map lower-case characters in a string to upper-case.
 *
 * @param str
 *   C-style null-terminated string.
 * @return
 *   C-style null-terminated string.
 */
extern char *strupr(char *str);

/**
 * Calculate the length of the initial segment of s which
 * consists entirely of characters in accept
 *
 * @param s
 *   C-style null-terminated string.
 * @param accept
 * @return
 *   Number of characters in the initial segment
 *   of s which consist only of characters from accept
 */
extern size_t strspn(const char *s, const char *accept);

/**
 * Calculate the length of the initial segment of s which
 * consists entirely of characters not in reject
 *
 * @param s
 *   C-style null-terminated string.
 * @param reject
 * @return
 *   Number of characters in the initial segment
 *   of s which are not in the string reject
 */
extern size_t strcspn(const char *s, const char *reject);

/**
 * Locate the first occurrence in the string s of any of
 * the characters in the string accept
 *
 * @param s
 *   C-style null-terminated string.
 * @param accept
 * @return
 *   Pointer to the character in s that matches one of the
 *   characters in accept
 * @retval NULL no such character is found
 */
extern char *strpbrk(const char *s, const char *accept);
/**
 * Returns a pointer to a new string which is a duplicate of the string s.
 * Memory for the new string is obtained with malloc, and can be freed with free.
 *
 * @param s
 *  C-style null-terminated string.
 * @return
 *   Pointer to a new string which is a duplicate of the string s
 */
extern char *strdup(const char *s);

/**
 * The function strtok_r() considers the null-terminated string s as a sequence
 * of zero or more text tokens separated by spans of one or more characters
 * from the separator string sep. The argument lasts points to a user-provided
 * pointer which points to stored information necessary for strtok_r() to
 * continue scanning the same string.
 * In the first call to strtok_r(), s points to a null-terminated string, sep
 * to a null-terminated string of separator characters and the value pointed to
 * by lasts is ignored. The function strtok_r() returns a pointer to the first
 * character of the first token, writes a null character into s immediately
 * following the returned token, and updates the pointer to which lasts points.
 * In subsequent calls, s is a NULL pointer and lasts will be unchanged from the previous call so that subsequent calls will move through the string s, returning successive tokens until no tokens remain. The separator string sep may be different from call to call. When no token remains in s, a NULL pointer is returned.
 *
 * @param str
 * @param delim
 * @param saveptr
 * @return The function strtok_r() returns a pointer to the token found, or a
 * NULL pointer when no token is found.
 */
extern char *strtok_r(char *str, const char *delim, char **saveptr);
/**
 * A sequence of calls to strtok() breaks the string pointed to by s1 into a
 * sequence of tokens, each of which is delimited by a byte from the string
 * pointed to by s2. The first call in the sequence has s1 as its first
 * argument, and is followed by calls with a null pointer as their first
 * argument. The separator string pointed to by s2 may be different from call
 * to call.
 * The first call in the sequence searches the string pointed to by s1 for the
 * first byte that is not contained in the current separator string pointed to
 * by s2. If no such byte is found, then there are no tokens in the string
 * pointed to by s1 and strtok() returns a null pointer. If such a byte is
 * found, it is the start of the first token.
 * The strtok() function then searches from there for a byte that is contained
 * in the current separator string. If no such byte is found, the current token
 * extends to the end of the string pointed to by s1, and subsequent searches
 * for a token will return a null pointer. If such a byte is found, it is
 * overwritten by a null byte, which terminates the current token. The strtok()
 * function saves a pointer to the following byte, from which the next search
 * for a token will start.
 * Each subsequent call, with a null pointer as the value of the first argument,
 * starts searching from the saved pointer and behaves as described above.
 *
 * @param str
 * @param delim
 * @return Upon successful completion, strtok() returns a pointer to the first
 * byte of a token. Otherwise, if there is no token, strtok() returns a null
 * pointer.
 */
extern char *strtok(char *str, const char *delim);

/**
 * The strnlen() function shall compute the smaller of the number of bytes in
 * the array to which s points, not including the terminating NUL character,
 * or the value of the maxlen argument. The strnlen() function shall never
 * examine more than maxlen bytes of the array pointed to by s.
 *
 * @param s
 * @param maxlen
 *
 * @return The strnlen() function shall return an integer containing the smaller
 *  of either the length of the string pointed to by s or maxlen.
 */
extern size_t strnlen(const char *s, size_t maxlen);
/**
 * The strndup() function shall be equivalent to the strdup() function,
 * duplicating the provided s in a new block of memory allocated as if by using
 * malloc(), with the exception being that strndup() copies at most size plus
 * one bytes into the newly allocated memory, terminating the new string with a
 * NUL character. If the length of s is larger than size, only size bytes shall
 * be duplicated. If size is larger than the length of s, all bytes in s shall
 * be copied into the new memory buffer, including the terminating NUL
 * character. The newly created string shall always be properly terminated.
 *
 * @param s
 * @param size
 * @return Upon successful completion, the strndup() function shall return a
 * pointer to the newly allocated memory containing the duplicated string.
 * Otherwise, it shall return a null pointer and set errno to indicate the error.
 */
extern char *strndup(const char *s, size_t size);

#include <string_bsd.h>

__END_DECLS

#endif /* COMPAT_LIBC_STRING_H_ */
