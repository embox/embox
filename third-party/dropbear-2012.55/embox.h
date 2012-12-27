/**
 * @file
 *
 * @brief
 * @date 23.11.2012
 * @author Alexander Kalmuk
 */


#define LOG_ERR 0 /*third-party/dropbear-2012.55/sshpty.c:213:*/
#define SIG_IGN 0 /*third-party/dropbear-2012.55/sshpty.c:274:*/
#define TIOCSWINSZ 0 /*third-party/dropbear-2012.55/sshpty.c:352:*/

struct winsize /* third-party/dropbear-2012.55/sshpty.c:346: */
{
  unsigned short ws_row;	/* rows, in characters */
  unsigned short ws_col;	/* columns, in characters */
  unsigned short ws_xpixel;	/* horizontal size, pixels */
  unsigned short ws_ypixel;	/* vertical size, pixels */
};

//--------------------------------------------------
#define LOG_NOTICE 0 /*third-party/dropbear-2012.55/svr-authpasswd.c:97:*/
//---------------------------------------------------

#define WNOHANG 0 /*third-party/dropbear-2012.55/svr-chansession.c:91:*/
#define SA_NOCLDSTOP 0 /*third-party/dropbear-2012.55/svr-chansession.c:139:*/
#define SA_NOCLDSTOP 0 /*third-party/dropbear-2012.55/svr-chansession.c:983:*/
//---------------------------------------------------

typedef struct {   /* third-party/dropbear-2012.55/svr-chansession.c:89: */
	int si_signo;
	int si_code;
	union sigval si_value;
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
//-------------------------------------------------------

extern int snprintf(char *str, size_t size, const char *format, ...);

static inline pid_t setsid(void) {
	return -1;
}
extern void _exit(int status);

static inline int chdir(const char *path) {
	return -1;
}
static inline struct hostent *gethostbyaddr(const void *addr,
                                     socklen_t len, int type) {
	return NULL;
}
static inline struct servent *getservbyname(const char *name, const char *proto) {
	return NULL;
}

static inline uid_t geteuid(void) {
	return -1;
}
static inline int tcgetattr(int fd, struct termios *termios_p) {
	return -1;
}
static inline int tcsetattr(int fd, int optional_actions,
        const struct termios *termios_p) {
	return -1;
}
static inline int chown(const char *path, uid_t owner, gid_t group) {
	return -1;
}

extern char *crypt(const char *key, const char *salt);

static inline struct tm *localtime(const time_t *timep) {
	return NULL;
}

#define WIFEXITED(s) 0
#define WEXITSTATUS(s) 0
#define WIFSIGNALED(s) 0
#define WTERMSIG(s) 0

static inline int sigemptyset(sigset_t *set) {
	return -1;
}

extern int gethostname(char *name, size_t len);
static inline int sigaction(int signum, const struct sigaction *act,
        struct sigaction *oldact) {
	return -1;
}
extern char **environ;
static inline uid_t getuid(void) {
	return 0; // for now: same as in embox_passwd
}

static inline int setuid(uid_t uid) {
	return -1;
}
static inline int setgid(gid_t gid) {
	return -1;
}
static inline int initgroups(const char *user, gid_t group) {
	return -1;
}
static inline int putenv(char *string) {
	return -1;
}
extern int vsnprintf(char *str, size_t size, const char *format, va_list ap);

#include <asm/termbits.h>

//------------------------------------------------------------


