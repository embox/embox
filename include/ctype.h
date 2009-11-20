/**
 * \file ctype.h
 * \date 14.10.09
 * \author Sikmir
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

extern unsigned char _ctype[];

#define __ismask(x) (_ctype[(int)(unsigned char)(x)])

/* Сhecks for an alphanumeric character. */
#define isalnum(c)      ((__ismask(c)&(_U|_L|_D)) != 0)
/* Сhecks for an alphabetic character. */
#define isalpha(c)      ((__ismask(c)&(_U|_L)) != 0)
/* Сhecks for a control character. */
#define iscntrl(c)      ((__ismask(c)&(_C)) != 0)
/* Сhecks for a digit (0 through 9). */
#define isdigit(c)      ((__ismask(c)&(_D)) != 0)
/* Сhecks for any printable character except space. */
#define isgraph(c)      ((__ismask(c)&(_P|_U|_L|_D)) != 0)
/* Сhecks for a lower-case character. */
#define islower(c)      ((__ismask(c)&(_L)) != 0)
/* Сhecks for any printable character including space. */
#define isprint(c)      ((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
/* Сhecks for any printable character which is not a space or an alphanumeric character. */
#define ispunct(c)      ((__ismask(c)&(_P)) != 0)
/* Checks for white-space characters. */
#define isspace(c)      ((__ismask(c)&(_S)) != 0)
/* Checks for an uppercase letter. */
#define isupper(c)      ((__ismask(c)&(_U)) != 0)
/* Checks for a hexadecimal digits. */
#define isxdigit(c)     ((__ismask(c)&(_D|_X)) != 0)

/* Сhecks whether c is a 7-bit unsigned char value that fits into the ASCII character set. */
#define isascii(c) (((unsigned char)(c))<=0x7f)
#define toascii(c) (((unsigned char)(c))&0x7f)

static inline unsigned char __tolower(unsigned char c) {
        if (isupper(c))
                c -= 'A'-'a';
        return c;
}

static inline unsigned char __toupper(unsigned char c) {
        if (islower(c))
    		c -= 'a'-'A';
	return c;
}

/* Convert a character to lower case */
#define tolower(c) __tolower(c)

/* Convert a character to upper case */
#define toupper(c) __toupper(c)

#endif /* CTYPE_H_ */
