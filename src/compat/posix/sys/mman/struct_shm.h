#ifndef STRUCT_SHM_H_
#define STRUCT_SHM_H_

#include <limits.h>
#include <fs/idesc.h>
#include <fs/file_desc.h>
#include <fs/inode.h>
#include <util/dlist.h>

struct shm {
	struct file_desc fd;
	struct inode inode;
	struct dlist_head lnk;
	char name[NAME_MAX + 1];
};

#endif /* STRUCT_SHM_H_ */
