/**
 * @file
 *
 * @brief
 * @date 23.11.2012
 * @author Alexander Kalmuk
 */



#define VINTR 0 /*third-party/dropbear-2012.55/termcodes.c:31:*/
#define VQUIT 0 /*third-party/dropbear-2012.55/termcodes.c:32:*/
#define VERASE 0 /*third-party/dropbear-2012.55/termcodes.c:33:*/
#define VKILL 0 /*third-party/dropbear-2012.55/termcodes.c:34:*/
#define VEOF 0 /*third-party/dropbear-2012.55/termcodes.c:35:*/
#define VEOL 0 /*third-party/dropbear-2012.55/termcodes.c:36:*/
#define VEOL2 0 /*third-party/dropbear-2012.55/termcodes.c:37:*/
#define VSTART 0 /*third-party/dropbear-2012.55/termcodes.c:38:*/
#define VSTOP 0 /*third-party/dropbear-2012.55/termcodes.c:39:*/
#define VSUSP 0 /*third-party/dropbear-2012.55/termcodes.c:40:*/
#define VWERASE 0 /*third-party/dropbear-2012.55/termcodes.c:54:*/
#define VLNEXT 0 /*third-party/dropbear-2012.55/termcodes.c:56:*/
#define IGNPAR 0 /*third-party/dropbear-2012.55/termcodes.c:90:*/
#define PARMRK 0 /*third-party/dropbear-2012.55/termcodes.c:91:*/
#define INPCK 0 /*third-party/dropbear-2012.55/termcodes.c:92:*/
#define ISTRIP 0 /*third-party/dropbear-2012.55/termcodes.c:93:*/
#define INLCR 0 /*third-party/dropbear-2012.55/termcodes.c:94:*/
#define IGNCR 0 /*third-party/dropbear-2012.55/termcodes.c:95:*/
#define ICRNL 0 /*third-party/dropbear-2012.55/termcodes.c:96:*/
#define IXON 0 /*third-party/dropbear-2012.55/termcodes.c:102:*/
#define IXANY 0 /*third-party/dropbear-2012.55/termcodes.c:103:*/
#define IXOFF 0 /*third-party/dropbear-2012.55/termcodes.c:104:*/
#define ISIG 0 /*third-party/dropbear-2012.55/termcodes.c:118:*/
#define ICANON 0 /*third-party/dropbear-2012.55/termcodes.c:119:*/
#define ECHO 0 /*third-party/dropbear-2012.55/termcodes.c:125:*/
#define ECHOE 0 /*third-party/dropbear-2012.55/termcodes.c:126:*/
#define ECHOK 0 /*third-party/dropbear-2012.55/termcodes.c:127:*/
#define ECHONL 0 /*third-party/dropbear-2012.55/termcodes.c:128:*/
#define NOFLSH 0 /*third-party/dropbear-2012.55/termcodes.c:129:*/
#define TOSTOP 0 /*third-party/dropbear-2012.55/termcodes.c:130:*/
#define IEXTEN 0 /*third-party/dropbear-2012.55/termcodes.c:131:*/
#define ECHOCTL 0 /*third-party/dropbear-2012.55/termcodes.c:132:*/
#define ECHOKE 0 /*third-party/dropbear-2012.55/termcodes.c:133:*/
#define OPOST 0 /*third-party/dropbear-2012.55/termcodes.c:146:*/
#define ONLCR 0 /*third-party/dropbear-2012.55/termcodes.c:152:*/
#define CS7 0 /*third-party/dropbear-2012.55/termcodes.c:182:*/
#define CS8 0 /*third-party/dropbear-2012.55/termcodes.c:183:*/
#define PARENB 0 /*third-party/dropbear-2012.55/termcodes.c:184:*/
#define PARODD 0 /*third-party/dropbear-2012.55/termcodes.c:185:*/
//---------------------------------------------------

#define LOG_ERR 0 /*third-party/dropbear-2012.55/sshpty.c:213:*/
#define TCSANOW 0 /*third-party/dropbear-2012.55/sshpty.c:228:*/
#define LOG_ERR 0 /*third-party/dropbear-2012.55/sshpty.c:252:*/
#define SIG_IGN 0 /*third-party/dropbear-2012.55/sshpty.c:274:*/
#define LOG_ERR 0 /*third-party/dropbear-2012.55/sshpty.c:285:*/
#define TIOCSWINSZ 0 /*third-party/dropbear-2012.55/sshpty.c:352:*/
//#define S_IRUSR 0 /*third-party/dropbear-2012.55/sshpty.c:367:*/
//#define S_IWUSR 0 /*third-party/dropbear-2012.55/sshpty.c:367:*/
//#define S_IWGRP 0 /*third-party/dropbear-2012.55/sshpty.c:367:*/
//#define S_IWOTH 0 /*third-party/dropbear-2012.55/sshpty.c:370:*/
#define LOG_ERR 0 /*third-party/dropbear-2012.55/sshpty.c:387:*/
//#define S_IRWXU 0 /*third-party/dropbear-2012.55/sshpty.c:399:*/
//#define S_IRWXG 0 /*third-party/dropbear-2012.55/sshpty.c:399:*/
//#define S_IRWXO 0 /*third-party/dropbear-2012.55/sshpty.c:399:*/
//#define S_IRGRP 0 /*third-party/dropbear-2012.55/sshpty.c:402:*/
//#define S_IROTH 0 /*third-party/dropbear-2012.55/sshpty.c:402:*/
//---------------------------------------------------

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


struct passwd {
	char *pw_name;
	uid_t pw_uid;
	gid_t pw_gid;
	char *pw_dir;
	char *pw_shell;
	char *pw_passwd;
};

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

extern char *strdup(const char *s);
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
extern struct passwd *getpwnam(const char *name);

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
struct passwd *getpwuid(uid_t uid);
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

//------------------------------------------------------------


