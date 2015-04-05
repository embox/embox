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
	ino_t  d_ino;                    /* File serial number. */
	char   d_name[DIRENT_DNAME_LEN]; /* Name of entry. */
};

typedef struct DIR_struct DIR;


extern int            closedir(DIR *);

extern DIR           *opendir(const char *);

extern struct dirent *readdir(DIR *);

extern int            readdir_r(DIR *, struct dirent *, struct dirent **);

__END_DECLS

#endif /* DIRENT_H_ */
