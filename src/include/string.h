/**
 * @file
 * @brief C standard library header.
 * @details Contains declarations of functions used for C-style strings
 * handling and operating with memory blocks.
 *
 * @date 02.12.08
 * @author Anton Bondarev
 *         - Initial implementation
 * @author Nikolay Korotky
 *         - Major rework
 * @author Eldar Abusalimov
 *         - Documenting and library code cleanup (TODO in progress. -- Eldar)
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
 *   Source null-terminated string.
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
 *   Source null-terminated string.
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
 * Append src on the end of dest.
 */
extern char *strcat(char *dest, const char *src);
extern char *strncat(char *dest, const char *src, size_t n);

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
 *   If @a str1 is greater than @a str2
 * @retval negative
 *   If @a str1 is less than @a str2
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
 *   If @a str1 is greater than @a str2
 * @retval negative
 *   If @a str1 is less than @a str2
 *
 * @see strcmp()
 */
extern int strncmp(const char *str1, const char *str2, size_t n);

/**
 * Finds the first occurrence of the character @a ch (converted to a char) in
 * the given null-terminated string.
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
 * function finds the first occurrence of the substring
 * needle in the string haystack. The terminating '\0'
 * characters are not compared.
 * @return pointer to the beginning of the substring,
 *         or NULL if the substring is not found.
 */
extern char *strstr(const char *haystack, const char *needle);

/**
 * Returns the string representation of an error number e.g. errno.
 *
 * @param errnum error code
 * @return C-style string, containing an error message
 */
extern char *strerror(int errnum);

/**
 * copy one massive of bytes to another
 * @param dst pointer to first byte in dst mass
 * @param src pointer to first byte in src mass
 * @return pointer to first byte in dst mass
 */
extern void *memcpy(void *dst, const void *src, size_t n);

/**
 * Copy n bytes of src to dest, guaranteeing
 * correct behavior for overlapping strings.
 * @return dst
 */
extern void *memmove(void *dst, const void *src, size_t n);

/**
 * set massive of bytes pointed value
 * @param p pointer to first byte in massive
 * @return pointer to first byte in massive
 */
extern void *memset(void *p, int c, size_t n);

/**
 * comparing 2 massive of bytes
 * @param dst first mass
 * @param src second mass
 * @return 0 if equal
 * @return positive if dst > src
 * @return negotive if  dst < src
 */
extern int memcmp(const void *dst, const void *src, size_t n);

#endif /* STRING_H_ */
