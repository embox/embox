#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>

#include <util/err.h>
#include <lib/libds/array.h>

#include <drivers/char_dev.h>
#include <drivers/block_dev.h>
#include <drivers/device.h>
#include <module/embox/driver/block_dev.h>

#include <fs/dir_context.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/mount.h>
#include <fs/super_block.h>
#include <fs/dentry.h>
#include <fs/file_desc.h>
#include <fs/fs_driver.h>

#include <net/net_namespace.h>
#include <kernel/task.h>

#include <framework/mod/options.h>


extern struct idesc *dvfs_file_open_idesc(struct lookup *lookup, int __oflag);

extern net_namespace_p net_ns_lookup(const char *name);
extern net_namespace_p net_ns_lookup_by_inode(struct inode *inode);

unsigned long int strtoul(const char *nptr, char **endptr, int base);
static int is_number(const char *str) {
	char *invalid;
	strtoul(str, &invalid, 10);
	if (invalid == str)
		return 0;
	else
		return 1;
}

static struct inode *procfs_lookup(struct inode *node, char const *name, struct inode const *dir) {
	net_namespace_p net_ns_p;

	/* /proc/pid/ns/net case */
	if (is_number(name))
		node->i_mode = S_IFDIR;
	else if (strcmp(name, "ns") == 0)
		node->i_mode = S_IFDIR;
	else if (strcmp(name, "net") == 0) {
		node->i_size = NAME_MAX + 1;
		node->i_mode = S_IFLNK;
	}

	/* /proc/netns/name case */
	if (strcmp(name, "netns") == 0)
		node->i_mode = S_IFDIR;
	else if (strcmp(dir->i_dentry->name, "netns") == 0) {
		net_ns_p = net_ns_lookup(name);
		if (net_ns_p.p != NULL) {
			return net_ns_p.p->inode;
		} else {
			return NULL;
		}
	}

	return node;
}

static int procfs_iterate(struct inode *next, char *name, struct inode *parent,
			  struct dir_ctx *ctx) {
	struct task *tsk, *last;
	int show_this_task = 0;

	last = (struct task *)ctx->fs_ctx;

	task_foreach(tsk) {
		if (show_this_task || last == NULL) {
			ctx->fs_ctx = (void *)tsk;
			next->i_mode = S_IFDIR; 
			snprintf(name, NAME_MAX - 1, "%d", tsk->tsk_id);
			name[NAME_MAX - 1] = '\0';
			return 0;
		}

		if (tsk == last) {
			show_this_task = 1;
		}
	}

	return -1;
}

static size_t procfs_fill_read_buff(struct file_desc *desc, void *buf,
				    size_t size, const char *content) {
	off_t pos;
	size_t content_size;

	pos = file_get_pos(desc);
	content_size = strlen(content) + 1;

	if (pos + size > content_size) {
		size = content_size - pos;
	}

	memcpy(buf, content + pos, size);

	return size;
}

static size_t procfs_read(struct file_desc *desc, void *buf, size_t size) {
	char netns_name[NAME_MAX + 1];
	char netns_path[1024];
	unsigned long pid;
	struct task *tsk;

	if (S_ISLNK(desc->f_inode->i_mode)) { /* /proc/pid/ns/net case */
		pid = strtoul(desc->f_dentry->parent->parent->name, NULL, 10);
		task_foreach(tsk) {
			if (tsk->tsk_id != pid)
				continue;
			memcpy(netns_name, tsk->nsproxy.net_ns.p->name,
			       strlen(desc->f_inode->i_dentry->name) + 1);
			sprintf(netns_path, "/proc/netns/%s", netns_name);
			return procfs_fill_read_buff(desc, buf, size,
						     netns_path);
		}
		return 0;
	} else {
		return 0;
	}
}

struct idesc *procfs_open_idesc(struct lookup *lookup, int __oflag) {
	struct file_desc *desc;
	net_namespace_p net_ns_p;

	desc = (struct file_desc *)dvfs_file_open_idesc(lookup, __oflag);

	net_ns_p = net_ns_lookup_by_inode(desc->f_inode);

	if (net_ns_p.p != NULL)
		netns_increment_ref_cnt(net_ns_p);

	return &desc->f_idesc;
}

int procfs_close(struct file_desc *desc) {
	net_namespace_p net_ns_p;

	net_ns_p = net_ns_lookup_by_inode(desc->f_inode);

	if (net_ns_p.p != NULL)
		netns_decrement_ref_cnt(net_ns_p);

	return 0;
}

static struct super_block_operations procfs_sbops = {
	.open_idesc = procfs_open_idesc,
};

struct inode_operations procfs_iops = {
	.ino_lookup   = procfs_lookup,
	.ino_iterate  = procfs_iterate,
};

struct file_operations procfs_fops = {
	.read  = procfs_read,
	.close  = procfs_close
};

static int procfs_fill_sb(struct super_block *sb, const char *source) {
	if (source) {
		return -1;
	}

	sb->sb_iops = &procfs_iops;
	sb->sb_fops = &procfs_fops;
	sb->sb_ops  = &procfs_sbops;

	return 0; 
}

static const struct fs_driver procfs_dumb_driver = {
	.name      = "procfs",
	.fill_sb   = procfs_fill_sb,
};

DECLARE_FILE_SYSTEM_DRIVER(procfs_dumb_driver);

FILE_SYSTEM_AUTOMOUNT("/proc", procfs_dumb_driver);
