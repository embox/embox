/**
 * @file
 *
 * @brief
 *
 * @date Jul 23, 2013
 * @author: Vita Loginova
 */

#include <time.h>
#include <sys/time.h>
#include <limits.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <util/err.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>
#include <fs/fs_driver.h>
#include <libsmbclient.h>
#include <fs/vfs.h>
#include <fs/inode.h>
#include <drivers/block_dev.h>
#include <mem/misc/pool.h>
#include <embox/unit.h>
#include <fs/hlpr_path.h>

#include <util/math.h>


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
embox_set_node (struct nas *nas, char *filename, int mode)
{
	struct inode *node;
	node = vfs_subtree_create (nas->node, filename, samba_type_to_mode_fmt (mode));
	if (!node) {
		return NULL;
	}
	node->nas->fs = nas->fs;
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

static int
embox_cifs_mounting_recurse (struct nas *nas, SMBCCTX * ctx, char *smb_path,
							 int maxlen)
{
	int len, rc;
	struct smbc_dirent *dirent;
	SMBCFILE * fd;
	struct inode *node;

	len = strlen (smb_path);

	if ((fd = smbc_getFunctionOpendir (ctx) (ctx, smb_path)) == NULL)
		return -errno;
	while ((dirent = smbc_getFunctionReaddir (ctx) (ctx, fd)) != NULL) {
		if (strcmp (dirent->name, "") == 0)
			continue;
		if (strcmp (dirent->name, ".") == 0)
			continue;
		if (strcmp (dirent->name, "..") == 0)
			continue;
		switch (dirent->smbc_type) {
		case SMBC_FILE_SHARE:
		case SMBC_DIR:
		case SMBC_FILE:
			//ToDo: use namelen
			if (maxlen < len + strlen (dirent->name) + 2)
				break;

			smb_path[len] = '/';
			strcpy (smb_path + len + 1, dirent->name);
			node = embox_set_node (nas, dirent->name, dirent->smbc_type);
			if (!node) {
				errno = ENOMEM;
				return -errno;
			}
			if (dirent->smbc_type != SMBC_FILE) {
				rc = embox_cifs_mounting_recurse (node->nas, ctx, smb_path,
												  maxlen);
				if (0 > rc) {
					return rc;
				}
				if (dirent->smbc_type == SMBC_FILE_SHARE)
					smbc_getFunctionPurgeCachedServers (ctx) (ctx);
			}
			break;
		}
	}

	smbc_getFunctionClose (ctx) (ctx, fd);

	smb_path[len] = '\0';
	return 0;
}

static int cifs_umount_entry(struct nas *nas) {
	struct inode *child;

	if (node_is_directory(nas->node)) {
		while (NULL != (child =	vfs_subtree_get_child_next(nas->node, NULL))) {
			if(node_is_directory(child)) {
				cifs_umount_entry(child->nas);
			}

			(void)0; // no CIFS-specific resources to free
			vfs_del_leaf(child);
		}
	}

	return 0;
}

static int embox_cifs_umount(struct inode *dir) {
	struct nas *dir_nas;
	struct cifs_fs_info *fsi;

	dir_nas = dir->nas;

	/* delete all entry node */
	cifs_umount_entry(dir_nas);

	if(NULL != dir_nas->fs) {
		fsi = dir_nas->fs->sb_data;

		if(NULL != fsi) {
			if (fsi->ctx) {
				// ToDo: check if everything passed Ok
				embox_delete_smbctx(fsi->ctx);
			}
			pool_free(&cifs_fs_pool, fsi);
		}
		super_block_free(dir_nas->fs);
		dir_nas->fs = NULL;
	}

	return 0;
}

static int embox_cifs_mount(const char *source, struct inode *dir) {
	SMBCCTX *ctx;
	char smb_path[PATH_MAX] = "smb://";
	struct nas *dir_nas;
	struct cifs_fs_info *fsi;
	int rc;

	strncpy(smb_path + 6, source, sizeof (smb_path) - 7);
	smb_path[sizeof (smb_path) - 1] = '\0';
	rc = strlen(smb_path);
	if(smb_path[rc-1] == '/') {
		smb_path[rc-1] = '\0';
	}

	dir_nas = dir->nas;

	dir_nas->fs = super_block_alloc("cifs", NULL);
	if (NULL == dir_nas->fs) {
		rc = ENOMEM;
		return -rc;
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
		rc = ENOMEM;
		goto error;
	}
	memset (fsi, 0, sizeof(*fsi));
	strcpy (fsi->url, smb_path);
	fsi->mntto = dir;
	fsi->ctx = ctx;
	dir_nas->fs->sb_data = fsi;

	//get smb_path
	rc = embox_cifs_mounting_recurse(dir->nas, ctx, smb_path,
			sizeof (smb_path));
	if (0 > rc) {
		goto error;
	}

	return 0;

error:
	embox_cifs_umount(dir);
	return -rc;
}

static struct idesc *cifs_open(struct inode *node, struct idesc *idesc)
{
	struct cifs_fs_info *fsi;
	char fileurl[2 * PATH_MAX];
	SMBCFILE *file;
	struct stat st;
	int rc;

	fsi = node->nas->fs->sb_data;

	strcpy(fileurl,fsi->url);
	fileurl[rc=strlen(fileurl)] = '/';
#if 0
	if ((rc = vfs_get_pathbynode_tilln(node, fsi->mntto, &fileurl[rc+1], sizeof(fileurl)-rc-1))) {
		return rc;
	}
#endif

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

	fsi = file_desc->f_inode->nas->fs->sb_data;
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

	fsi = file_desc->f_inode->nas->fs->sb_data;
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

	fsi = file_desc->f_inode->nas->fs->sb_data;
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

static int embox_cifs_node_create(struct inode *parent_node, struct inode *new_node) {
	struct cifs_fs_info *pfsi;
	char fileurl[PATH_MAX];
	SMBCFILE *file;
	mode_t mode;
	int rc;

	pfsi = parent_node->nas->fs->sb_data;

	strcpy(fileurl,pfsi->url);
	fileurl[rc=strlen(fileurl)] = '/';
#if 0
	if ((rc = vfs_get_pathbynode_tilln(new_node, pfsi->mntto, &fileurl[rc+1], sizeof(fileurl)-rc-1))) {
		return rc;
	}
#endif
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

	new_node->nas->fs = parent_node->nas->fs;

	return 0;
}

static int embox_cifs_node_delete(struct inode *node) {
	struct cifs_fs_info *fsi;
	char fileurl[PATH_MAX];
	int rc;

	fsi = node->nas->fs->sb_data;

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

static const struct fsop_desc cifs_fsop = {
	.create_node = embox_cifs_node_create,
	.delete_node = embox_cifs_node_delete,
	.mount = embox_cifs_mount,
	.umount = embox_cifs_umount,
};

static struct file_operations cifs_fop = {
	.open = cifs_open,
	.close = cifs_close,
	.read = cifs_read,
	.write = cifs_write,
};

static const struct fs_driver cifs_driver = {
	.name = "cifs",
	.fsop = &cifs_fsop,
	.file_op = &cifs_fop,
};

DECLARE_FILE_SYSTEM_DRIVER (cifs_driver);
