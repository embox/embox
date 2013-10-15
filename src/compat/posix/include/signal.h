/**
 * @file
 *
 * @brief
 *
 * @date 15.05.2012
 * @author Anton Bondarev
 */

#ifndef SIGNAL_H_
#define SIGNAL_H_

#include <sys/types.h>
// #include <kernel/task/signal.h>

#define SIG_DFL ((sighandler_t) 0x1)
#define SIG_IGN ((sighandler_t) 0x3)
#define SIG_ERR ((sighandler_t) 0x5)

/* Signals.  */
#define SIGHUP      1       /* (POSIX)    Hangup.  */
#define SIGINT      2       /* (ANSI)     Interrupt.  */
#define SIGQUIT     3       /* (POSIX)    Quit.  */
#define SIGILL      4       /* (ANSI)     Illegal instruction.  */
#define SIGTRAP     5       /* (POSIX)    Trace trap.  */
#define SIGABRT     6       /* (ANSI)     Abort.  */
#define SIGIOT      6       /* (4.2 BSD)  IOT trap.  */
#define SIGBUS      7       /* (4.2 BSD)  BUS error.  */
#define SIGFPE      8       /* (ANSI)     Floating-point exception.  */
#define SIGKILL     9       /* (POSIX)    Kill, unblockable.  */
#define SIGUSR1     10      /* (POSIX)    User-defined signal 1.  */
#define SIGSEGV     11      /* (ANSI)     Segmentation violation.  */
#define SIGUSR2     12      /* (POSIX)    User-defined signal 2.  */
#define SIGPIPE     13      /* (POSIX)    Broken pipe.  */
#define SIGALRM     14      /* (POSIX)    Alarm clock.  */
#define SIGTERM     15      /* (ANSI)     Termination.  */
#define SIGSTKFLT   16      /*            Stack fault.  */
#define SIGCLD      SIGCHLD /* (System V) Same as SIGCHLD.  */
#define SIGCHLD     17      /* (POSIX)    Child status has changed.  */
#define SIGCONT     18      /* (POSIX)    Continue.  */
#define SIGSTOP     19      /* (POSIX)    Stop, unblockable.  */
#define SIGTSTP     20      /* (POSIX)    Keyboard stop.  */
#define SIGTTIN     21      /* (POSIX)    Background read from tty.  */
#define SIGTTOU     22      /* (POSIX)    Background write to tty.  */
#define SIGURG      23      /* (4.2 BSD)  Urgent condition on socket.  */
#define SIGXCPU     24      /* (4.2 BSD)  CPU limit exceeded.  */
#define SIGXFSZ     25      /* (4.2 BSD)  File size limit exceeded.  */
#define SIGVTALRM   26      /* (4.2 BSD)  Virtual alarm clock.  */
#define SIGPROF     27      /* (4.2 BSD)  Profiling alarm clock.  */
#define SIGWINCH    28      /* (4.3 BSD, Sun) Window size change.  */
#define SIGPOLL     SIGIO   /* (System V) Pollable event occurred.  */
#define SIGIO       29      /* (4.2 BSD)  I/O now possible.  */
#define SIGPWR      30      /* (System V) Power failure restart.  */
#define SIGSYS      31      /*            Bad system call.  */
#define SIGUNUSED   31

#define SIGRTMIN    TASK_SIGRTMIN
#define SIGRTMAX    TASK_SIGRTMAX

typedef int sigset_t;

union sigval {
	int   sival_int;
	void *sival_ptr;
};

typedef struct {
	int           si_signo;
	int           si_code;
	union sigval  si_value;
	int           si_errno;
	pid_t         si_pid;
	uid_t         si_uid;
	void         *si_addr;
	int           si_status;
	int           si_band;
} siginfo_t;

struct sigaction {
	/* The storage occupied by sa_handler and sa_sigaction may overlap,
	 * and a conforming application shall not use both simultaneously.  */
	union {
		void (*sa_handler)(int);
		void (*sa_sigaction)(int, siginfo_t *, void *);
	} /* unnamed */;
	sigset_t   sa_mask;
	int        sa_flags;
};

/* Non-standard GNU extension. */
typedef void (*sighandler_t)(int);

extern int kill(int tid, int sig);
extern sighandler_t signal(int sig, sighandler_t fn);
extern int sigqueue(int tid, int sig, const union sigval value);
static inline int raise(int sig) {
	return 0;
}

//static inline int sigaction(int sig, const struct sigaction *act,
		//struct sigaction *oact) { return -1; }
//static inline int sigfillset(sigset_t *set)

#endif /* SIGNAL_H_ */
