/**
 * \file ctype.h
 * \date 14.10.09
 * \author Sikmir
 */
#ifndef CTYPE_H_
#define CTYPE_H_

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
int isdigit(int c, int base);

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

#endif /* CTYPE_H_ */
