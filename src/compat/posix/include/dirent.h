/**
 *
 * @date 23.11.2012
 * @author Alexander Kalmuk
 */

#ifndef DIRENT_H_
#define DIRENT_H_

#include <sys/types.h>
#include <sys/cdefs.h>
#include <fs/node.h>

__BEGIN_DECLS

#define DIRENT_DNAME_LEN 40

struct dirent {
	ino_t  d_ino;      /* File serial number. */
	char   d_name[DIRENT_DNAME_LEN]; /* Name of entry. */
};

typedef struct DIR {
	struct dirent current;
	node_t *node;
	struct tree_link *child_lnk;
} DIR;


extern int            closedir(DIR *);

extern DIR           *opendir(const char *);

extern struct dirent *readdir(DIR *);

extern int            readdir_r(DIR *, struct dirent *,
				struct dirent **);

extern void           rewinddir(DIR *);

extern void           seekdir(DIR *, long);

extern long           telldir(DIR *);

__END_DECLS

#endif /* DIRENT_H_ */
