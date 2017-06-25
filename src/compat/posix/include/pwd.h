/**
 * @file
 *
 * @date 26.12.12
 * @author Alexander Kalmuk
 */

#ifndef PWD_H_
#define PWD_H_

#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

struct passwd {
	char *pw_name;
	char *pw_passwd;
	uid_t pw_uid;
	gid_t pw_gid;
	char *pw_gecos;
	char *pw_dir;
	char *pw_shell;
};

extern int fgetpwent_r(FILE *file, struct passwd *pwd, char *buf,
		size_t buflen, struct passwd **result);

extern int getpwnam_r(const char *name, struct passwd *pwd,
		char *buf, size_t buflen, struct passwd **result);

extern int getpwuid_r(uid_t uid, struct passwd *pwd,
		char *buf, size_t buflen, struct passwd **result);

extern struct passwd *getpwnam(const char *name);

extern struct passwd *getpwuid(uid_t uid);



extern int getmaxuid(void);
extern int get_defpswd(struct passwd *passwd, char *buf, size_t buf_len);


static inline void endpwent(void) { }
static inline struct passwd * getpwent(void) { return NULL; }
static inline void setpwent(void) { }

__END_DECLS

#endif /* PWD_H_ */
