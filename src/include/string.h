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

#ifndef STRING_H_
#define STRING_H_

#include <stddef.h>

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
 * Maps given error code to its string representation.
 *
 * @param errno
 *   The error code.
 * @return
 *   C-style string, containing a descriptive error message
 */
extern char *strerror(int errno);

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

#endif /* STRING_H_ */
