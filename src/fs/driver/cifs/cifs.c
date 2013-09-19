/**
 * @file
 *
 * @brief
 *
 * @date Jul 23, 2013
 * @author: Vita Loginova
 */

#include <time.h>
#include <fs/fs_driver.h>
#include <libsmbclient.h>
#include <fs/vfs.h>
#include <embox/block_dev.h>
#include <limits.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <mem/misc/pool.h>
#include <embox/unit.h>
#include <fs/path.h>
#include <errno.h>


struct cifs_fs_info
{
	SMBCCTX *ctx;
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

struct node *
embox_set_node (struct nas *nas, char *filename, int mode)
{
	struct node *node;
	node = vfs_create (nas->node, filename, samba_type_to_mode_fmt (mode));
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
	struct node *node;

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

static int
embox_cifs_mount (void *dev, void *dir)
{
	SMBCCTX *ctx;
	char smb_path[PATH_MAX] = "smb://";
	struct node *dir_node;
	struct nas *dir_nas;
	struct cifs_fs_info *fsi;
	int rc;

	strncpy (smb_path + 6, dev, sizeof (smb_path) - 7);
	smb_path[sizeof (smb_path) - 1] = '\0';

	dir_node = dir;
	dir_nas = dir_node->nas;

	if (NULL != vfs_get_child_next (dir_node)) {
		rc = ENOTEMPTY;
		return -rc;
	}

	if (NULL == (dir_nas->fs = filesystem_create ("cifs"))) {
		rc = ENOMEM;
		return -rc;
	}

	if ((ctx = embox_create_smbctx ()) == NULL) {
		/* ToDo: error: exit without deallocation of filesystem */
		//Cant create samba context
		rc = 1;
		return -rc;
	}

	/* allocate this fs info */
	if (NULL == (fsi = pool_alloc (&cifs_fs_pool))) {
		/* ToDo: error: exit without deallocation of filesystem */
		rc = ENOMEM;
		goto error;
	}
	memset (fsi, 0, sizeof (*fsi));
	strcpy (fsi->url, smb_path);
	fsi->ctx = ctx;
	dir_nas->fs->fsi = fsi;

	//get smb_path
	rc = embox_cifs_mounting_recurse (dir_node->nas, ctx, smb_path,
									  sizeof (smb_path));
	if (0 > rc) {
		//embox_delete_smbctx(ctx);
		return rc;
	}

	//embox_delete_smbctx(ctx);
	return 0;

error:
	//embox_cifs_umount(dir);
	return -rc;
}

static const struct fsop_desc cifs_fsop = {
	.mount = embox_cifs_mount,
};

static const struct fs_driver cifs_driver = {
	.name = "cifs",
	.fsop = &cifs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER (cifs_driver);
