/**
 * @file
 *
 * @date Aug 22, 2013
 * @author: Anton Bondarev
 */

#ifndef INTTYPES_H_
#define INTTYPES_H_

/* The <inttypes.h> header shall include the <stdint.h> header. */
#include <stdint.h>
#include <stddef.h>

/**
 * imaxdiv_t - Structure type that is the type of the value returned by the
 * imaxdiv() function.
 */
typedef struct {
	intmax_t quot;
	intmax_t rem;
} imaxdiv_t;

#include <sys/cdefs.h>
__BEGIN_DECLS

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
extern intmax_t strtoimax(const char */*restrict*/ nptr, char **/*restrict*/ endptr,
		int base);

extern uintmax_t strtoumax(const char */*restrict*/ nptr, char **/*restrict*/ endptr,
		int base);

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
extern intmax_t wcstoimax(const wchar_t */*restrict*/ nptr,
		wchar_t **/*restrict*/ endptr, int base);

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
extern uintmax_t wcstoumax(const wchar_t */*restrict*/ nptr,
		wchar_t **/*restrict*/ endptr, int base);

__END_DECLS

/* The fprintf() macros for signed integers are:
 * PRIdN PRIdLEASTN PRIdFASTN PRIdMAX PRIdPTR PRIiN PRIiLEASTN PRIiFASTN
 * PRIiMAX PRIiPTR*/
#define    PRId8       "d"
#define    PRIdLEAST8  "d"
#define    PRIdFAST8   "d"
#define    PRIi8       "i"
#define    PRIiLEAST8  "i"
#define    PRIiFAST8   "i"

#define    PRId16      "d"
#define    PRIdLEAST16 "d"
#define    PRIdFAST16  "d"
#define    PRIi16      "i"
#define    PRIiLEAST16 "i"
#define    PRIiFAST16  "i"

#if defined(__mips__) || defined(__sparc__) || defined(__microblaze__) || defined(__PPC__)
#define    PRId32      "ld"
#define    PRIdLEAST32 "ld"
#define    PRIdFAST32  "ld"
#define    PRIi32      "li"
#define    PRIiLEAST32 "li"
#define    PRIiFAST32  "li"
#elif defined (i386) || defined (__e2k__) || defined(__arm__) || defined(__aarch64__)
#define    PRId32      "d"
#define    PRIdLEAST32 "d"
#define    PRIdFAST32  "d"
#define    PRIi32      "i"
#define    PRIiLEAST32 "i"
#define    PRIiFAST32  "i"
#else
#define    PRId32      "ld"
#define    PRIdLEAST32 "ld"
#define    PRIdFAST32  "ld"
#define    PRIi32      "li"
#define    PRIiLEAST32 "li"
#define    PRIiFAST32  "li"
#endif

#define    PRIdPTR     PRId32
#define    PRIiPTR     PRIi32

#define    PRIdMAX     PRId64
#define    PRIiMAX     PRIi32

/* The fprintf() macros for unsigned integers are:
 * PRIoN PRIoLEASTN PRIoFASTN PRIoMAX PRIoPTR PRIuN PRIuLEASTN PRIuFASTN
 * PRIuMAX PRIuPTR PRIxN PRIxLEASTN PRIxFASTN PRIxMAX PRIxPTR PRIXN PRIXLEASTN
 * PRIXFASTN PRIXMAX PRIXPTR
 */
#define    PRIo8       "o"
#define    PRIoLEAST8  "o"
#define    PRIoFAST8   "o"
#define    PRIu8       "u"
#define    PRIuLEAST8  "u"
#define    PRIuFAST8   "u"
#define    PRIx8       "x"
#define    PRIxLEAST8  "x"
#define    PRIxFAST8   "x"
#define    PRIX8       "X"
#define    PRIXLEAST8  "X"
#define    PRIXFAST8   "X"

#define    PRIo16      "o"
#define    PRIoLEAST16 "o"
#define    PRIoFAST16  "o"
#define    PRIu16      "u"
#define    PRIuLEAST16 "u"
#define    PRIuFAST16  "u"
#define    PRIx16      "x"
#define    PRIxLEAST16 "x"
#define    PRIxFAST16  "x"
#define    PRIX16      "X"
#define    PRIXLEAST16 "X"
#define    PRIXFAST16  "X"


#if defined(__riscv) || defined (__e2k__) || (defined(__mips__) && !defined(__gnu_linux__)) || defined(__sparc__) || defined(__microblaze__) || defined(__PPC__)
#define    PRIo32      "lo"
#define    PRIoLEAST32 "lo"
#define    PRIoFAST32  "lo"
#define    PRIu32      "lu"
#define    PRIuLEAST32 "lu"
#define    PRIuFAST32  "lu"
#define    PRIx32      "lx"
#define    PRIxLEAST32 "lx"
#define    PRIxFAST32  "lx"
#define    PRIX32      "lX"
#define    PRIXLEAST32 "lX"
#define    PRIXFAST32  "lX"

#define    PRIu64      "llu"
#define    PRIi64      "lli"
#define    PRIx64      "llx"
#define    PRIX64      "llX"
#define    PRId64      "lld"

#elif defined(__arm__) || defined (i386) || (defined(__mips__) && defined(__gnu_linux__)) || defined(__aarch64__)

#define    PRIo32      "o"
#define    PRIoLEAST32 "o"
#define    PRIoFAST32  "o"
#define    PRIu32      "u"
#define    PRIuLEAST32 "u"
#define    PRIuFAST32  "u"
#define    PRIx32      "x"
#define    PRIxLEAST32 "x"
#define    PRIxFAST32  "x"
#define    PRIX32      "X"
#define    PRIXLEAST32 "X"
#define    PRIXFAST32  "X"

#define    PRIu64      "lu"
#define    PRIi64      "li"
#define    PRIx64      "lx"
#define    PRIX64      "lX"
#define    PRId64      "ld"

#elif defined(__aarch64__)
#define    PRIo32      "o"
#define    PRIoLEAST32 "o"
#define    PRIoFAST32  "o"
#define    PRIu32      "u"
#define    PRIuLEAST32 "u"
#define    PRIuFAST32  "u"
#define    PRIx32      "x"
#define    PRIxLEAST32 "x"
#define    PRIxFAST32  "x"
#define    PRIX32      "X"
#define    PRIXLEAST32 "X"
#define    PRIXFAST32  "X"

#define    PRIu64      "u"
#define    PRIi64      "i"
#define    PRIx64      "x"
#define    PRIX64      "X"
#define    PRId64      "d"
#else

#define    PRIo32      "o"
#define    PRIoLEAST32 "o"
#define    PRIoFAST32  "o"
#define    PRIu32      "u"
#define    PRIuLEAST32 "u"
#define    PRIuFAST32  "u"
#define    PRIx32      "x"
#define    PRIxLEAST32 "x"
#define    PRIxFAST32  "x"
#define    PRIX32      "X"
#define    PRIXLEAST32 "X"
#define    PRIXFAST32  "X"

#define    PRIu64      "lu"
#define    PRIi64      "li"
#define    PRIx64      "lx"
#define    PRIX64      "lX"
#define    PRId64      "ld"

#endif

#if defined (__e2k__)

#define    PRIoPTR     PRIo32
#define    PRIuPTR     PRIu32
#define    PRIxPTR     "llx"
#define    PRIXPTR     "llX"

#elif defined (i386) || defined(__arm__) || defined(__mips__) || defined(__sparc__) || defined(__microblaze__) || defined(__PPC__)

#define    PRIoPTR     PRIo32
#define    PRIuPTR     PRIu32
#define    PRIxPTR     "x"
#define    PRIXPTR     "X"

#elif defined(__aarch64__)

#define    PRIoPTR     PRIo64
#define    PRIuPTR     PRIu64
#define    PRIxPTR     "lx"
#define    PRIXPTR     "lX"

#else

#define    PRIoPTR     PRIo32
#define    PRIuPTR     PRIu32
#define    PRIxPTR     "x"
#define    PRIXPTR     "X"

#endif

#define    PRIoMAX     PRIo32
#define    PRIuMAX     PRIu64
#define    PRIxMAX     PRIx64
#define    PRIXMAX     PRIX64

/* The fscanf() macros for signed integers are:
 * SCNdN SCNdLEASTN SCNdFASTN SCNdMAX SCNdPTR SCNiN SCNiLEASTN SCNiFASTN
 * SCNiMAX SCNiPTR
 */
#define    SCNd16        "d"
#define    SCNdLEAST16   "d"
#define    SCNdFAST16    "d"
#define    SCNi16        "i"
#define    SCNiLEAST16   "i"
#define    SCNiFAST16    "i"


#define    SCNd32        "d"
#define    SCNdLEAST32   "d"
#define    SCNdFAST32    "d"
#define    SCNi32        "i"
#define    SCNiLEAST32   "i"
#define    SCNiFAST32    "i"

#define    SCNdPTR       SCNd32
#define    SCNiPTR       SCNi32

#define    SCNdMAX       SCNd32
#define    SCNiMAX       SCNi32

/* The fscanf() macros for unsigned integers are:
 * SCNoN SCNoLEASTN SCNoFASTN SCNoMAX SCNoPTR SCNuN SCNuLEASTN SCNuFASTN
 * SCNuMAX SCNuPTR SCNxN SCNxLEASTN SCNxFASTN SCNxMAX SCNxPTR
 */
#define    SCNo16        "o"
#define    SCNoLEAST16   "o"
#define    SCNoFAST16    "o"
#define    SCNu16        "u"
#define    SCNuLEAST16   "u"
#define    SCNuFAST16    "u"
#define    SCNx16        "x"
#define    SCNxLEAST16   "x"
#define    SCNxFAST16    "x"
#define    SCNo32        "o"
#define    SCNoLEAST32   "o"
#define    SCNoFAST32    "o"
#define    SCNu32        "u"
#define    SCNuLEAST32   "u"
#define    SCNuFAST32    "u"
#define    SCNx32        "x"
#define    SCNxLEAST32   "x"
#define    SCNxFAST32    "x"

#define    SCNoPTR       SCNo32
#define    SCNuPTR       SCNu32
#define    SCNxPTR       SCNx32

#define    SCNoMAX       SCNo32
#define    SCNuMAX       SCNu32
#define    SCNxMAX       SCNx32

//TODO this is C99 standard. It's required for mruby
#if 0
#ifndef NAN
#define 	NAN   __builtin_nan("")
#endif
#ifndef INFINITY
#define 	INFINITY   __builtin_inf()
#endif
#endif
#endif /* INTTYPES_H_ */
