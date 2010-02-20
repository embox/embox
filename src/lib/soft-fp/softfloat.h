/**
 * @file
 *
 * @brief Software FPU emulation.
 *
 * @date 25.11.09
 * @author John R. Hauser - initial implementation.
 * @author Nikolay Korotky
 */
#ifndef SOFTFLOAT_H_
#define SOFTFLOAT_H_

#include <types.h>

/**
 * Each of the following `typedef's defines the most convenient type that holds
 * integers of at least as many bits as specified.  For example, `uint8' should
 * be the most convenient type that can hold unsigned integers of as many as
 * 8 bits.  The `flag' type must be able to hold either a 0 or 1.  For most
 * implementations of C, `flag', `uint8', and `int8' should all be `typedef'ed
 * to the same as `int'.
 */
typedef __s32 flag;
typedef __s32 uint8;
typedef __s32 int8;
typedef __s32 uint16;
typedef __s32 int16;
typedef __u32 uint32;
typedef __s32 int32;

/**
 * Each of the following `typedef's defines a type that holds integers
 * of _exactly_ the number of bits specified.  For instance, for most
 * implementation of C, `bits16' and `sbits16' should be `typedef'ed to
 * `unsigned short int' and `signed short int' (or `short int'), respectively.
 */
typedef __u8 bits8;
typedef __s8 sbits8;
typedef __u16 bits16;
typedef __s16 sbits16;
typedef __u32 bits32;
typedef __s32 sbits32;

/**
 * Software IEC/IEEE floating-point types.
 */
typedef unsigned int float32;
typedef struct {
	unsigned int high, low;
} float64;

/**
 * Software IEC/IEEE floating-point underflow tininess-detection mode.
 */
extern int float_detect_tininess;
enum {
	float_tininess_after_rounding  = 0,
	float_tininess_before_rounding = 1
};

/**
 * Software IEC/IEEE floating-point rounding mode.
 */
extern int float_rounding_mode;
enum {
	float_round_nearest_even = 0,
	float_round_to_zero      = 1,
	float_round_up	   = 2,
	float_round_down	 = 3
};

/**
 * Software IEC/IEEE floating-point exception flags.
 */
extern int float_exception_flags;
enum {
	float_flag_inexact   =  1,
	float_flag_divbyzero =  2,
	float_flag_underflow =  4,
	float_flag_overflow  =  8,
	float_flag_invalid   = 16
};

/**
 * Internal canonical NaN format.
 */
typedef struct {
	flag sign;
	bits32 high, low;
} commonNaNT;

/**
 * The pattern for a default generated double-precision NaN.  The `high' and
 * `low' values hold the most- and least-significant bits, respectively.
 */
enum {
	float64_default_nan_high = 0x7FFFFFFF,
	float64_default_nan_low  = 0xFFFFFFFF
};

/**
 * The pattern for a default generated single-precision NaN.
 */
enum {
	float32_default_nan = 0x7FFFFFFF
};

/**
 * Routine to raise any or all of the software IEC/IEEE floating-point
 * exception flags.
 */
void float_raise( int );

/**
 * Software IEC/IEEE integer-to-floating-point conversion routines.
 */
float32 int32_to_float32( int );
float64 int32_to_float64( int );

/**
 * Software IEC/IEEE single-precision conversion routines.
 */
extern int float32_to_int32( float32 );
extern int float32_to_int32_round_to_zero( float32 );
extern float64 float32_to_float64( float32 );

/**
 * Software IEC/IEEE single-precision operations.
 */
extern float32 float32_round_to_int( float32 );
extern float32 float32_add( float32, float32 );
extern float32 float32_sub( float32, float32 );
extern float32 float32_mul( float32, float32 );
extern float32 float32_div( float32, float32 );
extern float32 float32_rem( float32, float32 );
extern float32 float32_sqrt( float32 );
extern flag float32_eq( float32, float32 );
extern flag float32_le( float32, float32 );
extern flag float32_lt( float32, float32 );
extern flag float32_eq_signaling( float32, float32 );
extern flag float32_le_quiet( float32, float32 );
extern flag float32_lt_quiet( float32, float32 );
extern flag float32_is_signaling_nan( float32 );

/**
 * Software IEC/IEEE double-precision conversion routines.
 */
extern int float64_to_int32( float64 );
extern int float64_to_int32_round_to_zero( float64 );
extern float32 float64_to_float32( float64 );

/**
 * Software IEC/IEEE double-precision operations.
 */
extern float64 float64_round_to_int( float64 );
extern float64 float64_add( float64, float64 );
extern float64 float64_sub( float64, float64 );
extern float64 float64_mul( float64, float64 );
extern float64 float64_div( float64, float64 );
extern float64 float64_rem( float64, float64 );
extern float64 float64_sqrt( float64 );
extern flag float64_eq( float64, float64 );
extern flag float64_le( float64, float64 );
extern flag float64_lt( float64, float64 );
extern flag float64_eq_signaling( float64, float64 );
extern flag float64_le_quiet( float64, float64 );
extern flag float64_lt_quiet( float64, float64 );
extern flag float64_is_signaling_nan( float64 );

/**
 * Primitive arithmetic functions, including multi-word arithmetic, and
 * division and square root approximations.  (Can be specialized to target if
 * desired.)
 */
extern void shift32RightJamming( bits32 a, int16 count, bits32 *zPtr);
extern void shift64Right(bits32 a0, bits32 a1, int16 count, bits32 *z0Ptr, bits32 *z1Ptr);
extern void shift64RightJamming(bits32 a0, bits32 a1, int16 count, bits32 *z0Ptr, bits32 *z1Ptr);
extern void shift64ExtraRightJamming( bits32 a0, bits32 a1, bits32 a2, int16 count,
			    bits32 *z0Ptr, bits32 *z1Ptr, bits32 *z2Ptr);
extern void shortShift64Left(bits32 a0, bits32 a1, int16 count, bits32 *z0Ptr, bits32 *z1Ptr);
extern void shortShift96Left(bits32 a0, bits32 a1, bits32 a2, int16 count,
		    	    bits32 *z0Ptr, bits32 *z1Ptr, bits32 *z2Ptr);
extern void add64(bits32 a0, bits32 a1, bits32 b0, bits32 b1, bits32 *z0Ptr, bits32 *z1Ptr);
extern void add96(bits32 a0, bits32 a1, bits32 a2, bits32 b0, bits32 b1, bits32 b2,
	    		    bits32 *z0Ptr, bits32 *z1Ptr, bits32 *z2Ptr);
extern void sub64(bits32 a0, bits32 a1, bits32 b0, bits32 b1, bits32 *z0Ptr, bits32 *z1Ptr);
extern void sub96(bits32 a0, bits32 a1, bits32 a2, bits32 b0, bits32 b1, bits32 b2,
				    bits32 *z0Ptr, bits32 *z1Ptr, bits32 *z2Ptr);
extern void mul32To64(bits32 a, bits32 b, bits32 *z0Ptr, bits32 *z1Ptr);
extern void mul64By32To96(bits32 a0, bits32 a1, bits32 b, bits32 *z0Ptr,
				bits32 *z1Ptr, bits32 *z2Ptr);
extern void mul64To128(bits32 a0, bits32 a1, bits32 b0, bits32 b1,
					bits32 *z0Ptr, bits32 *z1Ptr, bits32 *z2Ptr, bits32 *z3Ptr);
extern bits32 estimateDiv64To32(bits32 a0, bits32 a1, bits32 b);
extern bits32 estimateSqrt32(int16 aExp, bits32 a);
extern int8 countLeadingZeros32(bits32 a);
extern flag eq64(bits32 a0, bits32 a1, bits32 b0, bits32 b1);
extern flag le64(bits32 a0, bits32 a1, bits32 b0, bits32 b1);
extern flag lt64(bits32 a0, bits32 a1, bits32 b0, bits32 b1);
extern flag ne64(bits32 a0, bits32 a1, bits32 b0, bits32 b1);

/**
 * Functions and definitions to determine:  (1) whether tininess for underflow
 * is detected before or after rounding by default, (2) what (if anything)
 * happens when exceptions are raised, (3) how signaling NaNs are distinguished
 * from quiet NaNs, (4) the default generated quiet NaNs, and (4) how NaNs
 * are propagated from function inputs to output.  These details are target-
 * specific.
 */
extern void float_raise(int8 flags);
extern flag float32_is_nan(float32 a);
extern flag float32_is_signaling_nan(float32 a);
extern commonNaNT float32ToCommonNaN(float32 a);
extern float32 commonNaNToFloat32(commonNaNT a);
extern float32 propagateFloat32NaN(float32 a, float32 b);
extern flag float64_is_nan(float64 a);
extern flag float64_is_signaling_nan(float64 a);
extern commonNaNT float64ToCommonNaN(float64 a);
extern float64 commonNaNToFloat64(commonNaNT a);
extern float64 propagateFloat64NaN(float64 a, float64 b);

/**
 * Converter types.
 */
extern float64 double_to_float64(double a);
extern double float64_to_double(float64 a);
extern float32 float_to_float32(float a);
extern float float32_to_float(float32 a);

#endif /* SOFTFLOAT_H_ */
