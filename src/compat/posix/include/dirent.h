/**
 *
 * @date 23.11.2012
 * @author Alexander Kalmuk
 */

#ifndef DIRENT_H_
#define DIRENT_H_

#include <sys/types.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

#define DIRENT_DNAME_LEN 40

struct dirent {
	/* TODO: POSIX requiers ino_t to be defined as unsigned integer
	 * https://pubs.opengroup.org/onlinepubs/009695399/basedefs/sys/types.h.html
	 * but in Embox d_ino is used as a pointer in some modules, which
	 * limits VFS to use inodes from first 4 GiB of memory  */
	ino_t  d_ino;                    /* File serial number. */
	char   d_name[DIRENT_DNAME_LEN]; /* Name of entry. */

	/*only for linux compatible */
	off_t          d_off;       /* not an offset; see NOTES */
	unsigned short d_reclen;    /* length of this record */
	unsigned char  d_type;      /* type of file; not supported
	                            by all filesystem types */
};

typedef struct DIR_struct DIR;


extern int            closedir(DIR *);

extern DIR           *opendir(const char *);

extern struct dirent *readdir(DIR *);

extern int            readdir_r(DIR *, struct dirent *, struct dirent **);

extern void           rewinddir(DIR *dirp);

static inline
int scandir(const char *dir, struct dirent ***namelist,
       int (*sel)(const struct dirent *),
       int (*compar)(const struct dirent **, const struct dirent **)) {
	(void)dir;
	(void)namelist;
	(void)sel;
	(void)compar;
	return -1;
}

__END_DECLS

#endif /* DIRENT_H_ */
