/**
 * @brief
 *
 * @date 02.01.24
 * @author Aleksey Zhmulin
 */

#ifndef FS_VFS_FILE_H_
#define FS_VFS_FILE_H_

#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <kernel/sched/sync/mutex.h>
#include <kernel/task/resource/idesc.h>
#include <vfs/core.h>

struct stat;
struct dir_struct;

struct dir_idesc_ops {
	void (*close)(const struct inode *inode);
};

struct dir_idesc {
	struct idesc idesc;
	struct inode inode;
	const struct dir_idesc_ops *ops;
};

__BEGIN_DECLS

extern struct idesc *dir_idesc_open(const struct inode *inode,
    const struct dir_idesc_ops *ops, int oflag);

__END_DECLS

#endif /* FS_VFS_FILE_H_ */
