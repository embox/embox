/**
 * @file
 * @brief https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/float.h.html
 *
 * @date Jan 17, 2013
 * @author: Anton Bondarev
 */

#ifndef COMPAT_POSIX_FLOAT_H_
#define COMPAT_POSIX_FLOAT_H_

/**
 * The rounding mode for floating-point addition.
 * 
 * -1 Indeterminable
 *  0 Toward zero
 *  1 To nearest
 *  2 Toward positive infinity
 *  3 Toward negative infinity
 */
#define FLT_ROUNDS       1

/**
 * Evaluation format of operations and constants of float and double type.
 * 
 * -1 Indeterminable
 *  0 Evaluate to the range and precision of the type
 *  1 Evaluate to the range and precision of the double
 *  2 Evaluate to the range and precision of the long double
 */
#define FLT_EVAL_METHOD  __FLT_EVAL_METHOD__

/**
 * Radix of exponent representation
 */
#define FLT_RADIX        __FLT_RADIX__

/**
 * Number of base-FLT_RADIX digits in float/double/long double type.
 */
#define FLT_MANT_DIG     __FLT_MANT_DIG__
#define DBL_MANT_DIG     __DBL_MANT_DIG__
#define LDBL_MANT_DIG    __LDBL_MANT_DIG__

/** 
 * Number of decimal digits that can be converted to
 * float/double/long double type and back without losing precision
 */
#define DECIMAL_DIG      __DECIMAL_DIG__
#define FLT_DECIMAL_DIG  __FLT_DECIMAL_DIG__
#define DBL_DECIMAL_DIG  __DBL_DECIMAL_DIG__
#define LDBL_DECIMAL_DIG __DECIMAL_DIG__

/** 
 * Number of decimal digits that can be represented
 * without losing precision for float/double/long double type
 */
#define FLT_DIG          __FLT_DIG__
#define DBL_DIG          __DBL_DIG__
#define LDBL_DIG         __LDBL_DIG__

/** 
 * Minimum negative integer such that FLT_RADIX raised to that power minus 1
 * is a normalized float/double/long double type number
 */
#define FLT_MIN_EXP      __FLT_MIN_EXP__
#define DBL_MIN_EXP      __DBL_MIN_EXP__
#define LDBL_MIN_EXP     __LDBL_MIN_EXP__

/** 
 * Minimum negative integer such that 10 raised to that power
 * is in the range of normalized float/double/long double type numbers
 */
#define FLT_MIN_10_EXP   __FLT_MIN_10_EXP__
#define DBL_MIN_10_EXP   __DBL_MIN_10_EXP__
#define LDBL_MIN_10_EXP  __LDBL_MIN_10_EXP__

/** 
 * Maximum integer such that FLT_RADIX raised to that power minus 1
 * is a representable finite float/double/long double type number
 */
#define FLT_MAX_EXP      __FLT_MAX_EXP__
#define DBL_MAX_EXP      __DBL_MAX_EXP__
#define LDBL_MAX_EXP     __LDBL_MAX_EXP__

/** 
 * Maximum integer such that 10 raised to that power
 * is in the range of representable finite float/double/long double type numbers
 */
#define FLT_MAX_10_EXP   __FLT_MAX_10_EXP__
#define DBL_MAX_10_EXP   __DBL_MAX_10_EXP__
#define LDBL_MAX_10_EXP  __LDBL_MAX_10_EXP__

/** 
 * Maximum representable finite float/double/long double type number
 */
#define FLT_MAX          __FLT_MAX__
#define DBL_MAX          __DBL_MAX__
#define LDBL_MAX         __LDBL_MAX__

/** 
 * Minimum normalized positive float/double/long double type number
 */
#define FLT_MIN          __FLT_MIN__
#define DBL_MIN          __DBL_MIN__
#define LDBL_MIN         __LDBL_MIN__

/** 
 * Minimum positive float/double/long double type number
 */
#define FLT_TRUE_MIN     __FLT_MIN__
#define DBL_TRUE_MIN     __DBL_MIN__
#define LDBL_TRUE_MIN    __LDBL_MIN__

/** 
 * The difference between 1 and the least value greater than 1
 * that is representable in float/double/long double type
 */
#define FLT_EPSILON      __FLT_EPSILON__
#define DBL_EPSILON      __DBL_EPSILON__
#define LDBL_EPSILON     __LDBL_EPSILON__

#endif /* COMPAT_POSIX_FLOAT_H_ */
