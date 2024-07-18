/**
 *
 * @date 23.11.2012
 * @author Alexander Kalmuk
 */

#ifndef DIRENT_H_
#define DIRENT_H_

#include <sys/cdefs.h>
#include <sys/types.h>

/* Constants for the file types returned in the d_type */
#define DT_UNKNOWN 0
#define DT_BLK     2
#define DT_CHR     4
#define DT_DIR     6
#define DT_FIFO    8
#define DT_LNK     10
#define DT_REG     12
#define DT_SOCK    14

__BEGIN_DECLS

struct dirent {
	/* TODO: POSIX requiers ino_t to be defined as unsigned integer
	 * https://pubs.opengroup.org/onlinepubs/009695399/basedefs/sys/types.h.html
	 * but in Embox d_ino is used as a pointer in some modules, which
	 * limits VFS to use inodes from first 4 GiB of memory  */
	ino_t d_ino;           /* File serial number. */
	char d_name[NAME_MAX]; /* Name of entry. */

	/*only for linux compatible */
	off_t d_off;             /* not an offset; see NOTES */
	unsigned short d_reclen; /* length of this record */
	unsigned char d_type;    /* type of file; not supported
	                            by all filesystem types */
};

typedef struct DIR_struct DIR;

extern int closedir(DIR *);

extern DIR *opendir(const char *);

extern struct dirent *readdir(DIR *);

extern int readdir_r(DIR *, struct dirent *, struct dirent **);

extern void rewinddir(DIR *dirp);

static inline int scandir(const char *dir, struct dirent ***namelist,
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
