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

/* A DIR holds a reference on the inode in its path, from
 * opendir() to closedir(). if_mounted_follow_down() REPLACES that inode with
 * the root of whatever is mounted there, so anything that follows a DIR down
 * has to carry the reference with it. Use this instead of calling
 * if_mounted_follow_down() on a DIR's path: doing it by hand leaves the
 * reference on the directory the DIR no longer points at, and closedir() then
 * releases one it never took. */
extern int dir_follow_down(DIR *dir);

#endif /* DIRENT_IMPL_H_ */
