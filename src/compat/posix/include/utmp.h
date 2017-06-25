/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.02.2013
 */

#ifndef COMPAT_POSIX_UTMP_H_
#define COMPAT_POSIX_UTMP_H_

#include <sys/time.h>
#include <stdint.h>
#include <sys/types.h>

#define EMPTY         0 /* Record does not contain valid info
		      (formerly known as UT_UNKNOWN on Linux) */
#define RUN_LVL       1 /* Change in system run-level (see
		      init(8)) */
#define BOOT_TIME     2 /* Time of system boot (in ut_tv) */
#define NEW_TIME      3 /* Time after system clock change
		      (in ut_tv) */
#define OLD_TIME      4 /* Time before system clock change
		      (in ut_tv) */
#define INIT_PROCESS  5 /* Process spawned by init(8) */
#define LOGIN_PROCESS 6 /* Session leader process for user login */
#define USER_PROCESS  7 /* Normal process */
#define DEAD_PROCESS  8 /* Terminated process */
#define ACCOUNTING    9 /* Not implemented */

#define UT_IDSIZE	  4
#define UT_LINESIZE      32
#define UT_NAMESIZE      32
#define UT_HOSTSIZE     256

struct exit_status {              /* Type for ut_exit, below */
	short int e_termination;      /* Process termination status */
	short int e_exit;             /* Process exit status */
};

struct utmp {
	short   ut_type;              /* Type of record */
	pid_t   ut_pid;               /* PID of login process */
	char    ut_line[UT_LINESIZE]; /* Device name of tty - "/dev/" */
	char    ut_id[UT_IDSIZE];     /* Terminal name suffix,
					or inittab(5) ID */
	char    ut_user[UT_NAMESIZE]; /* Username */
	char    ut_host[UT_HOSTSIZE]; /* Hostname for remote login, or
					kernel version for run-level
					messages */
	struct  exit_status ut_exit;  /* Exit status of a process
					marked as DEAD_PROCESS; not
					used by Linux init(8) */
	/* The ut_session and ut_tv fields must be the same size when
	  compiled 32- and 64-bit.  This allows data files and shared
	  memory to be shared between 32- and 64-bit applications. */
	long   ut_session;           /* Session ID */
	struct timeval ut_tv;        /* Time entry was made */

	int32_t ut_addr_v6[4];        /* Internet address of remote
					host; IPv4 address uses
					just ut_addr_v6[0] */
	char __unused[20];            /* Reserved for future use */
};

struct lastlog {
        int32_t ll_time;                /* When user logged in */
        char    ll_line[UT_LINESIZE];   /* Terminal line name */
        char    ll_host[UT_HOSTSIZE];   /* Host user came from */
};

#define ut_name ut_user
#ifndef _NO_UT_TIME
#define ut_time ut_tv.tv_sec
#endif
#define ut_xtime ut_tv.tv_sec
#define ut_addr ut_addr_v6[0]


extern struct utmp *getutent(void);
extern struct utmp *getutid(struct utmp *ut);
extern struct utmp *getutline(struct utmp *ut);

extern struct utmp *pututline(struct utmp *ut);
extern void setutent(void);
extern void endutent(void);

extern int utmpname(const char *file);

#endif /* COMPAT_POSIX_UTMP_H_ */

