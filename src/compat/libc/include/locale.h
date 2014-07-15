/*
 * @file
 * @brief
 *
 * @date 11.01.13
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef LOCALE_H_
#define LOCALE_H_

#include <defines/null.h>
#include <defines/size_t.h>

#include <sys/cdefs.h>
__BEGIN_DECLS

/* These are the possibilities for the first argument to setlocale.
   The code assumes that the lowest LC_* symbol has the value zero.  */
#define LC_CTYPE          0
#define LC_NUMERIC        1
#define LC_TIME           2
#define LC_COLLATE        3
#define LC_MONETARY       4
#define LC_MESSAGES       5
#define	LC_ALL		  6
#define LC_PAPER	  7
#define LC_NAME		  8
#define LC_ADDRESS	  9
#define LC_TELEPHONE	  10
#define LC_MEASUREMENT	  11
#define LC_IDENTIFICATION 12

static inline char * setlocale(int category, const char *locale) {
	(void)category; (void)locale;
	return NULL;
}

__END_DECLS

#endif /* LOCALE_H_ */
