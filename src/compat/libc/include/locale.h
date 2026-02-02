/**
 * @file
 * @brief
 *
 * @date 11.01.13
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef COMPAT_LIBC_LOCALE_H_
#define COMPAT_LIBC_LOCALE_H_

#include <stddef.h>
#include <sys/cdefs.h>

/** 
 * These are the possibilities for the first argument to setlocale.
 * The code assumes that the lowest LC_* symbol has the value zero.
 */
#define LC_CTYPE          0
#define LC_NUMERIC        1
#define LC_TIME           2
#define LC_COLLATE        3
#define LC_MONETARY       4
#define LC_MESSAGES       5
#define LC_ALL            6
#define LC_PAPER          7
#define LC_NAME           8
#define LC_ADDRESS        9
#define LC_TELEPHONE      10
#define LC_MEASUREMENT    11
#define LC_IDENTIFICATION 12

#define LC_CTYPE_MASK          (1 << LC_CTYPE)
#define LC_NUMERIC_MASK        (1 << LC_NUMERIC)
#define LC_TIME_MASK           (1 << LC_TIME)
#define LC_COLLATE_MASK        (1 << LC_COLLATE)
#define LC_MONETARY_MASK       (1 << LC_MONETARY)
#define LC_MESSAGES_MASK       (1 << LC_MESSAGES)
#define LC_PAPER_MASK          (1 << LC_PAPER)
#define LC_NAME_MASK           (1 << LC_NAME)
#define LC_ADDRESS_MASK        (1 << LC_ADDRESS)
#define LC_TELEPHONE_MASK      (1 << LC_TELEPHONE)
#define LC_MEASUREMENT_MASK    (1 << LC_MEASUREMENT)
#define LC_IDENTIFICATION_MASK (1 << LC_IDENTIFICATION)

#define LC_ALL_MASK                                                          \
	(LC_CTYPE_MASK | LC_NUMERIC_MASK | LC_TIME_MASK | LC_COLLATE_MASK        \
	    | LC_MONETARY_MASK | LC_MESSAGES_MASK | LC_PAPER_MASK | LC_NAME_MASK \
	    | LC_ADDRESS_MASK | LC_TELEPHONE_MASK | LC_MEASUREMENT_MASK          \
	    | LC_IDENTIFICATION_MASK)

#define LC_GLOBAL_LOCALE ((locale_t)-1)

struct lconv {
	char *decimal_point;
	char *thousands_sep;
	char *grouping;
	char *int_curr_symbol;
	char *currency_symbol;
	char *mon_decimal_point;
	char *mon_thousands_sep;
	char *mon_grouping;
	char *positive_sign;
	char *negative_sign;
	char int_frac_digits;
	char frac_digits;
	char p_cs_precedes;
	char p_sep_by_space;
	char n_cs_precedes;
	char n_sep_by_space;
	char p_sign_posn;
	char n_sign_posn;
	char int_n_cs_precedes;
	char int_n_sep_by_space;
	char int_n_sign_posn;
	char int_p_cs_precedes;
	char int_p_sep_by_space;
	char int_p_sign_posn;
};

typedef void *locale_t;

__BEGIN_DECLS

extern char *setlocale(int category, const char *locale);
extern struct lconv *localeconv(void);

extern locale_t newlocale(int category_mask, const char *locale, locale_t base);
extern locale_t duplocale(locale_t locobj);
extern void freelocale(locale_t locobj);
extern locale_t uselocale(locale_t newloc);

__END_DECLS

#endif /* COMPAT_LIBC_LOCALE_H_ */
