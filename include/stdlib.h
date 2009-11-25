/**
 * \file stdlib.h
 * \date 20.11.09
 * \author Sikmir
 */
#ifndef STDLIB_H_
#define STDLIB_H_

/**
 * convert digit character to integer
 * @param digit character for converting
 * @param base for converting
 * @return converted symbol
 * @return -1 if error
 */
extern int ch_to_digit(char ch, int base);

/**
 * Convert a string to a floating-point number.
 */
extern double atof(const char *nptr);

/**
 * Convert a string to an integer.
 */
extern int atoi(const char *nptr);

/**
 * Convert a string to a long integer.
 */
extern long int atol(const char *nptr);

/**
 * Convert a string to a floating-point number.
 */
extern double strtod(const char *nptr, char **endptr);

/**
 * Converts the initial part of the string in nptr to a long
 * integer value according to the given base.
 */
extern long int strtol(const char *nptr, char **endptr, int base);

/**
 * Return directory part of PATH or "." if none is available.
 */
extern char *dirname(char *path);

#endif /* STDLIB_H_ */
