#ifndef DIRENT_IMPL_H_
#define DIRENT_IMPL_H_

#include <sys/types.h>
#include <sys/cdefs.h>

#include <fs/dvfs.h>

struct DIR_struct {
	struct dirent dirent;
	struct dir_ctx ctx;
	struct dentry *dir_dentry;
	struct dentry *prv_dentry;
};

#endif /* DIRENT_IMPL_H_ */
