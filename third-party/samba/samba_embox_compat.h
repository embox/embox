/*
 * samba_embox_compat.h
 *
 *  Created on: 21 mars 2013
 *      Author: fsulima
 */

#ifndef SAMBA_EMBOX_COMPAT_H_
#define SAMBA_EMBOX_COMPAT_H_

#ifdef linux
#undef linux
#endif

#ifdef __linux__
#undef __linux__
#endif

#if 0
#define DPRINT() printf(">>> samba CALL %s\n", __FUNCTION__)
#else
#define DPRINT()
#endif

#include <stdio.h>

struct statfs {
	int f_bsize;   /* optimal transfer block size */
};
int statfs(const char *path, struct statfs *buf);

#define ID_EFFECTIVE 0
#define ID_REAL 1

#define SIG_BLOCK 0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

//#define FD_CLOEXEC	(printf(">>> FC_CLOEXEC\n"),0)

#include <sys/select.h>

#include <assert.h>
#undef assert
#define assert(x, msg...)

struct sockaddr_un {
    unsigned short sun_family;  /* AF_UNIX */
    char sun_path[108];
};


#include <sys/mman.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#define	IN_CLASSA(a)		((((in_addr_t)(a)) & 0x80000000) == 0)
#define	IN_CLASSA_NET		0xff000000
#define	IN_CLASSA_NSHIFT	24
#define	IN_CLASSA_HOST		(0xffffffff & ~IN_CLASSA_NET)
#define	IN_CLASSA_MAX		128
#define	IN_CLASSB(a)		((((in_addr_t)(a)) & 0xc0000000) == 0x80000000)
#define	IN_CLASSB_NET		0xffff0000
#define	IN_CLASSB_NSHIFT	16
#define	IN_CLASSB_HOST		(0xffffffff & ~IN_CLASSB_NET)
#define	IN_CLASSB_MAX		65536
#define	IN_CLASSC(a)		((((in_addr_t)(a)) & 0xe0000000) == 0xc0000000)
#define	IN_CLASSC_NET		0xffffff00
#define	IN_CLASSC_NSHIFT	8
#define	IN_CLASSC_HOST		(0xffffffff & ~IN_CLASSC_NET)
static inline
struct in_addr
inet_makeaddr(net, host)
	in_addr_t net, host;
{
	struct in_addr in;

	if (net < 128)
		in.s_addr = (net << IN_CLASSA_NSHIFT) | (host & IN_CLASSA_HOST);
	else if (net < 65536)
		in.s_addr = (net << IN_CLASSB_NSHIFT) | (host & IN_CLASSB_HOST);
	else if (net < 16777216L)
		in.s_addr = (net << IN_CLASSC_NSHIFT) | (host & IN_CLASSC_HOST);
	else
		in.s_addr = net | host;
	in.s_addr = htonl(in.s_addr);
	return in;
}

#define EILSEQ 84

#define EMLINK 31

typedef __u16 u_int16_t;
typedef __u32 u_int32_t;

static inline void atexit(void *addr) {
	printf(">>> atexit(%p)\n",addr);
}

static inline void *dlsym(void *handle, const char *symbol) {
	printf(">>> dlsym(%p,%s)\n",handle,symbol);
	return NULL;
}

static inline
int dn_expand(unsigned char *msg, unsigned char *eomorig,
              unsigned char *comp_dn, char *exp_dn,
              int length) {
	printf(">>> dn_expand(%s)\n",comp_dn);
	return -1;
}
static inline
int res_query(const char *dname, int class, int type,
              unsigned char *answer, int anslen) {
	printf(">>> res_query(%s)\n",dname);
	return -1;
}

static inline
int socketpair(int domain, int type, int protocol, int sv[2]) {
	DPRINT();
	return -1;
}

static inline
char *mktemp(char *template) {
	DPRINT();
	if (template) {
		*template = 0;
	}
	return template;
}

static inline
ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
	DPRINT();
	errno = ENOSYS;
	return -1;
}

static inline
ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
	int i;
	size_t bw = 0;
	DPRINT();
	for(i=0; i<iovcnt; i++) {
		int res;
		if (iov[i].iov_len) {
			res = write(fd, iov[i].iov_base, iov[i].iov_len);
		} else {
			res = 0;
		}
		if (res<0) {
			return -1;
		}
		if (res != iov[i].iov_len) {
			errno = EIO;
			return -1;
		}
		bw += res;
	}
	return bw;
}

#include <grp.h>

static inline
int initgroups(const char *user, gid_t group) {
	DPRINT();
	errno = EPERM;
	return -1;
}

static inline
void setgrent(void) {
	DPRINT();
}

static inline
struct group *getgrent(void) {
	DPRINT();
	errno = EPERM;
	return 0;
}

static inline
void endgrent(void) {
	DPRINT();
}

static inline
unsigned int alarm(unsigned int seconds) {
	DPRINT();
	return 0;
}

static inline
int chown(const char *path, uid_t owner, gid_t group) {
	DPRINT();
	errno = EPERM;
	return -1;
}

#include <time.h>
static inline
time_t timegm(struct tm *tm) {
	DPRINT();
	return mktime(tm);
}

static inline
int execl(const char *path, const char *arg, ...) {
	DPRINT();
	errno = EPERM;
	return -1;
}

static inline
int execvp(const char *file, char *const argv[]) {
	DPRINT();
	errno = EPERM;
	return -1;
}

#include <sys/types.h>
static inline
int getgroups(int size, gid_t list[]) {
	DPRINT();
	errno = -EPERM;
	return -1;
}

#include <stdlib.h>

#include <dirent.h>
static inline
void seekdir(DIR *dirp, long offset) {
	DPRINT();
}

static inline
long telldir(DIR *dirp) {
	DPRINT();
	errno = EPERM;
	return -1;
}

static inline
void rewinddir(DIR *dirp) {
	DPRINT();
}

static inline
void
swab (const void *bfrom, void *bto, ssize_t n)
{
  const char *from = (const char *) bfrom;
  char *to = (char *) bto;

  n &= ~((ssize_t) 1);
  while (n > 1)
    {
      const char b0 = from[--n], b1 = from[--n];
      to[n] = b0;
      to[n + 1] = b1;
    }
}

static inline
int fnmatch(const char *pattern, const char *string, int flags) {
	DPRINT();
	return -1;
}

//XXX redefine malloc through sysmalloc. Revert it!
#include <stdlib.h>
#define malloc(x)     sysmalloc(x)
#define free(x)       sysfree(x)
#define memalign(x,y) sysmemalign(x,y)
#define realloc(x,y)  sysrealloc(x,y)
#define calloc(x,y)   syscalloc(x,y)

#ifndef HAVE_CLOCK_GETTIME
#define HAVE_CLOCK_GETTIME
#endif

#define rb_replace_node rb_replace_node_samba
#define rb_prev rb_prev_samba
#define rb_next rb_next_samba
#define rb_first rb_first_samba
#define rb_insert_color rb_insert_color_samba
#define rb_erase rb_erase_samba

static inline int fork(void) {
	DPRINT();
	return -1;
}

static inline int fdatasync(int fd) {
	DPRINT();
	return -1;
}

#endif /* SAMBA_EMBOX_COMPAT_H_ */
