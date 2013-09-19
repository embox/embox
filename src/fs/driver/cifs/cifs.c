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


typedef struct smbitem smbitem;

struct smbitem{
    smbitem	*next;
    int		type;
    char	name[1];
};

static mode_t samba_type_to_mode_fmt(const unsigned dt_type) {
	switch (dt_type) {
		case SMBC_FILE_SHARE:
		case SMBC_DIR:
			return S_IFDIR;
		case SMBC_FILE:
			return S_IFREG;
		default: return 0;
	}
}

struct node* embox_set_node(struct nas *nas, char *filename, int mode) {
	struct node *node;
	node = vfs_create(nas->node, filename, samba_type_to_mode_fmt(mode));
	if (!node) {
		return NULL;
	}
	node->nas->fs = nas->fs;
	return node;
}

SMBCCTX* embox_create_smbctx(void){
    SMBCCTX	*ctx;

    if ((ctx = smbc_new_context()) == NULL) return NULL;

    //smbc_setDebug(ctx, debuglevel);
    //smbc_setFunctionAuthData(ctx, smbc_auth_fn);

    if (smbc_init_context(ctx) == NULL){
	smbc_free_context(ctx, 1);
	return NULL;
    }

    return ctx;
}

void embox_delete_smbctx(SMBCCTX* ctx){
    smbc_getFunctionPurgeCachedServers(ctx)(ctx);
    smbc_free_context(ctx, 1);
}

smbitem* embox_get_smbitem_list(SMBCCTX *ctx, char *smb_path){
    SMBCFILE		*fd;
    struct smbc_dirent	*dirent;
    smbitem		*list = NULL, *item;

    if ((fd = smbc_getFunctionOpendir(ctx)(ctx, smb_path)) == NULL)
        return NULL;
    while((dirent = smbc_getFunctionReaddir(ctx)(ctx, fd)) != NULL){
	if (strcmp(dirent->name, "") == 0) continue;
	if (strcmp(dirent->name, ".") == 0) continue;
	if (strcmp(dirent->name, "..") == 0) continue;

	if ((item = malloc(sizeof(smbitem) + strlen(dirent->name))) == NULL)
	    continue;

	item->next = list;
	item->type = dirent->smbc_type;
	strcpy(item->name, dirent->name);
	list = item;
    }
    smbc_getFunctionClose(ctx)(ctx, fd);
    return /* smbitem_list_sort */ (list);

}

static int embox_recurse(struct nas *nas, SMBCCTX *ctx, char *smb_group, char *smb_path, int maxlen){
    int 	len, rc;
    smbitem	*list, *item;
    SMBCCTX	*ctx1;
    char filename[NAME_MAX + 1] = "";
    struct node *node;

    len = strlen(smb_path);

    list = embox_get_smbitem_list(ctx, smb_path);
    while(list != NULL){
	switch(list->type){
	    case SMBC_WORKGROUP:
	    case SMBC_SERVER:
	    //in this case there is no error handling
		if (list->type == SMBC_WORKGROUP){
			//embox_set_node(list->name, "");
		    smb_group = list->name;
		}
		//else embox_set_node(smb_group, list->name);

		if (maxlen < 7 + strlen(list->name)) break;
		strcpy(smb_path + 6, list->name);
		if ((ctx1 = embox_create_smbctx()) != NULL){
			embox_recurse(node->nas, ctx1, smb_group, smb_path, maxlen);
			embox_delete_smbctx(ctx1);
		}else{
			embox_recurse(node->nas, ctx, smb_group, smb_path, maxlen);
		    smbc_getFunctionPurgeCachedServers(ctx)(ctx);
		}
		break;
	    case SMBC_FILE_SHARE:
	    case SMBC_DIR:
	    case SMBC_FILE:
		if (maxlen < len + strlen(list->name) + 2) break;

		smb_path[len] = '/';
		strcpy(smb_path + len + 1, list->name);
		strcpy(filename, list->name);
		//embox_set_node(smb_group, smb_path + 6);
		node = embox_set_node(nas, filename, list->type);
		if (!node) {
			errno = ENOMEM;
			return -errno;
		}
		if (list->type != SMBC_FILE){
			rc = embox_recurse(node->nas, ctx, smb_group, smb_path, maxlen);
			if (0 > rc) {
				free(item);
				return rc;
			}
		    if (list->type == SMBC_FILE_SHARE)
			smbc_getFunctionPurgeCachedServers(ctx)(ctx);
		}
		break;
	}
	item = list;
	list = list->next;
	free(item);
    }
    smb_path[len] = '\0';
    return 0;
}

static int embox_samba_mount(void *dev, void *dir) {
	//TODO
	SMBCCTX *ctx;
	char smb_path[PATH_MAX] = "smb://";
	//try recurse
	//char smb_path[PATH_MAX] = "smb://192.168.33.129";
	struct node *dir_node, *dev_node;
	struct nas *dir_nas, *dev_nas;
	struct node_fi *dev_fi;
	int rc;

	dev_node = dev;
	dev_nas = dev_node->nas;
	dir_node = dir;
	dir_nas = dir_node->nas;

	if (NULL == (dev_fi = dev_nas->fi)) {
			rc = ENODEV;
			return -rc;
		}

	if(NULL != vfs_get_child_next(dir_node)) {
		rc = ENOTEMPTY;
		return -rc;
	}

	if (NULL == (dir_nas->fs = filesystem_create("cifs"))) {
		rc = ENOMEM;
		return -rc;
	}

	if ((ctx = embox_create_smbctx()) == NULL) {
		//Cant create samba context
		rc = 1;
		return -rc;
	}

	//get smb_path
	rc = embox_recurse(dir_node->nas, ctx, "", smb_path, sizeof(smb_path));
	if (0 > rc) {
		embox_delete_smbctx(ctx);
		return rc;
	}

	embox_delete_smbctx(ctx);
	return 0;
}

static const struct fsop_desc cifs_fsop = {
	.init = 0,
	.mount = embox_samba_mount,
};

static const struct fs_driver cifs_driver = {
	.name = "cifs",
	.fsop = &cifs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(cifs_driver);
