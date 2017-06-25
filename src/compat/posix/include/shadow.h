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
#include <sys/cdefs.h>

__BEGIN_DECLS

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

extern struct spwd *getspnam(char *name);

extern int putspent(struct spwd *p, FILE *fp);

/* non-standart extention */

extern struct spwd *spwd_find(const char *spwd_path, const char *name);
__END_DECLS

#endif /* COMPAT_POSIX_SHADOW_H_ */
