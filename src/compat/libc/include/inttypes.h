/**
 * @file
 *
 * @date Aug 22, 2013
 * @author: Anton Bondarev
 * @author: Aleksey Zhmulin
 */

#ifndef COMPAT_LIBC_INTTYPES_H_
#define COMPAT_LIBC_INTTYPES_H_

#include <stddef.h>
#include <stdint.h> /* The <inttypes.h> header shall include the <stdint.h> header. */
#include <sys/cdefs.h>

#include <util/macro.h>

#define __PRI_TYPE__
#define __PRI_TYPE__int         int,
#define __PRI_TYPE__long        long,
#define __PRI_TYPE__char        char,
#define __PRI_TYPE__short       short,
#define __PRI_TYPE__signed      signed,
#define __PRI_TYPE__unsigned    unsigned,
#define __PRI_TYPE_APPEND(a, b) __PRI_TYPE__##a b
#define __PRI_TYPE(...)         MACRO_FOREACH(__PRI_TYPE_APPEND, __VA_ARGS__)

#define __PRI_PREFIX__
#define __PRI_PREFIX__int
#define __PRI_PREFIX__long  "l"
#define __PRI_PREFIX__char  "hh"
#define __PRI_PREFIX__short "h"
#define __PRI_PREFIX__signed
#define __PRI_PREFIX__unsigned
#define __PRI_PREFIX_APPEND(a, b) __PRI_PREFIX__##a b
#define __PRI_PREFIX(type) \
	MACRO_FOREACH(__PRI_PREFIX_APPEND, __PRI_TYPE(__PRI_TYPE(__PRI_TYPE(type))))

#ifndef __INT8_TYPE__
#define __PRI8_PREFIX "hh"
#else
#define __PRI8_PREFIX __PRI_PREFIX(__INT8_TYPE__)
#endif

#ifndef __INT16_TYPE__
#define __PRI16_PREFIX "h"
#else
#define __PRI16_PREFIX __PRI_PREFIX(__INT16_TYPE__)
#endif

#ifdef __ARM_32BIT_STATE
#define __PRI32_PREFIX
#else
#ifndef __INT32_TYPE__
#define __PRI32_PREFIX
#else
#define __PRI32_PREFIX __PRI_PREFIX(__INT32_TYPE__)
#endif
#endif /* __ARM_32BIT_STATE */

#ifndef __INT64_TYPE__
#define __PRI64_PREFIX "l"
#else
#define __PRI64_PREFIX __PRI_PREFIX(__INT64_TYPE__)
#endif

#ifndef __INT_LEAST8_TYPE__
#define __PRI_LEAST8_PREFIX "hh"
#else
#define __PRI_LEAST8_PREFIX __PRI_PREFIX(__INT_LEAST8_TYPE__)
#endif

#ifndef __INT_LEAST16_TYPE__
#define __PRI_LEAST16_PREFIX "h"
#else
#define __PRI_LEAST16_PREFIX __PRI_PREFIX(__INT_LEAST16_TYPE__)
#endif

#ifndef __INT_LEAST32_TYPE__
#define __PRI_LEAST32_PREFIX
#else
#define __PRI_LEAST32_PREFIX __PRI_PREFIX(__INT_LEAST32_TYPE__)
#endif

#ifndef __INT_LEAST64_TYPE__
#define __PRI_LEAST64_PREFIX "l"
#else
#define __PRI_LEAST64_PREFIX __PRI_PREFIX(__INT_LEAST64_TYPE__)
#endif

#ifndef __INT_FAST8_TYPE__
#define __PRI_FAST8_PREFIX "hh"
#else
#define __PRI_FAST8_PREFIX __PRI_PREFIX(__INT_FAST8_TYPE__)
#endif

#ifndef __INT_FAST16_TYPE__
#define __PRI_FAST16_PREFIX "h"
#else
#define __PRI_FAST16_PREFIX __PRI_PREFIX(__INT_FAST16_TYPE__)
#endif

#ifndef __INT_FAST32_TYPE__
#define __PRI_FAST32_PREFIX
#else
#define __PRI_FAST32_PREFIX __PRI_PREFIX(__INT_FAST32_TYPE__)
#endif

#ifndef __INT_FAST64_TYPE__
#define __PRI_FAST64_PREFIX "l"
#else
#define __PRI_FAST64_PREFIX __PRI_PREFIX(__INT_FAST64_TYPE__)
#endif

#ifndef __INTMAX_TYPE__
#define __PRIMAX_PREFIX "ll"
#else
#define __PRIMAX_PREFIX __PRI_PREFIX(__INTMAX_TYPE__)
#endif

#ifndef __INTPTR_TYPE__
#define __PRIPTR_PREFIX "l"
#else
#define __PRIPTR_PREFIX __PRI_PREFIX(__INTPTR_TYPE__)
#endif

#define PRId8       __PRI8_PREFIX "d"
#define PRIi8       __PRI8_PREFIX "i"
#define PRIo8       __PRI8_PREFIX "o"
#define PRIu8       __PRI8_PREFIX "u"
#define PRIx8       __PRI8_PREFIX "x"
#define PRIX8       __PRI8_PREFIX "X"

#define PRId16      __PRI16_PREFIX "d"
#define PRIi16      __PRI16_PREFIX "i"
#define PRIo16      __PRI16_PREFIX "o"
#define PRIu16      __PRI16_PREFIX "u"
#define PRIx16      __PRI16_PREFIX "x"
#define PRIX16      __PRI16_PREFIX "X"

#define PRId32      __PRI32_PREFIX "d"
#define PRIi32      __PRI32_PREFIX "i"
#define PRIo32      __PRI32_PREFIX "o"
#define PRIu32      __PRI32_PREFIX "u"
#define PRIx32      __PRI32_PREFIX "x"
#define PRIX32      __PRI32_PREFIX "X"

#define PRId64      __PRI64_PREFIX "d"
#define PRIi64      __PRI64_PREFIX "i"
#define PRIo64      __PRI64_PREFIX "o"
#define PRIu64      __PRI64_PREFIX "u"
#define PRIx64      __PRI64_PREFIX "x"
#define PRIX64      __PRI64_PREFIX "X"

#define PRIdLEAST8  __PRI_LEAST8_PREFIX "d"
#define PRIiLEAST8  __PRI_LEAST8_PREFIX "i"
#define PRIoLEAST8  __PRI_LEAST8_PREFIX "o"
#define PRIuLEAST8  __PRI_LEAST8_PREFIX "u"
#define PRIxLEAST8  __PRI_LEAST8_PREFIX "x"
#define PRIXLEAST8  __PRI_LEAST8_PREFIX "X"

#define PRIdLEAST16 __PRI_LEAST16_PREFIX "d"
#define PRIiLEAST16 __PRI_LEAST16_PREFIX "i"
#define PRIoLEAST16 __PRI_LEAST16_PREFIX "o"
#define PRIuLEAST16 __PRI_LEAST16_PREFIX "u"
#define PRIxLEAST16 __PRI_LEAST16_PREFIX "x"
#define PRIXLEAST16 __PRI_LEAST16_PREFIX "X"

#define PRIdLEAST32 __PRI_LEAST32_PREFIX "d"
#define PRIiLEAST32 __PRI_LEAST32_PREFIX "i"
#define PRIoLEAST32 __PRI_LEAST32_PREFIX "o"
#define PRIuLEAST32 __PRI_LEAST32_PREFIX "u"
#define PRIxLEAST32 __PRI_LEAST32_PREFIX "x"
#define PRIXLEAST32 __PRI_LEAST32_PREFIX "X"

#define PRIdLEAST64 __PRI_LEAST64_PREFIX "d"
#define PRIiLEAST64 __PRI_LEAST64_PREFIX "i"
#define PRIoLEAST64 __PRI_LEAST64_PREFIX "o"
#define PRIuLEAST64 __PRI_LEAST64_PREFIX "u"
#define PRIxLEAST64 __PRI_LEAST64_PREFIX "x"
#define PRIXLEAST64 __PRI_LEAST64_PREFIX "X"

#define PRIdFAST8   __PRI_FAST8_PREFIX "d"
#define PRIiFAST8   __PRI_FAST8_PREFIX "i"
#define PRIoFAST8   __PRI_FAST8_PREFIX "o"
#define PRIuFAST8   __PRI_FAST8_PREFIX "u"
#define PRIxFAST8   __PRI_FAST8_PREFIX "x"
#define PRIXFAST8   __PRI_FAST8_PREFIX "X"

#define PRIdFAST16  __PRI_FAST16_PREFIX "d"
#define PRIiFAST16  __PRI_FAST16_PREFIX "i"
#define PRIoFAST16  __PRI_FAST16_PREFIX "o"
#define PRIuFAST16  __PRI_FAST16_PREFIX "u"
#define PRIxFAST16  __PRI_FAST16_PREFIX "x"
#define PRIXFAST16  __PRI_FAST16_PREFIX "X"

#define PRIdFAST32  __PRI_FAST32_PREFIX "d"
#define PRIiFAST32  __PRI_FAST32_PREFIX "i"
#define PRIoFAST32  __PRI_FAST32_PREFIX "o"
#define PRIuFAST32  __PRI_FAST32_PREFIX "u"
#define PRIxFAST32  __PRI_FAST32_PREFIX "x"
#define PRIXFAST32  __PRI_FAST32_PREFIX "X"

#define PRIdFAST64  __PRI_FAST64_PREFIX "d"
#define PRIiFAST64  __PRI_FAST64_PREFIX "i"
#define PRIoFAST64  __PRI_FAST64_PREFIX "o"
#define PRIuFAST64  __PRI_FAST64_PREFIX "u"
#define PRIxFAST64  __PRI_FAST64_PREFIX "x"
#define PRIXFAST64  __PRI_FAST64_PREFIX "X"

#define PRIdMAX     __PRIMAX_PREFIX "d"
#define PRIiMAX     __PRIMAX_PREFIX "i"
#define PRIoMAX     __PRIMAX_PREFIX "o"
#define PRIuMAX     __PRIMAX_PREFIX "u"
#define PRIxMAX     __PRIMAX_PREFIX "x"
#define PRIXMAX     __PRIMAX_PREFIX "X"

#define PRIdPTR     __PRIPTR_PREFIX "d"
#define PRIiPTR     __PRIPTR_PREFIX "i"
#define PRIoPTR     __PRIPTR_PREFIX "o"
#define PRIuPTR     __PRIPTR_PREFIX "u"
#define PRIxPTR     __PRIPTR_PREFIX "x"
#define PRIXPTR     __PRIPTR_PREFIX "X"

#define SCNd8       __PRI8_PREFIX "d"
#define SCNi8       __PRI8_PREFIX "i"
#define SCNo8       __PRI8_PREFIX "o"
#define SCNu8       __PRI8_PREFIX "u"
#define SCNx8       __PRI8_PREFIX "x"
#define SCNX8       __PRI8_PREFIX "X"

#define SCNd16      __PRI16_PREFIX "d"
#define SCNi16      __PRI16_PREFIX "i"
#define SCNo16      __PRI16_PREFIX "o"
#define SCNu16      __PRI16_PREFIX "u"
#define SCNx16      __PRI16_PREFIX "x"
#define SCNX16      __PRI16_PREFIX "X"

#define SCNd32      __PRI32_PREFIX "d"
#define SCNi32      __PRI32_PREFIX "i"
#define SCNo32      __PRI32_PREFIX "o"
#define SCNu32      __PRI32_PREFIX "u"
#define SCNx32      __PRI32_PREFIX "x"
#define SCNX32      __PRI32_PREFIX "X"

#define SCNd64      __PRI64_PREFIX "d"
#define SCNi64      __PRI64_PREFIX "i"
#define SCNo64      __PRI64_PREFIX "o"
#define SCNu64      __PRI64_PREFIX "u"
#define SCNx64      __PRI64_PREFIX "x"
#define SCNX64      __PRI64_PREFIX "X"

#define SCNdLEAST8  __PRI_LEAST8_PREFIX "d"
#define SCNiLEAST8  __PRI_LEAST8_PREFIX "i"
#define SCNoLEAST8  __PRI_LEAST8_PREFIX "o"
#define SCNuLEAST8  __PRI_LEAST8_PREFIX "u"
#define SCNxLEAST8  __PRI_LEAST8_PREFIX "x"
#define SCNXLEAST8  __PRI_LEAST8_PREFIX "X"

#define SCNdLEAST16 __PRI_LEAST16_PREFIX "d"
#define SCNiLEAST16 __PRI_LEAST16_PREFIX "i"
#define SCNoLEAST16 __PRI_LEAST16_PREFIX "o"
#define SCNuLEAST16 __PRI_LEAST16_PREFIX "u"
#define SCNxLEAST16 __PRI_LEAST16_PREFIX "x"
#define SCNXLEAST16 __PRI_LEAST16_PREFIX "X"

#define SCNdLEAST32 __PRI_LEAST32_PREFIX "d"
#define SCNiLEAST32 __PRI_LEAST32_PREFIX "i"
#define SCNoLEAST32 __PRI_LEAST32_PREFIX "o"
#define SCNuLEAST32 __PRI_LEAST32_PREFIX "u"
#define SCNxLEAST32 __PRI_LEAST32_PREFIX "x"
#define SCNXLEAST32 __PRI_LEAST32_PREFIX "X"

#define SCNdLEAST64 __PRI_LEAST64_PREFIX "d"
#define SCNiLEAST64 __PRI_LEAST64_PREFIX "i"
#define SCNoLEAST64 __PRI_LEAST64_PREFIX "o"
#define SCNuLEAST64 __PRI_LEAST64_PREFIX "u"
#define SCNxLEAST64 __PRI_LEAST64_PREFIX "x"
#define SCNXLEAST64 __PRI_LEAST64_PREFIX "X"

#define SCNdFAST8   __PRI_FAST8_PREFIX "d"
#define SCNiFAST8   __PRI_FAST8_PREFIX "i"
#define SCNoFAST8   __PRI_FAST8_PREFIX "o"
#define SCNuFAST8   __PRI_FAST8_PREFIX "u"
#define SCNxFAST8   __PRI_FAST8_PREFIX "x"
#define SCNXFAST8   __PRI_FAST8_PREFIX "X"

#define SCNdFAST16  __PRI_FAST16_PREFIX "d"
#define SCNiFAST16  __PRI_FAST16_PREFIX "i"
#define SCNoFAST16  __PRI_FAST16_PREFIX "o"
#define SCNuFAST16  __PRI_FAST16_PREFIX "u"
#define SCNxFAST16  __PRI_FAST16_PREFIX "x"
#define SCNXFAST16  __PRI_FAST16_PREFIX "X"

#define SCNdFAST32  __PRI_FAST32_PREFIX "d"
#define SCNiFAST32  __PRI_FAST32_PREFIX "i"
#define SCNoFAST32  __PRI_FAST32_PREFIX "o"
#define SCNuFAST32  __PRI_FAST32_PREFIX "u"
#define SCNxFAST32  __PRI_FAST32_PREFIX "x"
#define SCNXFAST32  __PRI_FAST32_PREFIX "X"

#define SCNdFAST64  __PRI_FAST64_PREFIX "d"
#define SCNiFAST64  __PRI_FAST64_PREFIX "i"
#define SCNoFAST64  __PRI_FAST64_PREFIX "o"
#define SCNuFAST64  __PRI_FAST64_PREFIX "u"
#define SCNxFAST64  __PRI_FAST64_PREFIX "x"
#define SCNXFAST64  __PRI_FAST64_PREFIX "X"

#define SCNdMAX     __PRIMAX_PREFIX "d"
#define SCNiMAX     __PRIMAX_PREFIX "i"
#define SCNoMAX     __PRIMAX_PREFIX "o"
#define SCNuMAX     __PRIMAX_PREFIX "u"
#define SCNxMAX     __PRIMAX_PREFIX "x"
#define SCNXMAX     __PRIMAX_PREFIX "X"

#define SCNdPTR     __PRIPTR_PREFIX "d"
#define SCNiPTR     __PRIPTR_PREFIX "i"
#define SCNoPTR     __PRIPTR_PREFIX "o"
#define SCNuPTR     __PRIPTR_PREFIX "u"
#define SCNxPTR     __PRIPTR_PREFIX "x"
#define SCNXPTR     __PRIPTR_PREFIX "X"

__BEGIN_DECLS

/**
 * imaxdiv_t - Structure type that is the type of the value returned by the
 * imaxdiv() function.
 */
typedef struct {
	intmax_t quot;
	intmax_t rem;
} imaxdiv_t;

/* The following shall be declared as functions and may also be defined as
 * macros. Function prototypes shall be provided.
 */

/**
 * The imaxabs() function shall compute the absolute value of an integer j. If
 * the result cannot be represented, the behavior is undefined.
 *
 * @param j
 * @return The imaxabs() function shall return the absolute value.
 */
extern intmax_t imaxabs(intmax_t j);

/**
 * The imaxdiv() function shall compute numer / denom and numer % denom in a
 * single operation.
 *
 * @param numer
 * @param denom
 * @return The imaxdiv() function shall return a structure of type imaxdiv_t,
 *  comprising both the quotient and the remainder. The structure shall contain
 *  (in either order) the members quot (the quotient) and rem (the remainder),
 *  each of which has type intmax_t.
 *  If either part of the result cannot be represented, the behavior is
 *  undefined.
 */
extern imaxdiv_t imaxdiv(intmax_t numer, intmax_t denom);

/**
 * These functions shall be equivalent to the strtol(), strtoll(), strtoul(),
 * and strtoull() functions, except that the initial portion of the string shall
 * be converted to intmax_t and uintmax_t representation, respectively.
 *
 * @param nptr
 * @param endptr
 * @param base
 * @return These functions shall return the converted value, if any.
 *   If no conversion could be performed, zero shall be returned.
 *   If the correct value is outside the range of representable values,
 *   {INTMAX_MAX}, {INTMAX_MIN}, or {UINTMAX_MAX} shall be returned (according
 *   to the return type and sign of the value, if any), and errno shall be set
 *   to [ERANGE].
 */
extern intmax_t strtoimax(const char * /*restrict*/ nptr,
    char ** /*restrict*/ endptr, int base);

extern uintmax_t strtoumax(const char * /*restrict*/ nptr,
    char ** /*restrict*/ endptr, int base);

/**
 * The function shall be equivalent to the wcstol(), wcstoll(), wcstoul(),
 * and wcstoull() functions, respectively, except that the initial portion of
 * the wide string shall be converted to intmax_t and uintmax_t representation,
 * respectively.
 *
 * @param nptr
 * @param endptr
 * @param base
 * @return The function shall return the converted value, if any.
 *   If no conversion could be performed, zero shall be returned. If the correct
 *   value is outside the range of representable values, {INTMAX_MAX},
 *   {INTMAX_MIN}, or {UINTMAX_MAX} shall be returned (according to the return
 *   type and sign of the value, if any), and errno shall be set to [ERANGE].
 */
extern intmax_t wcstoimax(const wchar_t * /*restrict*/ nptr,
    wchar_t ** /*restrict*/ endptr, int base);

/**
 * The function shall be equivalent to the wcstol(), wcstoll(), wcstoul(),
 * and wcstoull() functions, respectively, except that the initial portion of
 * the wide string shall be converted to intmax_t and uintmax_t representation,
 * respectively.
 *
 * @param nptr
 * @param endptr
 * @param base
 * @return The function shall return the converted value, if any.
 *   If no conversion could be performed, zero shall be returned. If the correct
 *   value is outside the range of representable values, {INTMAX_MAX},
 *   {INTMAX_MIN}, or {UINTMAX_MAX} shall be returned (according to the return
 *   type and sign of the value, if any), and errno shall be set to [ERANGE].
 */
extern uintmax_t wcstoumax(const wchar_t * /*restrict*/ nptr,
    wchar_t ** /*restrict*/ endptr, int base);

__END_DECLS

//TODO this is C99 standard. It's required for mruby
#if 0
#ifndef NAN
#define NAN __builtin_nan("")
#endif
#ifndef INFINITY
#define INFINITY __builtin_inf()
#endif
#endif

#endif /* COMPAT_LIBC_INTTYPES_H_ */
