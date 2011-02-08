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
 * Calculates the length of the specified string.
 *
 * @param str target C-style string
 * @return string length
 */
extern int strlen(const char *str);

/**
 * Copies a string from one location to another.
 *
 * @param dst destination string
 * @param src source string
 * @return pointer to destination string
 */
extern char *strcpy(char *dst, const char *src);

/**
 * Copies up to @c n bytes of a string from one location to another adding
 * zeros to the end of the copy is the source string length is less than @c n.
 * Despite its name it is not a bounded version of #strcpy(), it does not
 * guarantee that the result is a null-terminated string.
 *
 * @param dst destination string
 * @param src source string
 * @param n the size of block to fill in the destination buffer
 * @return pointer to destination string
 */
extern char *strncpy(char *dst, const char *src, size_t n);

/**
 * Compares two strings lexicographically.
 *
 * @param str1 first string
 * @param str2 second string
 * @return comparison result according to the lexicographical order
 * @retval 0 if the strings are equal
 * @return positive if @c str1 is greater than @c str2
 * @return negative if @c str1 is less than @c str2
 */
extern int strcmp(const char *str1, const char *str2);

/**
 * Compares up to @c n bytes of two strings lexicographically. This function is
 * a bounded version of #strcmp().
 *
 * @param str1 first string
 * @param str2 second string
 * @param n max number of symbols to compare
 * @return comparison result according to the lexicographical order
 * @retval 0 if the strings are equal
 * @return positive if @c str1 is greater than @c str2
 * @return negative if @c str1 is less than @c str2
 */
extern int strncmp(const char *str1, const char *str2, size_t n);

/**
 * Find the first occurrence of C in S.
 */
extern char *strchr(const char *s, int c);

/**
 * Find the last occurrence of C in S.
 */
extern char *strrchr(const char *s, int c);

/**
 * Append src on the end of dest.
 */
extern char *strcat(char *dest, const char *src);
extern char *strncat(char *dest, const char *src, size_t n);

/**
 * Compares the two strings s1 and s2.
 * @return an integer less than, equal to, or greater than zero
 *         if s1 is found, respectively.
 */
extern int strcoll(const char *s1, const char *s2);

/**
 * Find the first occurrence in S of any character in ACCEPT.
 */
extern char *strpbrk(const char *s, const char *accept);

/**
 * Parse S into tokens separated by characters in DELIM.
 */
extern char *strtok(char *str, const char *delim);

/**
 * Calculates the length of the initial segment of s
 * which consists entirely of characters in accept.
 * @return the number of characters in the initial segment
 *         of s which consist only of characters from accept.
 */
extern size_t strspn(const char *s, const char *accept);
extern size_t strcspn(const char *s, const char *reject);

/**
 * Returns the string representation of an error number e.g. errno.
 *
 * @param errnum error code
 * @return C-style string, containing an error message
 */
extern char *strerror(int errnum);

/**
 * comparing 2 massive of bytes
 * @param dst first mass
 * @param src second mass
 * @return 0 if equal
 * @return positive if dst > src
 * @return negotive if  dst < src
 */
extern int memcmp(const void *dst, const void *src, size_t n);

/**
 * copy one massive of bytes to another
 * @param dst pointer to first byte in dst mass
 * @param src pointer to first byte in src mass
 * @return pointer to first byte in dst mass
 */
extern void *memcpy(void *dst, const void *src, size_t n);

/**
 * Copy no more than N bytes of SRC to DEST, stopping when C is found.
 * @return the position in DEST one byte past where C was copied,
 *      or NULL if C was not found in the first N bytes of SRC.
 */
extern void *memccpy(void *dest, const void *src, int c, size_t n);

/**
 * set massive of bytes pointed value
 * @param p pointer to first byte in massive
 * @return pointer to first byte in massive
 */
extern void *memset(void *p, int c, size_t n);

/**
 * Search no more than N bytes of S for C.
 * @param s points to the memory area to search.
 * @param c is the character to find.
 * @param n is the maximum number of bytes to search.
 * @return pointer to the matching byte or NULL if the character
 *         does not occur in the given memory area.
 */
extern void *memchr(const void *s, int c, size_t n);
extern void *memrchr(const void *s, int c, size_t n);

/**
 * Copy n bytes of src to dest, guaranteeing
 * correct behavior for overlapping strings.
 * @return dst
 */
extern void *memmove(void *dst, const void *src, size_t n);

/**
 * Copy n bytes from src to dest. The result is correct,
 * even when both areas overlap.
 */
extern void bcopy(const void *src, void *dest, size_t n);

/**
 * function finds the first occurrence of the substring
 * needle in the string haystack. The terminating '\0'
 * characters are not compared.
 * @return pointer to the beginning of the substring,
 *         or NULL if the substring is not found.
 */
extern char *strstr(const char *haystack, const char *needle);

#endif /* STRING_H_ */
