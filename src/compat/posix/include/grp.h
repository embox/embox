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

__BEGIN_DECLS

extern int fgetgrent_r(FILE *fp, struct group *gbuf, char *tbuf,
		size_t buflen, struct group **gbufp);
extern struct group *fgetgrent(FILE *stream);

extern struct group * getgrgid(gid_t gid);
extern struct group *getgrnam(const char *name);
extern int getgrgid_r(gid_t gid, struct group *gbuf, char *buf,
		size_t buflen, struct group **gbufp);
extern int getgrnam_r(const char *name, struct group *gbuf, char *buf,
		size_t buflen, struct group **gbufp);

extern int getgrent_r(struct group *gbuf, char *buf,
		size_t buflen, struct group **gbufp);
extern struct group *getgrent(void);
extern void setgrent(void);
extern void endgrent(void);

__END_DECLS

#endif /* GRP_H_ */
