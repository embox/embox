/**
 * @file
 *
 * @brief
 *
 * @date Jul 23, 2013
 * @author: Vita Loginova
 */

#include <util/log.h>

#include <time.h>
#include <sys/time.h>
#include <limits.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <util/math.h>
#include <util/err.h>

#include <fs/file_desc.h>
#include <fs/fs_driver.h>
#include <fs/super_block.h>
#include <fs/hlpr_path.h>
#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/dir_context.h>
#include <fs/inode_operation.h>

#include <drivers/block_dev.h>

#include <mem/misc/pool.h>
#include <embox/unit.h>


#include <libsmbclient.h>

struct cifs_fs_info
{
	SMBCCTX *ctx;
	struct inode *mntto;
	char url[PATH_MAX];
};

/* ntfs filesystem description pool */
POOL_DEF (cifs_fs_pool, struct cifs_fs_info,
		  OPTION_GET (NUMBER, cifs_descriptor_quantity));

typedef struct smbitem smbitem;

struct smbitem
{
	smbitem *next;
	int type;
	char name[1];
};

static struct super_block_operations cifs_sbops;

static mode_t
samba_type_to_mode_fmt (const unsigned dt_type)
{
	switch (dt_type) {
	case SMBC_FILE_SHARE:
	case SMBC_DIR:
		return S_IFDIR;
	case SMBC_FILE:
		return S_IFREG;
	default:
		return 0;
	}
}

struct inode *
embox_set_node (struct inode *parent, char *filename, int mode) {
	struct inode *node;

	node = vfs_subtree_create (parent, filename, samba_type_to_mode_fmt (mode));
	if (!node) {
		return NULL;
	}
	return node;
}

SMBCCTX *
embox_create_smbctx (void)
{
	SMBCCTX *ctx;

	if ((ctx = smbc_new_context ()) == NULL)
		return NULL;

	//smbc_setDebug(ctx, debuglevel);
	//smbc_setFunctionAuthData(ctx, smbc_auth_fn);

	if (smbc_init_context (ctx) == NULL) {
		smbc_free_context (ctx, 1);
		return NULL;
	}

	return ctx;
}

void
embox_delete_smbctx (SMBCCTX * ctx)
{
	smbc_getFunctionPurgeCachedServers (ctx) (ctx);
	smbc_free_context (ctx, 1);
}
static void
cifs_fill_node(struct inode *node, char *name, unsigned type) {
	node->i_mode = samba_type_to_mode_fmt(type);
}

static int cifs_clean_sb(struct super_block *sb) {
	struct cifs_fs_info *fsi = sb->sb_data;

	if (fsi->ctx) {
		// ToDo: check if everything passed Ok
		embox_delete_smbctx(fsi->ctx);
	}
	pool_free(&cifs_fs_pool, fsi);

	return 0;
}

struct inode *cifs_lookup(char const *name, struct inode const *dir) {
	return NULL;
}

static void cifs_get_file_url(struct inode *node, char fileurl[PATH_MAX]) {
	int rc;
	struct cifs_fs_info *fsi;

	fsi = node->i_sb->sb_data;

	strncpy(fileurl, fsi->url, PATH_MAX - 1);
	fileurl[PATH_MAX - 1] = '\0';
	rc=strlen(fileurl);
	fileurl[rc] = '/';
	vfs_get_relative_path(node, &fileurl[rc + 1], PATH_MAX / 2);

}

int cifs_iterate(struct inode *next, char *name, struct inode *parent, struct dir_ctx *dir_ctx) {
	struct cifs_fs_info *fsi;
	char smb_path[PATH_MAX];
	struct smbc_dirent *dirent;
	SMBCCTX *ctx;
	SMBCFILE * fd_dir;
	int i = 0;
	int ret = -1;

	fsi = parent->i_sb->sb_data;
	ctx = fsi->ctx;

	cifs_get_file_url(parent, smb_path);

	if ((fd_dir = smbc_getFunctionOpendir (ctx) (ctx, smb_path)) == NULL) {
		log_error("could not open dir");
		return -1;
	}

	while ((dirent = smbc_getFunctionReaddir (ctx) (ctx, fd_dir)) != NULL) {
		if (strcmp (dirent->name, "") == 0) {
			continue;
		}
		if (strcmp (dirent->name, ".") == 0) {
			continue;
		}
		if (strcmp (dirent->name, "..") == 0) {
			continue;
		}

		if (i++ < (int)(uintptr_t)dir_ctx->fs_ctx) {
			continue;
		} else {
			cifs_fill_node(next, dirent->name, dirent->smbc_type);
			strncpy(name, dirent->name, NAME_MAX - 1);
			name[NAME_MAX - 1] = '0';
			dir_ctx->fs_ctx = (void *)(intptr_t)i;
			log_debug("found %s", name);

			switch (dirent->smbc_type) {
			case SMBC_DIR:
				break;
			case SMBC_FILE_SHARE:
				smbc_getFunctionPurgeCachedServers(ctx)(ctx);
				break;
			case SMBC_FILE:
				break;
			}
			ret = 0;
			break;
		}

	}

	smbc_getFunctionClose (ctx) (ctx, fd_dir);

	return ret;
}

static struct inode_operations cifs_iops = {
//	.ino_lookup   = cifs_lookup,
	.ino_iterate  = cifs_iterate,
};
static struct file_operations cifs_fop;
static int cifs_fill_sb(struct super_block *sb, const char *source) {
	SMBCCTX *ctx;
	char smb_path[PATH_MAX] = "smb://";
	struct cifs_fs_info *fsi;
	int rc;

	strncpy(smb_path + 6, source, sizeof (smb_path) - 7);
	smb_path[sizeof (smb_path) - 1] = '\0';
	rc = strlen(smb_path);
	if(smb_path[rc-1] == '/') {
		smb_path[rc-1] = '\0';
	}

	if ((ctx = embox_create_smbctx()) == NULL) {
		/* ToDo: error: exit without deallocation of filesystem */
		//Cant create samba context
		rc = 1;
		return -rc;
	}

	/* allocate this fs info */
	if (NULL == (fsi = pool_alloc(&cifs_fs_pool))) {
		/* ToDo: error: exit without deallocation of filesystem */
		return -ENOMEM;
	}
	memset (fsi, 0, sizeof(*fsi));
	strcpy (fsi->url, smb_path);
	fsi->ctx = ctx;
	sb->sb_data = fsi;
	sb->sb_ops = &cifs_sbops;
	sb->sb_iops = &cifs_iops;
	sb->sb_fops = &cifs_fop;


	return 0;
}

static int embox_cifs_mount(struct super_block *sb, struct inode *dir) {
	struct cifs_fs_info *fsi;
	char smb_path[PATH_MAX];

	fsi = sb->sb_data;

	fsi->mntto = dir;
	strcpy(smb_path, fsi->url);

	return 0;
}

static struct idesc *cifs_open(struct inode *node, struct idesc *idesc, int __oflag)
{
	struct cifs_fs_info *fsi;
	char fileurl[2 * PATH_MAX];
	SMBCFILE *file;
	struct stat st;
	int rc;

	fsi = node->i_sb->sb_data;

	strcpy(fileurl,fsi->url);
	fileurl[rc=strlen(fileurl)] = '/';

	vfs_get_relative_path(node, &fileurl[rc+1], PATH_MAX);

	if (smbc_getFunctionStat(fsi->ctx)(fsi->ctx, fileurl, &st)) {
		return err_ptr(errno);
	}

	file = smbc_getFunctionOpen(fsi->ctx)(fsi->ctx,fileurl,idesc->idesc_flags,0);
	if(!file) {
		return err_ptr(errno);
	}

	file_desc_set_file_info(file_desc_from_idesc(idesc), file);

	// Yet another bullshit: size is not valid until open
	file_set_size(file_desc_from_idesc(idesc), st.st_size);

	return idesc;
}

static int cifs_close(struct file_desc *file_desc)
{
	struct cifs_fs_info *fsi;
	SMBCFILE *file;

	fsi = file_desc->f_inode->i_sb->sb_data;
	file = file_desc->file_info;

	if (smbc_getFunctionClose(fsi->ctx)(fsi->ctx, file)) {
		return -errno;
	}

	return 0;
}

static size_t cifs_read(struct file_desc *file_desc, void *buf, size_t size)
{
	struct cifs_fs_info *fsi;
	SMBCFILE *file;
	size_t res;
	off_t pos;

	pos = file_get_pos(file_desc);

	fsi = file_desc->f_inode->i_sb->sb_data;
	file = file_desc->file_info;

	res = smbc_getFunctionLseek(fsi->ctx)(fsi->ctx, file, pos, SEEK_SET);
	if (res != pos) {
		if (-1 == res) {
			return -errno;
		} else {
			return EIO;
		}
	}

	res = smbc_getFunctionRead(fsi->ctx)(fsi->ctx, file, buf, size);

	return res;
}

static size_t cifs_write(struct file_desc *file_desc, void *buf, size_t size) {
	struct cifs_fs_info *fsi;
	SMBCFILE *file;
	size_t res;
	off_t pos;

	pos = file_get_pos(file_desc);

	fsi = file_desc->f_inode->i_sb->sb_data;
	file = file_desc->file_info;

	res = smbc_getFunctionLseek(fsi->ctx)(fsi->ctx, file, pos, SEEK_SET);
	if (res != pos) {
		if (-1 == res) {
			return -errno;
		} else {
			return -EIO;
		}
	}

	res = smbc_getFunctionWrite(fsi->ctx)(fsi->ctx, file, buf, size);

	if (res > 0) {
		file_set_size(file_desc, max(file_get_size(file_desc), pos));
	}

	return res;
}

static int embox_cifs_node_create(struct inode *new_node, struct inode *parent_node, int I_mode) {
	struct cifs_fs_info *pfsi;
	char fileurl[PATH_MAX];
	SMBCFILE *file;
	mode_t mode;
	int rc;

	pfsi = parent_node->i_sb->sb_data;

	strcpy(fileurl,pfsi->url);
	fileurl[rc=strlen(fileurl)] = '/';

	vfs_get_relative_path(new_node, &fileurl[rc+1], PATH_MAX - rc - 1);

	mode = new_node->i_mode & S_IRWXA;
	if (node_is_directory(new_node)) {
		mode |= S_IFDIR;
		if (smbc_getFunctionMkdir(pfsi->ctx)(pfsi->ctx, fileurl, mode)) {
			return -errno;
		}
	} else {
		mode |= S_IFREG;
		file = smbc_getFunctionCreat(pfsi->ctx)(pfsi->ctx, fileurl, mode);
		if (!file) {
			return -errno;
		}
		if (smbc_getFunctionClose(pfsi->ctx)(pfsi->ctx, file)) {
			return -errno;
		}
	}

	return 0;
}

static int embox_cifs_node_delete(struct inode *node) {
	struct cifs_fs_info *fsi;
	char fileurl[PATH_MAX];
	int rc;

	fsi = node->i_sb->sb_data;

	strcpy(fileurl,fsi->url);
	fileurl[rc=strlen(fileurl)] = '/';
#if 0
	if ((rc = vfs_get_pathbynode_tilln(node, fsi->mntto, &fileurl[rc+1], sizeof(fileurl)-rc-1))) {
		return rc;
	}
#endif
	vfs_get_relative_path(node, &fileurl[rc+1], PATH_MAX - rc - 1);

	if (node_is_directory(node)) {
		if (smbc_getFunctionRmdir(fsi->ctx)(fsi->ctx, fileurl)) {
			return -errno;
		}
	} else {
		if (smbc_getFunctionUnlink(fsi->ctx)(fsi->ctx, fileurl)) {
			return -errno;
		}
	}

	return 0;
}

static int cifs_destroy_inode(struct inode *inode) {
	return 0;
}

static struct super_block_operations cifs_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = cifs_destroy_inode,
};

static const struct fsop_desc cifs_fsop = {
	.create_node = embox_cifs_node_create,
	.delete_node = embox_cifs_node_delete,
	.mount = embox_cifs_mount,
};

static struct file_operations cifs_fop = {
	.open = cifs_open,
	.close = cifs_close,
	.read = cifs_read,
	.write = cifs_write,
};

static const struct fs_driver cifs_driver = {
	.name     = "cifs",
	.fill_sb  = cifs_fill_sb,
	.clean_sb = cifs_clean_sb,
	.fsop     = &cifs_fsop,
//	.file_op  = &cifs_fop,
};

DECLARE_FILE_SYSTEM_DRIVER (cifs_driver);
