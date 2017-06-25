/**
 *
 * @date 23.11.2012
 * @author Alexander Kalmuk
 */


#ifndef GRP_H_
#define GRP_H_

#include <sys/types.h>
#include <sys/cdefs.h>

struct group {
	char   *gr_name; /* The name of the group. */
	char   *gr_passwd;
	gid_t   gr_gid; /* Numerical group ID. */
	char  **gr_mem; /* Pointer to a null-terminated array of character pointers to member names.*/
};

static inline struct group *getgrnam(const char *name) {
	(void)name;
	return NULL;
}

__BEGIN_DECLS

extern struct group * getgrgid(gid_t gid);
extern int getgrgid_r(gid_t, struct group *, char *, size_t, struct group **);
extern int getgrnam_r(const char *, struct group *, char *, size_t , struct group **);

__END_DECLS

#endif /* GRP_H_ */
