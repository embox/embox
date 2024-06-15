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

#define __PRI_PREFIX_INT8    __PRI_PREFIX(__INT8_TYPE__)
#define __PRI_PREFIX_INT16   __PRI_PREFIX(__INT16_TYPE__)
#define __PRI_PREFIX_INT32   __PRI_PREFIX(__INT32_TYPE__)
#define __PRI_PREFIX_INT64   __PRI_PREFIX(__INT64_TYPE__)

#define __PRI_PREFIX_LEAST8  __PRI_PREFIX(__INT_LEAST8_TYPE__)
#define __PRI_PREFIX_LEAST16 __PRI_PREFIX(__INT_LEAST16_TYPE__)
#define __PRI_PREFIX_LEAST32 __PRI_PREFIX(__INT_LEAST32_TYPE__)
#define __PRI_PREFIX_LEAST64 __PRI_PREFIX(__INT_LEAST64_TYPE__)

#define __PRI_PREFIX_FAST8   __PRI_PREFIX(__INT_FAST8_TYPE__)
#define __PRI_PREFIX_FAST16  __PRI_PREFIX(__INT_FAST16_TYPE__)
#define __PRI_PREFIX_FAST32  __PRI_PREFIX(__INT_FAST32_TYPE__)
#define __PRI_PREFIX_FAST64  __PRI_PREFIX(__INT_FAST64_TYPE__)

#define __PRI_PREFIX_MAX     __PRI_PREFIX(__INTMAX_TYPE__)
#define __PRI_PREFIX_PTR     __PRI_PREFIX(__INTPTR_TYPE__)

#define PRId8                __PRI_PREFIX_INT8 "d"
#define PRIi8                __PRI_PREFIX_INT8 "i"
#define PRIo8                __PRI_PREFIX_INT8 "o"
#define PRIu8                __PRI_PREFIX_INT8 "u"
#define PRIx8                __PRI_PREFIX_INT8 "x"
#define PRIX8                __PRI_PREFIX_INT8 "X"

#define PRId16               __PRI_PREFIX_INT16 "d"
#define PRIi16               __PRI_PREFIX_INT16 "i"
#define PRIo16               __PRI_PREFIX_INT16 "o"
#define PRIu16               __PRI_PREFIX_INT16 "u"
#define PRIx16               __PRI_PREFIX_INT16 "x"
#define PRIX16               __PRI_PREFIX_INT16 "X"

#define PRId32               __PRI_PREFIX_INT32 "d"
#define PRIi32               __PRI_PREFIX_INT32 "i"
#define PRIo32               __PRI_PREFIX_INT32 "o"
#define PRIu32               __PRI_PREFIX_INT32 "u"
#define PRIx32               __PRI_PREFIX_INT32 "x"
#define PRIX32               __PRI_PREFIX_INT32 "X"

#define PRId64               __PRI_PREFIX_INT64 "d"
#define PRIi64               __PRI_PREFIX_INT64 "i"
#define PRIo64               __PRI_PREFIX_INT64 "o"
#define PRIu64               __PRI_PREFIX_INT64 "u"
#define PRIx64               __PRI_PREFIX_INT64 "x"
#define PRIX64               __PRI_PREFIX_INT64 "X"

#define PRIdLEAST8           __PRI_PREFIX_LEAST8 "d"
#define PRIiLEAST8           __PRI_PREFIX_LEAST8 "i"
#define PRIoLEAST8           __PRI_PREFIX_LEAST8 "o"
#define PRIuLEAST8           __PRI_PREFIX_LEAST8 "u"
#define PRIxLEAST8           __PRI_PREFIX_LEAST8 "x"
#define PRIXLEAST8           __PRI_PREFIX_LEAST8 "X"

#define PRIdLEAST16          __PRI_PREFIX_LEAST16 "d"
#define PRIiLEAST16          __PRI_PREFIX_LEAST16 "i"
#define PRIoLEAST16          __PRI_PREFIX_LEAST16 "o"
#define PRIuLEAST16          __PRI_PREFIX_LEAST16 "u"
#define PRIxLEAST16          __PRI_PREFIX_LEAST16 "x"
#define PRIXLEAST16          __PRI_PREFIX_LEAST16 "X"

#define PRIdLEAST32          __PRI_PREFIX_LEAST32 "d"
#define PRIiLEAST32          __PRI_PREFIX_LEAST32 "i"
#define PRIoLEAST32          __PRI_PREFIX_LEAST32 "o"
#define PRIuLEAST32          __PRI_PREFIX_LEAST32 "u"
#define PRIxLEAST32          __PRI_PREFIX_LEAST32 "x"
#define PRIXLEAST32          __PRI_PREFIX_LEAST32 "X"

#define PRIdLEAST64          __PRI_PREFIX_LEAST64 "d"
#define PRIiLEAST64          __PRI_PREFIX_LEAST64 "i"
#define PRIoLEAST64          __PRI_PREFIX_LEAST64 "o"
#define PRIuLEAST64          __PRI_PREFIX_LEAST64 "u"
#define PRIxLEAST64          __PRI_PREFIX_LEAST64 "x"
#define PRIXLEAST64          __PRI_PREFIX_LEAST64 "X"

#define PRIdFAST8            __PRI_PREFIX_FAST8 "d"
#define PRIiFAST8            __PRI_PREFIX_FAST8 "i"
#define PRIoFAST8            __PRI_PREFIX_FAST8 "o"
#define PRIuFAST8            __PRI_PREFIX_FAST8 "u"
#define PRIxFAST8            __PRI_PREFIX_FAST8 "x"
#define PRIXFAST8            __PRI_PREFIX_FAST8 "X"

#define PRIdFAST16           __PRI_PREFIX_FAST16 "d"
#define PRIiFAST16           __PRI_PREFIX_FAST16 "i"
#define PRIoFAST16           __PRI_PREFIX_FAST16 "o"
#define PRIuFAST16           __PRI_PREFIX_FAST16 "u"
#define PRIxFAST16           __PRI_PREFIX_FAST16 "x"
#define PRIXFAST16           __PRI_PREFIX_FAST16 "X"

#define PRIdFAST32           __PRI_PREFIX_FAST32 "d"
#define PRIiFAST32           __PRI_PREFIX_FAST32 "i"
#define PRIoFAST32           __PRI_PREFIX_FAST32 "o"
#define PRIuFAST32           __PRI_PREFIX_FAST32 "u"
#define PRIxFAST32           __PRI_PREFIX_FAST32 "x"
#define PRIXFAST32           __PRI_PREFIX_FAST32 "X"

#define PRIdFAST64           __PRI_PREFIX_FAST64 "d"
#define PRIiFAST64           __PRI_PREFIX_FAST64 "i"
#define PRIoFAST64           __PRI_PREFIX_FAST64 "o"
#define PRIuFAST64           __PRI_PREFIX_FAST64 "u"
#define PRIxFAST64           __PRI_PREFIX_FAST64 "x"
#define PRIXFAST64           __PRI_PREFIX_FAST64 "X"

#define PRIdMAX              __PRI_PREFIX_MAX "d"
#define PRIiMAX              __PRI_PREFIX_MAX "i"
#define PRIoMAX              __PRI_PREFIX_MAX "o"
#define PRIuMAX              __PRI_PREFIX_MAX "u"
#define PRIxMAX              __PRI_PREFIX_MAX "x"
#define PRIXMAX              __PRI_PREFIX_MAX "X"

#define PRIdPTR              __PRI_PREFIX_PTR "d"
#define PRIiPTR              __PRI_PREFIX_PTR "i"
#define PRIoPTR              __PRI_PREFIX_PTR "o"
#define PRIuPTR              __PRI_PREFIX_PTR "u"
#define PRIxPTR              __PRI_PREFIX_PTR "x"
#define PRIXPTR              __PRI_PREFIX_PTR "X"

#define SCNd8                __PRI_PREFIX_INT8 "d"
#define SCNi8                __PRI_PREFIX_INT8 "i"
#define SCNo8                __PRI_PREFIX_INT8 "o"
#define SCNu8                __PRI_PREFIX_INT8 "u"
#define SCNx8                __PRI_PREFIX_INT8 "x"
#define SCNX8                __PRI_PREFIX_INT8 "X"

#define SCNd16               __PRI_PREFIX_INT16 "d"
#define SCNi16               __PRI_PREFIX_INT16 "i"
#define SCNo16               __PRI_PREFIX_INT16 "o"
#define SCNu16               __PRI_PREFIX_INT16 "u"
#define SCNx16               __PRI_PREFIX_INT16 "x"
#define SCNX16               __PRI_PREFIX_INT16 "X"

#define SCNd32               __PRI_PREFIX_INT32 "d"
#define SCNi32               __PRI_PREFIX_INT32 "i"
#define SCNo32               __PRI_PREFIX_INT32 "o"
#define SCNu32               __PRI_PREFIX_INT32 "u"
#define SCNx32               __PRI_PREFIX_INT32 "x"
#define SCNX32               __PRI_PREFIX_INT32 "X"

#define SCNd64               __PRI_PREFIX_INT64 "d"
#define SCNi64               __PRI_PREFIX_INT64 "i"
#define SCNo64               __PRI_PREFIX_INT64 "o"
#define SCNu64               __PRI_PREFIX_INT64 "u"
#define SCNx64               __PRI_PREFIX_INT64 "x"
#define SCNX64               __PRI_PREFIX_INT64 "X"

#define SCNdLEAST8           __PRI_PREFIX_LEAST8 "d"
#define SCNiLEAST8           __PRI_PREFIX_LEAST8 "i"
#define SCNoLEAST8           __PRI_PREFIX_LEAST8 "o"
#define SCNuLEAST8           __PRI_PREFIX_LEAST8 "u"
#define SCNxLEAST8           __PRI_PREFIX_LEAST8 "x"
#define SCNXLEAST8           __PRI_PREFIX_LEAST8 "X"

#define SCNdLEAST16          __PRI_PREFIX_LEAST16 "d"
#define SCNiLEAST16          __PRI_PREFIX_LEAST16 "i"
#define SCNoLEAST16          __PRI_PREFIX_LEAST16 "o"
#define SCNuLEAST16          __PRI_PREFIX_LEAST16 "u"
#define SCNxLEAST16          __PRI_PREFIX_LEAST16 "x"
#define SCNXLEAST16          __PRI_PREFIX_LEAST16 "X"

#define SCNdLEAST32          __PRI_PREFIX_LEAST32 "d"
#define SCNiLEAST32          __PRI_PREFIX_LEAST32 "i"
#define SCNoLEAST32          __PRI_PREFIX_LEAST32 "o"
#define SCNuLEAST32          __PRI_PREFIX_LEAST32 "u"
#define SCNxLEAST32          __PRI_PREFIX_LEAST32 "x"
#define SCNXLEAST32          __PRI_PREFIX_LEAST32 "X"

#define SCNdLEAST64          __PRI_PREFIX_LEAST64 "d"
#define SCNiLEAST64          __PRI_PREFIX_LEAST64 "i"
#define SCNoLEAST64          __PRI_PREFIX_LEAST64 "o"
#define SCNuLEAST64          __PRI_PREFIX_LEAST64 "u"
#define SCNxLEAST64          __PRI_PREFIX_LEAST64 "x"
#define SCNXLEAST64          __PRI_PREFIX_LEAST64 "X"

#define SCNdFAST8            __PRI_PREFIX_FAST8 "d"
#define SCNiFAST8            __PRI_PREFIX_FAST8 "i"
#define SCNoFAST8            __PRI_PREFIX_FAST8 "o"
#define SCNuFAST8            __PRI_PREFIX_FAST8 "u"
#define SCNxFAST8            __PRI_PREFIX_FAST8 "x"
#define SCNXFAST8            __PRI_PREFIX_FAST8 "X"

#define SCNdFAST16           __PRI_PREFIX_FAST16 "d"
#define SCNiFAST16           __PRI_PREFIX_FAST16 "i"
#define SCNoFAST16           __PRI_PREFIX_FAST16 "o"
#define SCNuFAST16           __PRI_PREFIX_FAST16 "u"
#define SCNxFAST16           __PRI_PREFIX_FAST16 "x"
#define SCNXFAST16           __PRI_PREFIX_FAST16 "X"

#define SCNdFAST32           __PRI_PREFIX_FAST32 "d"
#define SCNiFAST32           __PRI_PREFIX_FAST32 "i"
#define SCNoFAST32           __PRI_PREFIX_FAST32 "o"
#define SCNuFAST32           __PRI_PREFIX_FAST32 "u"
#define SCNxFAST32           __PRI_PREFIX_FAST32 "x"
#define SCNXFAST32           __PRI_PREFIX_FAST32 "X"

#define SCNdFAST64           __PRI_PREFIX_FAST64 "d"
#define SCNiFAST64           __PRI_PREFIX_FAST64 "i"
#define SCNoFAST64           __PRI_PREFIX_FAST64 "o"
#define SCNuFAST64           __PRI_PREFIX_FAST64 "u"
#define SCNxFAST64           __PRI_PREFIX_FAST64 "x"
#define SCNXFAST64           __PRI_PREFIX_FAST64 "X"

#define SCNdMAX              __PRI_PREFIX_MAX "d"
#define SCNiMAX              __PRI_PREFIX_MAX "i"
#define SCNoMAX              __PRI_PREFIX_MAX "o"
#define SCNuMAX              __PRI_PREFIX_MAX "u"
#define SCNxMAX              __PRI_PREFIX_MAX "x"
#define SCNXMAX              __PRI_PREFIX_MAX "X"

#define SCNdPTR              __PRI_PREFIX_PTR "d"
#define SCNiPTR              __PRI_PREFIX_PTR "i"
#define SCNoPTR              __PRI_PREFIX_PTR "o"
#define SCNuPTR              __PRI_PREFIX_PTR "u"
#define SCNxPTR              __PRI_PREFIX_PTR "x"
#define SCNXPTR              __PRI_PREFIX_PTR "X"

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
