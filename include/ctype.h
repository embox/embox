/**
 * \file ctype.h
 * \date 14.10.09
 * \author Sikmir
 */
#ifndef CTYPE_H_
#define CTYPE_H_

#define in_range(c, lo, up)  (((u8_t)c >= lo) && ((u8_t)c <= up))
#define isprint(c)           in_range(c, 0x20, 0x7f)
#define isxdigit(c)          (in_range(c,'0','9') || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))
#define islower(c)           in_range(c, 'a', 'z')
#define isspace(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')

/* checks for an alphanumeric character. */
//int isalnum(int c);

/* checks  for  an  alphabetic  character. */
int isalpha(int c);

/* checks whether c is a 7-bit unsigned char value that fits into the ASCII character set. */
//int isascii(int c);

/* checks for a blank character; that is, a space or a tab. */
//int isblank(int c);

/* checks for a control character. */
//int iscntrl(int c);

/* checks for a digit (0 through 9). */
#define isdigit(c) in_range(c,'0','9')

/* checks for any printable character except space. */
//int isgraph(int c);

/* checks for a lower-case character. */
//int islower(int c);

/* checks for any printable character including space. */
//int isprint(int c);

/* checks for any printable character which is not a space or an alphanumeric character. */
//int ispunct(int c);

/* checks for white-space characters. */
//int isspace(int c);

/* checks for an uppercase letter. */
//int isupper(int c);

/* checks for a hexadecimal digits. */
//int isxdigit(int c);

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

#endif /* CTYPE_H_ */
