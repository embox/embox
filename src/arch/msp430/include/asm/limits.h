/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.08.2013
 */

#ifndef ASM_LIMITS_H_
#define ASM_LIMITS_H_

#ifndef __ASSEMBLER__

#define CHAR_BIT 	8
#define CHAR_MIN 	SCHAR_MIN
#define CHAR_MAX 	SCHAR_MAX
#define SCHAR_MIN 	-127
#define SCHAR_MAX 	128
#define UCHAR_MAX 	255

#define WORD_BIT 	16
#define INT_MIN		(-INT_MAX - 1)
#define INT_MAX		32767
#define UINT_MAX	65535

#define SHRT_MIN	(-32768)
#define SHRT_MAX	32767
#define USHRT_MAX	65535

#define LONG_BIT	32
#define LONG_MIN	(-LONG_MAX - 1)
#define LONG_MAX	2147483647L
#define ULONG_MAX	4294967295UL

#define LLONG_MIN	(-LLONG_MAX - 1)
#define LLONG_MAX	9223372036854775807LL

#define ULLONG_MAX	18446744073709551615ULL

#endif /* __ASSEMBLER__ */

#endif /* ASM_LIMITS_H_ */
