/**
 * @file
 *
 * @date May 29, 2014
 * @author: Anton Bondarev
 */

#ifndef REGEX_H_
#define REGEX_H_

#include <sys/types.h>
#include <setjmp.h>

typedef jmp_buf sigjmp_buf;

struct regex {
	size_t re_nsub; /** number of parent hesised subexpressions */
} regex_t;

typedef ssize_t regoff_t;

struct regmatch {
	regoff_t    rm_so; /** byte offset from start of string to start of substring */
	regoff_t    rm_eo; /**    byte offset from start of string of the first character after the end of substring */
} regmatch_t;


//extern int    regcomp(regex_t *, const char *, int);
//extern int    regexec(const regex_t *, const char *, size_t, regmatch_t[], int);
//extern size_t regerror(int, const regex_t *, char *, size_t);
//extern void   regfree(regex_t *);

#endif /* REGEX_H_ */
