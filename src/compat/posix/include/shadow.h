/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.02.2013
 */

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
extern struct spwd *getspent();
extern struct swpd *getspname(char *name);

extern void setspent();

extern int lckpwdf();

extern int ulckpwdf();
#endif

extern struct spwd *fgetspent(FILE *file);
