/*
 * math_privat.h
 *
 *  Created on: Mar 31, 2022
 *      Author: anton
 */

#ifndef SRC_COMPAT_LIBC_MATH_MATH_PRIVATE_H_
#define SRC_COMPAT_LIBC_MATH_MATH_PRIVATE_H_

#include <sys/types.h>
#include <float.h>
#include <endian.h>

/*
 * The original fdlibm code used statements like:
 *	n0 = ((*(int*)&one)>>29)^1;		* index of high word *
 *	ix0 = *(n0+(int*)&x);			* high word of x *
 *	ix1 = *((1-n0)+(int*)&x);		* low word of x *
 * to dig two 32 bit words out of the 64 bit IEEE floating point
 * value.  That is non-ANSI, and, moreover, the gcc instruction
 * scheduler gets it wrong.  We instead use the following macros.
 * Unlike the original code, we determine the endianness at compile
 * time, not at run time; I don't see much benefit to selecting
 * endianness at run time.
 */

/*
 * A union which permits us to convert between a double and two 32 bit
 * ints.
 */

#if BYTE_ORDER == __ORDER_BIG_ENDIAN__

typedef union
{
  double value;
  struct
  {
    uint32_t msw;
    uint32_t lsw;
  } parts;
} ieee_double_shape_type;

#endif

#if BYTE_ORDER == __ORDER_LITTLE_ENDIAN__

typedef union
{
  double value;
  struct
  {
    uint32_t lsw;
    uint32_t msw;
  } parts;
} ieee_double_shape_type;

#endif

/* Get two 32 bit ints from a double.  */

#define EXTRACT_WORDS(ix0,ix1,d)				\
do {								\
  ieee_double_shape_type ew_u;					\
  ew_u.value = (d);						\
  (ix0) = ew_u.parts.msw;					\
  (ix1) = ew_u.parts.lsw;					\
} while (0)

/* Get the more significant 32 bit int from a double.  */

#define GET_HIGH_WORD(i,d)					\
do {								\
  ieee_double_shape_type gh_u;					\
  gh_u.value = (d);						\
  (i) = gh_u.parts.msw;						\
} while (0)

/* Get the less significant 32 bit int from a double.  */

#define GET_LOW_WORD(i,d)					\
do {								\
  ieee_double_shape_type gl_u;					\
  gl_u.value = (d);						\
  (i) = gl_u.parts.lsw;						\
} while (0)

/* Set a double from two 32 bit ints.  */

#define INSERT_WORDS(d,ix0,ix1)					\
do {								\
  ieee_double_shape_type iw_u;					\
  iw_u.parts.msw = (ix0);					\
  iw_u.parts.lsw = (ix1);					\
  (d) = iw_u.value;						\
} while (0)

/* Set the more significant 32 bits of a double from an int.  */

#define SET_HIGH_WORD(d,v)					\
do {								\
  ieee_double_shape_type sh_u;					\
  sh_u.value = (d);						\
  sh_u.parts.msw = (v);						\
  (d) = sh_u.value;						\
} while (0)

/* Set the less significant 32 bits of a double from an int.  */

#define SET_LOW_WORD(d,v)					\
do {								\
  ieee_double_shape_type sl_u;					\
  sl_u.value = (d);						\
  sl_u.parts.lsw = (v);						\
  (d) = sl_u.value;						\
} while (0)

/*
 * A union which permits us to convert between a float and a 32 bit
 * int.
 */

typedef union
{
  float value;
  /* FIXME: Assumes 32 bit int.  */
  unsigned int word;
} ieee_float_shape_type;

/* Get a 32 bit int from a float.  */

#define GET_FLOAT_WORD(i,d)					\
do {								\
  ieee_float_shape_type gf_u;					\
  gf_u.value = (d);						\
  (i) = gf_u.word;						\
} while (0)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(d,i)					\
do {								\
  ieee_float_shape_type sf_u;					\
  sf_u.word = (i);						\
  (d) = sf_u.value;						\
} while (0)

static inline  double local_scalbn( double x, int n )
{
	union{ uint64_t u; double d;} u;

	uint32_t absn = n >> (8*sizeof(n)-1);
	absn = (n ^ absn) - absn;

	if( absn > 1022 )
	{
		// step = copysign( 1022, n )
		int signMask = n >> (8 * sizeof( int ) - 1);
		int step = (1022 ^ signMask) - signMask;

		u.u = ( (int64_t) step + 1023ULL) << 52;

		if( n < 0 )
		{
			do
			{
				x *= u.d;
				n -= step;
			}while( n < -1022 );
		}
		else
		{
			do
			{
				x *= u.d;
				n -= step;
			}while( n > 1022 );
		}
	}

	//create 2**n in double
	u.u = ( (int64_t) n + 1023ULL) << 52;

	//scale by appropriate power of 2
	x *= u.d;

	//return result
	return x;
}


#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384 && __BYTE_ORDER == __LITTLE_ENDIAN
union ldshape {
	long double f;
	struct {
		uint64_t m;
		uint16_t se;
	} i;
};
#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384 && __BYTE_ORDER == __BIG_ENDIAN
/* This is the m68k variant of 80-bit long double, and this definition only works
 * on archs where the alignment requirement of uint64_t is <= 4. */
union ldshape {
	long double f;
	struct {
		uint16_t se;
		uint16_t pad;
		uint64_t m;
	} i;
};
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384 && __BYTE_ORDER == __LITTLE_ENDIAN
union ldshape {
	long double f;
	struct {
		uint64_t lo;
		uint32_t mid;
		uint16_t top;
		uint16_t se;
	} i;
	struct {
		uint64_t lo;
		uint64_t hi;
	} i2;
};
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384 && __BYTE_ORDER == __BIG_ENDIAN
union ldshape {
	long double f;
	struct {
		uint16_t se;
		uint16_t top;
		uint32_t mid;
		uint64_t lo;
	} i;
	struct {
		uint64_t hi;
		uint64_t lo;
	} i2;
};
#else
#error Unsupported long double representation
#endif


#define FORCE_EVAL(x) do {                        \
	if (sizeof(x) == sizeof(float)) {         \
		volatile float __x;               \
		__x = (x);                        \
	} else if (sizeof(x) == sizeof(double)) { \
		volatile double __x;              \
		__x = (x);                        \
	} else {                                  \
		volatile long double __x;         \
		__x = (x);                        \
	}                                         \
} while(0)



#if BYTE_ORDER == __BIG_ENDIAN
typedef union {
	long double value;
	struct {
		unsigned int sign_exponent :16;
		unsigned int empty :16;
		uint32_t msw;
		uint32_t lsw;
	} parts;
} ieee_long_double_shape_type;
#endif
#if BYTE_ORDER == __LITTLE_ENDIAN
typedef union {
	long double value;
	struct {
		uint32_t lsw;
		uint32_t msw;
		unsigned int sign_exponent :16;
		unsigned int empty :16;
	} parts;
} ieee_long_double_shape_type;
#endif
/* Get int from the exponent of a long double.  */
#define GET_LDOUBLE_EXP(exp,d)                                  \
do {                                                            \
  ieee_long_double_shape_type ge_u;                             \
  ge_u.value = (d);                                             \
  (exp) = ge_u.parts.sign_exponent;                             \
} while (0)

#if BYTE_ORDER == __BIG_ENDIAN
typedef union {
	long double value;
	struct {
		uint64_t msw;
		uint64_t lsw;
	} parts64;
	struct {
		uint32_t w0, w1, w2, w3;
	} parts32;
} ieee_quad_double_shape_type;
#endif
#if BYTE_ORDER == __LITTLE_ENDIAN
typedef union {
	long double value;
	struct {
		uint64_t lsw;
		uint64_t msw;
	} parts64;
	struct {
		uint32_t w3, w2, w1, w0;
	} parts32;
} ieee_quad_double_shape_type;
#endif
/* Get most significant 64 bit int from a quad long double.  */
#define GET_LDOUBLE_MSW64(msw,d)				\
do {								\
  ieee_quad_double_shape_type qw_u;				\
  qw_u.value = (d);						\
  (msw) = qw_u.parts64.msw;					\
} while (0)


#endif /* SRC_COMPAT_LIBC_MATH_MATH_PRIVATE_H_ */
