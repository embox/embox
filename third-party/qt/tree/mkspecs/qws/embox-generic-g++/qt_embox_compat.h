
#ifdef configure
#undef configure
#endif

#if defined(__linux__)
#undef __linux__
#endif

#if defined(__linux)
#undef __linux
#endif

#if defined(__STDC__)
// Causes problems with fdlibm
#undef __STDC__
#endif

#define __UCLIBC__
#define QT_LINUXBASE

#define __MAKEDEPEND__

#define RAND_MAX 32767

#define HUGE_VAL 0




#include <sys/types.h> // for size_t
#define MAP_SHARED    0x00
#define MAP_PRIVATE   0x01
#define PROT_READ     0x10
#define PROT_WRITE    0x20
#define MAP_FAILED    (-1)
extern int    munmap(void *, size_t);
extern void  *mmap(void *, size_t, int, int, int, off_t);



// Stuff below moved here because of testlib

// because of printf
#include <stdio.h>
static inline char *getenv(const char *name)
  { printf(">>> getenv(%s)\n",name); return 0; }
static inline int fflush(FILE *x)
  { printf(">>> fflush(%d)\n",(int)x); return EOF; }

// this function has to be added to string.h
int strcoll(const char *s1, const char *s2);


// This is because of pid_t & uid_t
#include <sys/types.h>

// The definition is not precise, please revise
typedef struct {
	int si_signo;
	int si_code;
        //union sigval si_value;
	int si_errno;
	pid_t si_pid;
	uid_t si_uid;
	void *si_addr;
	int si_status;
	int si_band;
} siginfo_t;

typedef int sigset_t;

struct sigaction {
    void (*sa_handler)(int);
    void (*sa_sigaction)(int, siginfo_t *, void *);
    sigset_t sa_mask;
    int sa_flags;
    void (*sa_restorer)(void);
};

#define SA_RESETHAND 0
int sigaddset(sigset_t *set, int signum);
int sigismember(const sigset_t *set, int signum);

int nanosleep(const struct timespec *req, struct timespec *rem);

/*
#define SA_NOCLDSTOP 0
*/

int sigemptyset(sigset_t *set);

int sigaction(int signum, const struct sigaction *act,
	      struct sigaction *oldact);


