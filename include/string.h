#ifndef _STRING_H_
#define _STRING_H_

#include "types.h"

/**
 * calculate length of string
 * @param pointer to  first symbol in string
 * @return string length
 */
int strlen(const char *str);

/**
 * copy string
 * @param pointer to first symbol in source string
 * @param pointer to first symbol in destination string
 * @return pointer to result string
 */
char *strcpy(char *dest, const char *src);

/**
 * copy not more then n symbols in string
 * @param pointer to first symbol in source string
 * @param pointer to first symbol in destination string
 * @param max symbols quantity for copy
 * @return pointer to result string
 */
char *strncpy(char *dest, const char *source, size_t count);

/**
 * string comparing
 * @param pointer to first symbol in source string
 * @param pointer to first symbol in destination string
 * @return 0 if string is equal
 * @return 1 if str1 greater then str2
 * @return -1 if str2 greater then str1
 */
int strcmp(const char *str1, const char *str2);

/**
 * string comparing max n symbols
 * @param pointer to first symbol in source string
 * @param pointer to first symbol in destination string
 * @return 0 if string is equal
 * @return 1 if str1 greater then str2
 * @return -1 if str2 greater then str1
 */
int strncmp(const char *s1, const char *s2, size_t count);

/**
 * Find the first occurrence of C in S.
 */
char *strchr(const char *s, int c);

/**
 * Find the last occurrence of C in S.
 */
char *strrchr(const char *s, int c);

/**
 * Append src on the end of dest.
 */
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t n);

/**
 * Compares the two strings s1 and s2.
 * @return an integer less than, equal to, or greater than zero
 *         if s1 is found, respectively.
 */
int strcoll(const char *s1, const char *s2);

/**
 * Find the first occurrence in S of any character in ACCEPT.
 */
char *strpbrk(const char *s, const char *accept);

/**
 * Calculates the length of the initial segment of s
 * which consists entirely of characters in accept.
 * @return the number of characters in the initial segment
 *         of s which consist only of characters from accept.
 */
size_t strspn(const char *s, const char *accept);
size_t strcspn(const char *s, const char *reject);

/**
 * Determines whether beg is the beginning of the str string
 */
//int str_starts_with(const char *str, const char *beg, int len);

/**
 * comparing 2 massive of bytes
 * @param dst first mass
 * @param src second mass
 * @return 0 if equal
 * @return positive if dst > src
 * @return negotive if  dst < src
 */
int memcmp(const void *dst, const void *src, size_t n);

/**
 * copy one massive of bytes to another
 * @param dst pointer to first byte in dst mass
 * @param src pointer to first byte in src mass
 * @return pointer to first byte in dst mass
 */
void *memcpy(void *dst, const void *src, size_t n);

/**
 * Copy no more than N bytes of SRC to DEST, stopping when C is found.
 * @return the position in DEST one byte past where C was copied,
 *      or NULL if C was not found in the first N bytes of SRC.
 */
void *memccpy(void *dest, const void *src, int c, size_t n);

/**
 * set massive of bytes pointed value
 * @param p pointer to first byte in massive
 * @return pointer to first byte in massive
 */
void *memset(void *p, int c, size_t n);

/**
 * Search no more than N bytes of S for C.
 * @param s points to the memory area to search.
 * @param c is the character to find.
 * @param n is the maximum number of bytes to search.
 * @return pointer to the matching byte or NULL if the character
 *         does not occur in the given memory area.
 */
void *memchr(const void *s, int c, size_t n);

/**
 * Copy n bytes of src to dest, guaranteeing
 * correct behavior for overlapping strings.
 * @return dst
 */
void *memmove( void *dst, const void *src, size_t n);

/**
 * Copy n bytes from src to dest. The result is correct,
 * even when both areas overlap.
 */
void bcopy(const void *src, void *dest, size_t n);

/**
 * function finds the first occurrence of the substring
 * needle in the string haystack. The terminating '\0'
 * characters are not compared.
 * @return pointer to the beginning of the substring,
 *         or NULL if the substring is not found.
 */
char *strstr(const char *haystack, const char *needle);

#endif //_STRING_H_
