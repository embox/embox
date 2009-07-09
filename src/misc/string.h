#ifndef _STRING_H_
#define _STRING_H_

#include "types.h"

char ch_upcase(char ch);

int ch_to_digit(char ch, int base);

int is_digit(int ch, int base);

int strlen(const char *str);

char *strcpy(char *dest, const char *src);

char *strncpy(char *dest, const char *source, size_t count);

int strcmp(const char *str1, const char *str2);

int strncmp(const char *s1, const char *s2, size_t count);

// Determines whether beg is the beginning of the str string
//TODO change to strncpy
//int str_starts_with(const char *str, const char *beg, int len);

int memcmp(const void *dst, const void *src, size_t n);

void *memcpy(void *dst, const void *src, size_t n);

void *memset(void *p, int c, size_t n);

#endif //_STRING_H_
