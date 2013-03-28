/**
 * @file
 *
 * @brief
 * @date 23.11.2012
 * @author Alexander Kalmuk
 */
/* <sys/wait.h> */
/* use in waitpid()  */
#define WNOHANG 0 /*third-party/dropbear-2012.55/svr-chansession.c:91:*/


#define WIFEXITED(s) 0
#define WEXITSTATUS(s) 0
#define WIFSIGNALED(s) 0
#define WTERMSIG(s) 0

/* end <sys/wait.h> */

/* <sygnal.h> */
#define SA_NOCLDSTOP 0 /*third-party/dropbear-2012.55/svr-chansession.c:139:*/
//#define SA_NOCLDSTOP 0 /*third-party/dropbear-2012.55/svr-chansession.c:983:*/

static inline int sigemptyset(sigset_t *set) {
	return -1;
}
/* end <sygnal.h> */

//---------------------------------------------------
/* <unistd.h */
static inline pid_t setsid(void) {
	return -1;
}

static inline int chown(const char *path, uid_t owner, gid_t group) {
	return -1;
}

extern char *crypt(const char *key, const char *salt);
/* end <unistd.h> */

/* <netdb.h> */
static inline struct hostent *gethostbyaddr(const void *addr,
                                     socklen_t len, int type) {
	return NULL;
}
static inline struct servent *getservbyname(const char *name, const char *proto) {
	return NULL;
}
/* end <netdb.h> */


/* <time.h> */
static inline struct tm *localtime(const time_t *timep) {
	return NULL;
}
/* end <time.h> */




/* <grp.h> */
/* initgroups NOT POSIX */
static inline int initgroups(const char *user, gid_t group) {
	return -1;
}

/* <pwd.h> */
extern struct passwd *getpwnam(const char *name);
/* end <pwd.h> */

#include <asm/termbits.h>
#include <sys/select.h>

//------------------------------------------------------------


