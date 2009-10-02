#ifndef _STRING_H_
#define _STRING_H_

#include "asm/types.h"

/**
 * character to upper case
 * @param character in any register case
 * @return symbol in upcase register
 */
char ch_upcase(char ch);

/**
 * convert digit character to integer
 * @param digit character for converting
 * @param base for converting
 * @return converted symbol
 * @return -1 if error
 */
int ch_to_digit(char ch, int base);

/**
 * is symbol digit
 *  @param checking symbol
 *  @param base
 *  @return 1 if sym is digit
 *  @return 0 if not
 */
int is_digit(int ch, int base);

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
 * set massive of bytes pointed value
 * @param p pointer to first byte in massive
 * @return pointer to first byte in massive
 */
void *memset(void *p, int c, size_t n);

#endif //_STRING_H_
