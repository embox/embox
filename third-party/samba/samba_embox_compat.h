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

#if 1
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

#define ELOOP 40

#define SIG_BLOCK 0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2
typedef int sigset_t;
static inline int sigprocmask(int how, const sigset_t *set, sigset_t *oldset) {
	printf(">>> sigprocmask(%i,%p,%p)\n",how,set,oldset);
	return -1;
}
static inline int sigaddset(sigset_t *set, int signum){
	printf(">>> sigaddset(%p,%i)\n",set,signum);
	if (signum>=sizeof(sigset_t)*8) {
		return -1;
	}
	*set |= 1<<signum;
	return 0;
}
static inline int sigemptyset(sigset_t *set) {
	*set = 0;
	printf(">>> sigemptyset()\n");
	return -1;
}
static inline int sigaction(int signum, const struct sigaction *act,
	      struct sigaction *oldact) {
	printf(">>> sigaction(%x,%p,%p)\n",signum,act,oldact);
	return -1;
}


int getpagesize(void);
#define FD_CLOEXEC	(printf(">>> FC_CLOEXEC\n"),0)

typedef struct { int val[2]; } fsid_t;

#include <sys/select.h>
// ToDo: this is actually already defined
#define FD_SETSIZE (_FDSETWORDS*_FDSETBITSPERWORD)

#include <assert.h>
#undef assert
#define assert(x)

#define FIONREAD 0

struct sockaddr_un {
    unsigned short sun_family;  /* AF_UNIX */
    char sun_path[108];
};

#define WNOHANG 0


#define MAP_SHARED    0x00
#define MAP_PRIVATE   0x01
#define PROT_READ     0x10
#define PROT_WRITE    0x20
#define MAP_FAILED    (-1)
#include <errno.h>
static inline void  *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
	// ToDo: implement for InitFS files
	(void)addr;
	(void)len;
	(void)prot;
	(void)flags;
	(void)off;
	printf(">>> mmap(%i)\n",fd);
	errno = EPERM;
	return 0;
}

static inline int munmap(void *addr, size_t size) {
	(void)size;
	printf(">>> munmap(%p)\n",addr);
	errno = EPERM;
	return -1;
}


struct in_addr inet_makeaddr(int net, int host);

#define E2BIG  7
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

#endif /* SAMBA_EMBOX_COMPAT_H_ */
