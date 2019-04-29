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

#define EILSEQ 84

#define EMLINK 31

typedef __u16 u_int16_t;
typedef __u32 u_int32_t;


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


#include <grp.h>

static inline
int initgroups(const char *user, gid_t group) {
	DPRINT();
	errno = EPERM;
	return -1;
}

static inline
unsigned int alarm(unsigned int seconds) {
	DPRINT();
	return 0;
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

#if 0
static inline
void rewinddir(DIR *dirp) {
	DPRINT();
}
#endif

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
