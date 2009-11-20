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

#endif /* STDLIB_H_ */
