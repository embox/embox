/**
 *
 * @date 23.11.2012
 * @author Alexander Kalmuk
 */

#ifndef DIRENT_H_
#define DIRENT_H_

#include <sys/types.h>


struct dirent {
	ino_t  d_ino;      /* File serial number. */
	char   d_name[40]; /* Name of entry. */
};

typedef struct DIR {
	struct dirent;
} DIR;


extern int            closedir(DIR *);

extern DIR           *opendir(const char *);

extern struct dirent *readdir(DIR *);

extern int            readdir_r(DIR *restrict, struct dirent *restrict,
                   struct dirent **restrict);

extern void           rewinddir(DIR *);

extern void           seekdir(DIR *, long);

extern long           telldir(DIR *);

#endif /* DIRENT_H_ */
