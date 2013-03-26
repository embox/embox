/**
 * @file
 *
 * @brief
 * @date 23.11.2012
 * @author Alexander Kalmuk
 */
#if 0
#define TIOCSWINSZ 0 /*third-party/dropbear-2012.55/sshpty.c:352:*/

struct winsize /* third-party/dropbear-2012.55/sshpty.c:346: */
{
  unsigned short ws_row;	/* rows, in characters */
  unsigned short ws_col;	/* columns, in characters */
  unsigned short ws_xpixel;	/* horizontal size, pixels */
  unsigned short ws_ypixel;	/* vertical size, pixels */
};
#endif
#define WNOHANG 0 /*third-party/dropbear-2012.55/svr-chansession.c:91:*/
#define SA_NOCLDSTOP 0 /*third-party/dropbear-2012.55/svr-chansession.c:139:*/
#define SA_NOCLDSTOP 0 /*third-party/dropbear-2012.55/svr-chansession.c:983:*/
//---------------------------------------------------
#if 0
typedef int sigset_t;
#endif
//-------------------------------------------------------

static inline pid_t setsid(void) {
	return -1;
}

static inline struct hostent *gethostbyaddr(const void *addr,
                                     socklen_t len, int type) {
	return NULL;
}
static inline struct servent *getservbyname(const char *name, const char *proto) {
	return NULL;
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

//extern int gethostname(char *name, size_t len);

static inline int initgroups(const char *user, gid_t group) {
	return -1;
}

extern struct passwd *getpwnam(const char *name);

#include <asm/termbits.h>
#include <sys/select.h>

//------------------------------------------------------------


