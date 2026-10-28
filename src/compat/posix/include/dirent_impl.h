#ifndef DIRENT_IMPL_H_
#define DIRENT_IMPL_H_

#include <sys/types.h>
#include <sys/cdefs.h>

#include <lib/libds/slist.h>

#include <fs/path.h>
#include <fs/dir_context.h>

struct DIR_struct {
	struct dirent current;
	struct path path;
	struct dir_ctx dir_context;
	struct slist inodes_list;
};

/* Follows a DIR onto the volume mounted at its path, moving the DIR's
 * reference with it. Use instead of if_mounted_follow_down() on a DIR's
 * path, which would leave the reference behind. */
extern int dir_follow_down(DIR *dir);

#endif /* DIRENT_IMPL_H_ */
