#ifndef DIRENT_IMPL_H_
#define DIRENT_IMPL_H_

#include <sys/types.h>
#include <sys/cdefs.h>

#include <dirent.h>
#include <fs/dir_context.h>

struct dentry;

struct DIR_struct {
	struct dirent dirent;
	struct dir_ctx ctx;
	struct dentry *dir_dentry;
	/* Entries readdir() has returned since the start: what telldir()
	 * answers and seekdir() counts back to. */
	long pos;
};

#endif /* DIRENT_IMPL_H_ */
