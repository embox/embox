/**
 * @file
 * @brief http://pubs.opengroup.org/onlinepubs/009695399/basedefs/float.h.html
 *
 * @date Jan 17, 2013
 * @author: Anton Bondarev
 */

#ifndef POSIX_FLOAT_H_
#define POSIX_FLOAT_H_



#define DBL_DIG          16      /* Number of significant digits in a floating point number.*/
#define DBL_EPSILON      (1e-15) /* The smallest x for which 1.0+x != 1.0. */
#define DBL_MANT_BITS    64      /* Number of bits used for the mantissa. */
#define DBL_MANT_DIG     16      /* Number of FLT_RADIX digits in the mantissa. */
#define DBL_MAX_10_EXP   999     /* The maximal exponent of a floating point value expressed in base 10 (see notes about FLT_MAX_EXP).*/
#define DBL_MAX_2_EXP    3321    /* The maximal exponent of a floating point value expressed in base 2 (see notes about FLT_MAX_EXP). */
#define DBL_MAX_EXP      999     /* The maximal exponent of a floating point value expressed in base FLT_RADIX; greater exponents are principally possible (up to 16383), but not supported in all math functions.*/
#define DBL_MAX          (9.999999999999999e999)   /*  The maximal floating point value (see notes about FLT_MAX_EXP). */
#define DBL_MIN_10_EXP   (-999)  /* The minimal exponent of a floating point value expressed in base 10 (see notes about FLT_MIN_EXP). */
#define DBL_MIN_2_EXP    (-3318) /* The minimal exponent of a floating point value expressed in base 2 (see notes about FLT_MIN_EXP). */
#define DBL_MIN_EXP      (-999) /* The maximal exponent of a floating point value expressed in base FLT_RADIX; smaller exponents are principally possible (up to -16383), but not supported in all math functions. */
#define DBL_MIN          (1e-999) /* The minimal floating point value (see notes about FLT_MIN_EXP). */
#define FLT_DIG          16      /* Number of significant digits in a floating point number. */
#define FLT_EPSILON      (1e-15) /* The smallest x for which 1.0+x != 1.0. */
#define FLT_MANT_BITS    64 /* Number of bits used for the mantissa. */
#define FLT_MANT_DIG     16 /*  Number of FLT_RADIX digits in the mantissa. */
#define FLT_MAX_10_EXP   999 /* The maximal exponent of a floating point value expressed in base 10 (see notes about FLT_MAX_EXP). */
#define FLT_MAX_2_EXP    3321 /*  The maximal exponent of a floating point value expressed in base 2 (see notes about FLT_MAX_EXP). */
#define FLT_MAX_EXP      999 /* The maximal exponent of a floating point value expressed in base FLT_RADIX; greater exponents are principally possible (up to 16383), but not supported in all math functions. */
#define FLT_MAX          (9.999999999999999e999) /* The maximal floating point value (see notes about FLT_MAX_EXP). */
#define FLT_MIN_10_EXP   (-999)  /*  The minimal exponent of a floating point value expressed in base 10 (see notes about FLT_MIN_EXP).*/
#define FLT_MIN_2_EXP    (-3318) /*  The minimal exponent of a floating point value expressed in base 2 (see notes about FLT_MIN_EXP). */
#define FLT_MIN_EXP      (-999) /* The minimal exponent of a floating point value expressed in base FLT_RADIX; smaller exponents are principally possible (up to -16383), but not supported in all math functions. */
#define FLT_MIN          (1e-999) /* The minimal floating point value (see notes about FLT_MIN_EXP).*/
#define FLT_NORMALIZE    1 /*  Indicates that floating point numbers should always be normalized. */
#define FLT_RADIX        10 /*  The base used for representing the exponent. */
#define FLT_ROUNDS       1  /*    Option for rounding floating point numbers during the addition. */
#define LDBL_DIG         16 /*    Number of significant digits in a floating point number. */
#define LDBL_EPSILON     (1e-15) /*   The smallest x for which 1.0+x != 1.0. */
#define LDBL_MANT_BITS   64/*   Number of bits used for the mantissa. */
#define LDBL_MANT_DIG    16 /*    Number of FLT_RADIX digits in the mantissa. */
#define LDBL_MAX_10_EXP  999 /*  The maximal exponent of a floating point value expressed in base 10 (see notes about FLT_MAX_EXP). */
#define LDBL_MAX_2_EXP   3321 /*  The maximal exponent of a floating point value expressed in base 2 (see notes about FLT_MAX_EXP). */
#define LDBL_MAX_EXP     999 /* The maximal exponent of a floating point value expressed in base FLT_RADIX; greater exponents are principally possible (up to 16383), but not supported in all math functions.*/
#define LDBL_MAX        (9.999999999999999e999) /* The maximal floating point value (see notes about FLT_MAX_EXP). */
#define LDBL_MIN_10_EXP (-999) /* The minimal exponent of a floating point value expressed in base 10 (see notes about FLT_MIN_EXP). */
#define LDBL_MIN_2_EXP  (-3318) /* The minimal exponent of a floating point value expressed in base 2 (see notes about FLT_MIN_EXP). */
#define LDBL_MIN_EXP    (-999) /* The maximal exponent of a floating point value expressed in base FLT_RADIX; smaller exponents are principally possible (up to -16383), but not supported in all math functions. */
#define LDBL_MIN        (1e-999) /* The minimal floating point value (see notes about FLT_MIN_EXP). */

#endif /* POSIX_FLOAT_H_ */
