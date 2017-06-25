/**
 * @file
 *
 * @date May 29, 2014
 * @author: Anton Bondarev
 */

#ifndef REGEX_H_
#define REGEX_H_

#include <sys/types.h>
#include <sys/cdefs.h>
#include <setjmp.h>

typedef jmp_buf sigjmp_buf;

typedef struct regex {
	size_t re_nsub; /** number of parenthesised subexpressions */
	void *regex_extended;
	char regex_error[40];
} regex_t;

typedef ssize_t regoff_t;

typedef struct regmatch {
	regoff_t    rm_so; /** byte offset from start of string to start of substring */
	regoff_t    rm_eo; /** byte offset from start of string of the first character after the end of substring */
} regmatch_t;

__BEGIN_DECLS
extern int    regcomp(regex_t *, const char *, int);
extern int    regexec(const regex_t *, const char *, size_t, regmatch_t[], int);
extern size_t regerror(int, const regex_t *, char *, size_t);
extern void   regfree(regex_t *);
__END_DECLS

/* Values for the cflags parameter to the regcomp() function are as follows: */
#define REG_EXTENDED 0x1 /* Use Extended Regular Expressions. */
#define REG_ICASE    0x2 /* Ignore case in match. */
#define REG_NOSUB    0x4 /* Report only success or fail in regexec(). */
#define REG_NEWLINE  0x8 /* Change the handling of newline. */

/* Values for the eflags parameter to the regexec() function: */

/* The circumflex character (^), when taken as a special character, will not match
 * the beginning of string. */
#define REG_NOTBOL  0x1
 /* The dollar sign ($), when taken as a special character, will not match the end
  * of string.  */
#define REG_NOTEOL  0x2


/* The following constants are defined as error return values: */
#define REG_NOMATCH 0x1 /* regexec() failed to match. */
#endif /* REGEX_H_ */
