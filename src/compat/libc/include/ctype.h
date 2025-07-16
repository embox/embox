/**
 * @file
 * @brief ISO C99 Standard 7.4: Character handling.
 * @details Contains declarations for character classification functions.
 *
 * @date 14.10.09
 * @author Nikolay Korotky
 */

#ifndef COMPAT_LIBC_CTYPE_H_
#define COMPAT_LIBC_CTYPE_H_

#define _CTYPE_U      0x01    /* upper */
#define _CTYPE_L      0x02    /* lower */
#define _CTYPE_D      0x04    /* digit */
#define _CTYPE_C      0x08    /* cntrl */
#define _CTYPE_P      0x10    /* punct */
#define _CTYPE_S      0x20    /* white space (space/lf/tab) */
#define _CTYPE_X      0x40    /* hex digit */
#define _CTYPE_SP     0x80    /* hard space (0x20) */

extern const unsigned char _ctype_[];

#define __ismask(x) (_ctype_[(int)(unsigned char)(x)])

#include <sys/cdefs.h>
#include <stdint.h>
__BEGIN_DECLS

#ifdef __GNUC__
#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 5 ))

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"

#endif /* __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 5 )) */
#endif /*__GNUC__ */

/* Checks for an alphanumeric character. */
static inline int isalnum(int c) { return __ismask(c) & (_CTYPE_U | _CTYPE_L | _CTYPE_D); }
/* Checks for an alphabetic character. */
static inline int isalpha(int c) { return __ismask(c) & (_CTYPE_U | _CTYPE_L); }
/* Checks for an blank character - space or tab */
static inline int isblank(int c) { return (c == ' ' || c == '\t'); }
/* Checks for a control character. */
static inline int iscntrl(int c) { return __ismask(c) & (_CTYPE_C); }
/* Checks for a digit (0 through 9). */
static inline int isdigit(int c) { return __ismask(c) & (_CTYPE_D); }
/* Checks for any printable character except space. */
static inline int isgraph(int c) { return __ismask(c) & (_CTYPE_P | _CTYPE_U | _CTYPE_L | _CTYPE_D); }
/* Checks for a lower-case character. */
static inline int islower(int c) { return __ismask(c) & (_CTYPE_L); }
/* Checks for any printable character including space. */
static inline int isprint(int c) { return __ismask(c) & (_CTYPE_P | _CTYPE_U | _CTYPE_L | _CTYPE_D | _CTYPE_SP); }
/* Checks for any printable character which is not a space
 * or an alphanumeric character. */
static inline int ispunct(int c) { return __ismask(c) & (_CTYPE_P); }
/* Checks for white-space characters. */
//FIXME enable different variants for isspace
extern int isspace(int c);
/* Checks for an uppercase letter. */
static inline int isupper(int c) { return __ismask(c) & (_CTYPE_U); }
/* Checks for a hexadecimal digits. */
static inline int isxdigit(int c) { return __ismask(c) & (_CTYPE_D | _CTYPE_X); }
/* Convert a character to lower case */
static inline int tolower(int c) { return isupper(c) ? c - ('A' - 'a') : c; }
/* Convert a character to upper case */
static inline int toupper(int c) { return islower(c) ? c - ('a' - 'A') : c; }

#ifdef __GNUC__
#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 5 ))

#pragma GCC diagnostic pop

#endif /* __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 5 )) */
#endif /*__GNUC__ */

extern const unsigned short **__ctype_b_loc(void);
extern int32_t **__ctype_tolower_loc(void);
extern int32_t **__ctype_toupper_loc(void);

__END_DECLS

/* Checks whether c is a 7-bit unsigned char value that
 * fits into the ASCII character set. */
#define isascii(c) (((unsigned char)(c))<=0x7f)
#define toascii(c) (((unsigned char)(c))&0x7f)

#endif /* COMPAT_LIBC_CTYPE_H_ */
