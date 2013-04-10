/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.02.2013
 */

#ifndef COMPAT_POSIX_SHADOW_H_
#define COMPAT_POSIX_SHADOW_H_

#include <stdio.h>

struct spwd {
	char               *sp_namp;
	char               *sp_pwdp;
	long int           sp_lstchg;
	long int           sp_min;
	long int           sp_max;
	long int           sp_warn;
	long int           sp_inact;
	long int           sp_expire;
	unsigned long int  sp_flag;
};

#if 0
extern struct spwd *getspent(void);

extern void setspent(void);

extern int lckpwdf(void);

extern int ulckpwdf(void);
#endif

extern struct spwd *fgetspent(FILE *file);

/**
 * @brief This will search entire DB for matching entry,
 * which differs from @a getpsnam, that will search from current
 * position
 *
 * @param name Name to searc
 *
 * @return pointer to @a spwd
 * @return NULL
 */
extern struct spwd *getspnam_f(const char *name);

#endif /* COMPAT_POSIX_SHADOW_H_ */
