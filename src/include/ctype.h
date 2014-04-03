/**
 * @file
 * @brief ISO C99 Standard 7.4: Character handling.
 * @details Contains declarations for character classification functions.
 *
 * @date 14.10.09
 * @author Nikolay Korotky
 */

#ifndef CTYPE_H_
#define CTYPE_H_

#define _U      0x01    /* upper */
#define _L      0x02    /* lower */
#define _D      0x04    /* digit */
#define _C      0x08    /* cntrl */
#define _P      0x10    /* punct */
#define _S      0x20    /* white space (space/lf/tab) */
#define _X      0x40    /* hex digit */
#define _SP     0x80    /* hard space (0x20) */

extern const unsigned char _ctype[];

#define __ismask(x) (_ctype[(int)(unsigned char)(x)])

#include <sys/cdefs.h>
__BEGIN_DECLS

/* Checks for an alphanumeric character. */
static inline int isalnum(int c) { return __ismask(c) & (_U | _L | _D); }
/* Checks for an alphabetic character. */
static inline int isalpha(int c) { return __ismask(c) & (_U | _L); }
/* Checks for an blank character - space or tab */
static inline int isblank(int c) { return (c == ' ' || c == '\t'); }
/* Checks for a control character. */
static inline int iscntrl(int c) { return __ismask(c) & (_C); }
/* Checks for a digit (0 through 9). */
static inline int isdigit(int c) { return __ismask(c) & (_D); }
/* Checks for any printable character except space. */
static inline int isgraph(int c) { return __ismask(c) & (_P | _U | _L | _D); }
/* Checks for a lower-case character. */
static inline int islower(int c) { return __ismask(c) & (_L); }
/* Checks for any printable character including space. */
static inline int isprint(int c) { return __ismask(c) & (_P | _U | _L | _D | _SP); }
/* Checks for any printable character which is not a space
 * or an alphanumeric character. */
static inline int ispunct(int c) { return __ismask(c) & (_P); }
/* Checks for white-space characters. */
static inline int isspace(int c) { return __ismask(c) & (_S); }
/* Checks for an uppercase letter. */
static inline int isupper(int c) { return __ismask(c) & (_U); }
/* Checks for a hexadecimal digits. */
static inline int isxdigit(int c) { return __ismask(c) & (_D | _X); }
/* Convert a character to lower case */
static inline int tolower(int c) { return isupper(c) ? c - ('A' - 'a') : c; }
/* Convert a character to upper case */
static inline int toupper(int c) { return islower(c) ? c - ('a' - 'A') : c; }

__END_DECLS

/* Checks whether c is a 7-bit unsigned char value that
 * fits into the ASCII character set. */
#define isascii(c) (((unsigned char)(c))<=0x7f)
#define toascii(c) (((unsigned char)(c))&0x7f)

#endif /* CTYPE_H_ */
