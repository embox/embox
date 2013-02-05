/**
 * @file
 *
 * @date 26.12.12
 * @author Alexander Kalmuk
 */

#ifndef PWD_H_
#define PWD_H_

#if 0
struct passwd {
	char *pw_name;
	uid_t pw_uid;
	gid_t pw_gid;
	char *pw_dir;
	char *pw_shell;
	char *pw_passwd;
};

extern struct passwd *getpwuid(uid_t uid);
extern struct passwd *getpwnam(const char *name);
#endif

#endif /* PWD_H_ */
