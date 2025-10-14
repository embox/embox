#ifndef ARDUPILOT_VFS_H_
#define ARDUPILOT_VFS_H_

#include <sys/cdefs.h>

typedef struct {
	int val[2];
} fsid_t;

struct statfs {
   long    f_type;
   long    f_bsize;
   long    f_blocks;
   long    f_bfree;
   long    f_bavail;
   long    f_files;
   long    f_ffree;
   fsid_t  f_fsid;
   long    f_namelen;
   long    f_spare[6];
};

__BEGIN_DECLS
extern int statfs(const char *path, struct statfs *buf);
__END_DECLS
#endif