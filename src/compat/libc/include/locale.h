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

__BEGIN_DECLS

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

extern char *setlocale(int category, const char *locale);
extern struct lconv *localeconv(void);

__END_DECLS

#endif /* COMPAT_LIBC_LOCALE_H_ */
