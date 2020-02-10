/**
 * @file
 * @brief
 *
 * @date 12.05.2012
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <util/log.h>

#include <fs/vfs.h>
#include <fs/nfs.h>
#include <fs/xdr_nfs.h>
#include <fs/hlpr_path.h>
#include <fs/inode.h>
#include <fs/file_desc.h>
#include <fs/fs_driver.h>
#include <fs/file_operation.h>
#include <fs/super_block.h>
#include <limits.h>

#include <mem/misc/pool.h>
#include <mem/sysmalloc.h>
#include <net/lib/rpc/clnt.h>
#include <net/lib/rpc/xdr.h>
#include <util/math.h>
#include <util/err.h>


static int nfs_create_dir_entry(struct inode *parent);

static int nfs_mount(struct nas *nas);
static int nfs_lookup(struct nas *nas);
static int nfs_call_proc_nfs(struct nas *nas,
		__u32 procnum, char *req, char *reply);

/* nfs filesystem description pool */
POOL_DEF (nfs_fs_pool, struct nfs_fs_info, OPTION_GET(NUMBER,nfs_descriptor_quantity));

/* nfs file description pool */
POOL_DEF (nfs_file_pool, struct nfs_file_info, OPTION_GET(NUMBER,inode_quantity));

/* File operations */

static struct idesc *nfsfs_open(struct inode *node, struct idesc *idesc);
static int    nfsfs_close(struct file_desc *desc);
static size_t nfsfs_read(struct file_desc *desc, void *buf, size_t size);
static size_t nfsfs_write(struct file_desc *desc, void *buf, size_t size);

static struct file_operations nfsfs_fop = {
	.open = nfsfs_open,
	.close = nfsfs_close,
	.read = nfsfs_read,
	.write = nfsfs_write,
};

static void unaligned_set_hyper(uint64_t *dst, void *src) {
	memcpy(dst, src, sizeof *dst);
}

/*
 * file_operation
 */
static struct idesc *nfsfs_open(struct inode *node, struct idesc *idesc) {
	nfs_file_info_t *fi;
	struct nas *nas;
	off_t pos;

	pos = file_get_pos(file_desc_from_idesc(idesc));

	nas = node->nas;
	fi = (nfs_file_info_t *)nas->fi->privdata;

	fi->offset = pos;

	if(0 == nfs_lookup(nas)) {
		nas->fi->ni.size = fi->attr.size;
		return idesc;
	}
	return err_ptr(ENOENT);
}

static int nfsfs_close(struct file_desc *desc) {
	nfs_file_info_t *fi;
	struct nas *nas;

	nas = desc->f_inode->nas;
	fi = (nfs_file_info_t *)nas->fi->privdata;
	fi->offset = 0;

	return 0;
}

static size_t nfsfs_read(struct file_desc *desc, void *buf, size_t size) {
	size_t size_to_read;
	nfs_file_info_t *fi;
	read_req_t req;
	read_reply_t reply;
	size_t datalen;
	struct nas *nas;
	off_t pos;

	pos = file_get_pos(desc);

	nas = desc->f_inode->nas;
	fi = (nfs_file_info_t *) nas->fi->privdata;
	datalen = 0;
	fi->offset = pos;

	while(1) {
		size_to_read = min(size, DIRCOUNT);

		/* set read structure */
		req.count = size_to_read;
		req.offset = fi->offset;
		req.fh = &fi->fh.name_fh;
		reply.datalen = 0;
		reply.data = (char *) buf + datalen;

		/* send read command */
		if (0 > nfs_call_proc_nfs(nas, NFSPROC3_READ,
				(char *) &req, (char *) &reply)) {
			return 0;
		}

		if (reply.datalen == 0) {
			break;
		}

		size -= reply.datalen;
		fi->offset += reply.datalen;
		datalen += reply.datalen;

		if (reply.eof || (0 >= size )) {
			break;
		}
	}
	return datalen;
}

static size_t nfsfs_write(struct file_desc *desc, void *buf, size_t size) {

	size_t size_to_write;
	nfs_file_info_t *fi;
	write_req_t req;
	write_reply_t reply;
	struct nas *nas;
	off_t pos;

	pos = file_get_pos(desc);

	size_to_write = size;
	nas = desc->f_inode->nas;
	fi = (nfs_file_info_t *) nas->fi->privdata;
	fi->offset = pos;

	/* set read structure */
	req.count = req.datalen = size_to_write;
	req.data = (char *) buf;
	req.offset = fi->offset;
	req.fh = &fi->fh.name_fh;
	req.stable = FILE_SYNC;

	reply.attr = &fi->attr;

	/* send read command */
	if (0 > nfs_call_proc_nfs(nas, NFSPROC3_WRITE,
			(char *) &req, (char *) &reply)) {
		return 0;
	}

	fi->offset += reply.count;
	pos = fi->offset;
	if (nas->fi->ni.size < pos) {
		nas->fi->ni.size = pos;
	}

	return reply.count;
}


/*
static int nfsfs_fseek(void *file, long offset, int whence);

static int nfsfs_fseek(void *file, long offset, int whence) {

	struct file_desc *desc;
	nfs_file_info_t *fi;

	desc = (struct file_desc *) file;
	fi = (nfs_file_info_t *) desc->node->fi;

	switch (whence) {
	case SEEK_SET:
		fi->offset = offset;
		break;
	case SEEK_CUR:
		fi->offset += offset;
		break;
	case SEEK_END:
		fi->offset = fi->attr.size + offset;
		break;
	default:
		return -1;
	}
	return 0;
}
*/

/* File system operations */

static int nfsfs_format(struct block_dev *bdev, void *priv);
static int nfsfs_mount(const char *source, struct inode *dest);
static int nfsfs_create(struct inode *parent_node, struct inode *node);
static int nfsfs_delete(struct inode *node);
static int nfsfs_truncate (struct inode *node, off_t length);
static int nfsfs_umount(struct inode *dir);

static struct fsop_desc nfsfs_fsop = {
	.format = nfsfs_format,
	.mount = nfsfs_mount,
	.create_node = nfsfs_create,
	.delete_node = nfsfs_delete,

	.truncate = nfsfs_truncate,
	.umount = nfsfs_umount,
};

static struct fs_driver nfsfs_driver = {
	.name = "nfs",
	.file_op = &nfsfs_fop,
	.fsop = &nfsfs_fsop,
};

static int nfsfs_format(struct block_dev *bdev, void *priv) {
	return 0;
}

static int nfsfs_truncate (struct inode *node, off_t length) {
	struct nas *nas = node->nas;

	nas->fi->ni.size = length;

	return 0;
}

static int nfs_unix_auth_set(struct client *clnt) {
	if (NULL == clnt)  {
		return -1;
	}
	if (NULL != clnt->ath) {
		auth_destroy(clnt->ath);
	}
	clnt->ath = authunix_create(EMBOX_MACHNAME, 0, 0, 0, NULL);
	if (clnt->ath == NULL) {
		return -1;
	}

	return 0;
}

static int nfs_clnt_destroy (struct nfs_fs_info *fsi) {

	if (NULL != fsi->mnt) {
		clnt_destroy(fsi->mnt);
		fsi->mnt = NULL;
	}
	if (NULL != fsi->nfs) {
		clnt_destroy(fsi->nfs);
		fsi->nfs = NULL;
	}
	return 0;
}

static int nfs_prepare(struct nfs_fs_info *fsi, const char *src) {
	char *dst;

	/* copy name of server and mount filesystem server directory*/
	dst = fsi->srv_name;

	do {
		if ('\0' == *src) {
			return -1;
		}
		*dst++ = *src++;
	} while (':' != *src);
	*dst = 0;

	src++;
	dst = fsi->srv_dir;
	do {
		*dst++ = *src++;
	} while ('\0' != *src);
	return 0;
}

static int nfs_client_init(struct nfs_fs_info *fsi) {

	nfs_clnt_destroy(fsi);
	/* Create nfs programm clients */
	fsi->mnt = clnt_create(fsi->srv_name,
			MOUNT_PROGNUM, MOUNT_VER, "tcp");
	if (fsi->mnt == NULL) {
		clnt_pcreateerror(fsi->srv_name);
		return -1;
	}
	if(0 != nfs_unix_auth_set(fsi->mnt)) {
		return -1;
	}

	fsi->nfs = clnt_create(fsi->srv_name, NFS_PROGNUM, NFS_VER, "tcp");
	if (fsi->nfs == NULL) {
		clnt_pcreateerror(fsi->srv_name);
		return -1;
	}

	return nfs_unix_auth_set(fsi->nfs);
}

static void nfs_free_fs(struct nas *nas) {
	struct nfs_file_info *fi;
	struct nfs_fs_info *fsi;

	if(NULL != nas->fs) {
		fsi = nas->fs->sb_data;

		if(NULL != fsi) {
			nfs_clnt_destroy(fsi);
			pool_free(&nfs_fs_pool, fsi);
		}
		super_block_free(nas->fs);
	}

	if(NULL != (fi = nas->fi->privdata)) {
		pool_free(&nfs_file_pool, fi);
	}
}

static int nfsfs_mount(const char *source, struct inode *dest) {
	nfs_file_info_t *fi;
	struct nas *dir_nas;
	struct nfs_fs_info *fsi;
	int rc;

	dir_nas = dest->nas;

	dir_nas->fs = super_block_alloc("nfs", NULL);
	if (NULL == dir_nas->fs) {
		return -ENOMEM;
	}

	if ((NULL == (fsi = pool_alloc(&nfs_fs_pool))) ||
			(NULL == (fi = pool_alloc(&nfs_file_pool)))) {
		rc = -ENOMEM;
		goto error;
	}

	dir_nas->fs->sb_data = fsi;
	dir_nas->fi->privdata = (void *) fi;

	memset(fsi, 0, sizeof *fsi);
	memset(fi, 0, sizeof *fi); /* FIXME maybe not required */

	/* get server name and mount directory from params */
	if ((0 > nfs_prepare(fsi, source)) || (0 > nfs_client_init(fsi)) ||
		(0 > nfs_mount(dir_nas))) {
		rc = -1;
		goto error;
	}

	/* copy filesystem filehandle to root directory filehandle */
	memcpy(&fi->fh, &fsi->fh, sizeof(fi->fh));

	if (0 >	nfs_create_dir_entry(dest)) { // XXX check the argument
		rc = -1;
	} else {
		return 0;
	}

	error:
	nfs_free_fs(dir_nas);

	return rc;
}

static int nfs_umount_entry(struct nas *nas) {
	struct inode *child;

	if(node_is_directory(nas->node)) {
		while (NULL != (child =	vfs_subtree_get_child_next(nas->node, NULL))) {
			if (node_is_directory(child)) {
				nfs_umount_entry(child->nas);
			}

			pool_free(&nfs_file_pool, child->nas->fi->privdata);
			vfs_del_leaf(child);
		}
	}

	return 0;
}

static int nfsfs_umount(struct inode *dir) {
	/* delete all entry node */
	nfs_umount_entry(dir->nas);

	/* free nfs file system pools, clnt and buffers*/
	nfs_free_fs(dir->nas);

	return 0;
}

static struct inode *nfs_create_file(struct nas *parent_nas, readdir_desc_t *predesc) {
	struct nas *nas;
	struct inode *node;
	nfs_file_info_t *fi;
	const char *name;
	mode_t mode;

	nas = NULL; /* XXX required for -O2 */

	name = (const char *) predesc->file_name.name.data;

	node = vfs_subtree_lookup_child(parent_nas->node, name);
	if (node) {
		nas = node->nas;
		fi = nas->fi->privdata;

	} else {
		/* TODO usually mount doesn't create a directory */
		fi = pool_alloc(&nfs_file_pool);
		if (!fi) {
			return NULL;
		}

	}

	/* copy read the description in the created file*/
	memcpy(&fi->name_dsc, &predesc->file_name,
			sizeof(predesc->file_name));

	if (VALUE_FOLLOWS_YES == predesc->vf_attr) {
		memcpy(&fi->attr, &predesc->file_attr,
				sizeof(predesc->file_attr));
	}
	if (VALUE_FOLLOWS_YES == predesc->vf_fh) {
		memcpy(&fi->fh, &predesc->file_handle,
				sizeof(predesc->file_handle));
		fi->fh.count = fi->fh.maxcount = DIRCOUNT;
		fi->fh.cookie = 0;
	}

	if (!node) {
		mode = fi->attr.mode;
		// TODO what is mode is not known (!VALUE_FOLLOWS_YES)?

		/**
		 *	enum ftype3 {
		 *		NF3REG    = 1,
		 *		NF3DIR    = 2,
		 *		NF3BLK    = 3,
		 *		NF3CHR    = 4,
		 *		NF3LNK    = 5,
		 *		NF3SOCK   = 6,
		 *		NF3FIFO   = 7
		 *	};
		 */
		switch (predesc->file_attr.type) {
			case 1:
				/* regular file */
				mode |= S_IFREG;
				break;
			case 2:
				/* directory */
				mode |= S_IFDIR;
				break;
			default:
				/* unknown file type. Skip it. */
				log_error("Unsupported file type=0x%x (name=%s). Skip it...\n",
					predesc->file_attr.type,
					predesc->file_name.name.data);
				pool_free(&nfs_file_pool, fi);
				return NULL;
		}

		node = vfs_subtree_create_child(parent_nas->node, name, mode);
		if (!node) {
			pool_free(&nfs_file_pool, fi);
			return NULL; /* device not found */
		}

		nas = node->nas;
	}

	nas->fs = parent_nas->fs;
	nas->fi->privdata = fi;
	return node;
}

static int nfs_create_dir_entry(struct inode *parent_node) {
	struct inode *node;
	struct nas *parent_nas;
	__u32 vf;
	char *point;
	nfs_file_info_t *parent_fi;
	nfs_filehandle_t *fh;
	readdir_desc_t *predesc = NULL;

	char *rcv_buf;

	parent_nas = parent_node->nas;
	parent_fi = (nfs_file_info_t *) parent_nas->fi->privdata;
	fh = &parent_fi->fh;
	fh->count = fh->maxcount = DIRCOUNT;
	fh->cookie = 0;

	if (NULL == (rcv_buf = sysmalloc(MAXDIRCOUNT * 2))) {
		return -1;
	}

	while (1) {

		memset(rcv_buf, 0, MAXDIRCOUNT * 2);

		if (0 >  nfs_call_proc_nfs(parent_nas, NFSPROC3_READDIRPLUS,
			(char *)fh, rcv_buf)) {
			sysfree(rcv_buf);
			return -1;
		}

		point = rcv_buf;
		/* check status */
		vf = *(__u32 *)point;
		if (STATUS_OK != vf) {
			sysfree(rcv_buf);
			return -1;
		}
		point += sizeof(vf);

		/* check if a directory attributes */
		vf = *(__u32 *)point;
		if (VALUE_FOLLOWS_YES != vf) {
			break;
		}
		point += sizeof(vf);

		/*TODO copy root dir attr*/
		point += sizeof(dir_attribute_rep_t);

		/* check if a new files attributes */
		vf = *(__u32 *)point;
		if (VALUE_FOLLOWS_YES != vf) {
			break;
		}

		while (VALUE_FOLLOWS_YES == (vf = *(__u32 *)point)) {
			point += sizeof(vf);
			predesc = (readdir_desc_t *) point;

			if(0 == path_is_dotname(predesc->file_name.name.data,
									predesc->file_name.name.len)) {
				node = nfs_create_file(parent_nas, predesc);
				if (!node) {
					log_error("nfs_create_file failed\n");
				} else if (node_is_directory(node)) {
					nfs_create_dir_entry(node);
				}
			}
			point += sizeof(*predesc);
		}
		point += sizeof(vf);
		if (NFS_EOF != *(__u32 *)point) {
			unaligned_set_hyper(&fh->cookie, &predesc->file_name.cookie);
		} else {
			fh->cookie = 0;
			break;
		}
	}
	sysfree(rcv_buf);
	return 0;
}

static int nfsfs_create(struct inode *parent_node, struct inode *node) {

	nfs_file_info_t *parent_fi, *fi;
	struct nas *nas, *parent_nas;
	create_req_t  req;
	rpc_string_t name;
	create_reply_t reply;
	__u32 procnum;

	nas = node->nas;
	parent_nas = parent_node->nas;
	nas->fs = parent_nas->fs;

	parent_fi = (nfs_file_info_t *) parent_nas->fi->privdata;

	if (node_is_directory(node)) {
		procnum = NFSPROC3_MKDIR;
		req.type = NFS_DIRECTORY_NODE_TYPE;
		req.create_mode = UNCHECKED_MODE;
		req.mode = 0x01ff;
		req.size = 512;
	}
	else {
		procnum = NFSPROC3_CREATE;
		req.type = NFS_FILE_NODE_TYPE;
		req.create_mode = GURDED_MODE;
		req.mode = 0x31ff;
		req.size = 0;
	}
	/* set dir filehandle */
	req.new.dir_fh = &parent_fi->fh.name_fh;
	/* set new file name */
	memset((void *) &name, 0, sizeof(name));
	strncpy(name.data, node->name, NAME_MAX + 1);
	name.len = strlen(node->name);
	req.new.fname = &name;
	/* set attribute of new file */
	req.mode_vf = req.uid_vf = req.gid_vf = req.size_vf =
			req.set_atime =	req.set_mtime =  VALUE_FOLLOWS_YES;
	req.uid = req.gid = 0;

	/* send nfs CREATE command   */
	if (0 > nfs_call_proc_nfs(nas, procnum, (char *) &req, (char *) &reply)) {
		return -1;
	}

	if(NULL == (fi = pool_alloc(&nfs_file_pool))) {
		return -1;
	}
	nas->fi->privdata = (void *) fi;

	return nfs_create_dir_entry(parent_node); // XXX parent_node? or node?
}

static int nfsfs_delete(struct inode *node) {
	nfs_file_info_t *fi;
	struct nas *nas, *dir_nas;
	struct inode *dir_node;
	nfs_file_info_t *dir_fi;
	lookup_req_t req;
	delete_reply_t reply;
	__u32 procnum;

	nas = node->nas;
	fi = (nfs_file_info_t *) nas->fi->privdata;

	if(NULL == (dir_node = vfs_subtree_get_parent(node))) {
		return -1;
	}

	/* set delete structure */
	req.fname = &fi->name_dsc.name;
	dir_nas = dir_node->nas;
	dir_fi = (nfs_file_info_t *) dir_nas->fi->privdata;
	req.dir_fh = &dir_fi->fh.name_fh;

	if (node_is_directory(node)) {
		procnum = NFSPROC3_RMDIR;
	}
	else {
		procnum = NFSPROC3_REMOVE;
	}

	reply.dir_attr = &dir_fi->attr;

	/* send delete command */
	if (0 > nfs_call_proc_nfs(nas, procnum, (char *) &req, (char *) &reply)) {
		return -1;
	}

	pool_free(&nfs_file_pool, fi);

	return 0;
}

DECLARE_FILE_SYSTEM_DRIVER(nfsfs_driver);

static int nfs_call_proc_mnt(struct nas *nas,
		__u32 procnum, char *req, char *reply) {
	struct timeval timeout = { 25, 0 };
	struct nfs_fs_info *fsi;

	fsi = nas->fs->sb_data;

	if(NULL == fsi->mnt){
		if(0 >  nfs_client_init(fsi)) {
			return -1;
		}
	}

	switch (procnum) {
	case MOUNTPROC3_NULL:
		if (clnt_call(fsi->mnt, MOUNTPROC3_NULL,
			(xdrproc_t)xdr_void, 0,
			(xdrproc_t)xdr_void, 0,
			timeout) != RPC_SUCCESS) {
			/*
			 * An error occurred while calling the server.
			 * Print error message and die
			 */
			clnt_perror(fsi->mnt, fsi->srv_name);
			/* error code in client, now */
			printf("mnt null failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case MOUNTPROC3_MNT:
		if (clnt_call(fsi->mnt, MOUNTPROC3_MNT,
			(xdrproc_t)xdr_wrapstring, req,
			(xdrproc_t)xdr_mnt_service, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fsi->mnt, fsi->srv_name);
			printf("mnt mount failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case MOUNTPROC3_EXPORT:
		if (clnt_call(fsi->mnt, MOUNTPROC3_EXPORT,
			(xdrproc_t)xdr_void, 0,
			(xdrproc_t)xdr_mnt_export, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fsi->mnt, fsi->srv_name);
			printf("mnt export failed. errno=%d\n", errno);
			return -1;
		}
		break;

	default:
		return -1;
	}
	return 0;
}

static int nfs_call_proc_nfs(struct nas *nas,
		__u32 procnum, char *req, char *reply) {
	struct timeval timeout = { 25, 0 };
	struct nfs_fs_info *fsi;

	fsi = nas->fs->sb_data;
	if(NULL == fsi->nfs){
		if(0 >  nfs_client_init(fsi)) {
			return -1;
		}
	}

	switch (procnum) {
	case NFSPROC3_NULL:
		if (clnt_call(fsi->nfs, NFSPROC3_NULL,
			(xdrproc_t)xdr_void, 0,
			(xdrproc_t)xdr_void, 0,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fsi->nfs, fsi->srv_name);
			printf("nfs null failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_LOOKUP:
		if (clnt_call(fsi->nfs, NFSPROC3_LOOKUP,
			(xdrproc_t)xdr_nfs_lookup, req,
			(xdrproc_t)xdr_nfs_lookup, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fsi->nfs, fsi->srv_name);
			printf("nfs lookup failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_READ:
		if (clnt_call(fsi->nfs, NFSPROC3_READ,
			(xdrproc_t)xdr_nfs_read_file, req,
			(xdrproc_t)xdr_nfs_read_file, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fsi->nfs, fsi->srv_name);
			printf("read file failed. errno=%d\n", errno);
			return 0;
		}
		break;
	case NFSPROC3_WRITE:
		if (clnt_call(fsi->nfs, NFSPROC3_WRITE,
			(xdrproc_t)xdr_nfs_write_file, req,
			(xdrproc_t)xdr_nfs_write_file, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fsi->nfs, fsi->srv_name);
			printf("write file failed. errno=%d\n", errno);
			return 0;
		}
		break;
	case NFSPROC3_CREATE:
	case NFSPROC3_MKDIR:
		if (clnt_call(fsi->nfs, procnum,
			(xdrproc_t)xdr_nfs_create, req,
			(xdrproc_t)xdr_nfs_create, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fsi->nfs, fsi->srv_name);
			printf("nfs create failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_REMOVE:
	case NFSPROC3_RMDIR:
		if (clnt_call(fsi->nfs, procnum,
			(xdrproc_t)xdr_nfs_delete, req,
			(xdrproc_t)xdr_nfs_delete, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fsi->nfs, fsi->srv_name);
			printf("nfs delete failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_READDIRPLUS:
		if (clnt_call(fsi->nfs, NFSPROC3_READDIRPLUS,
			(xdrproc_t)xdr_nfs_readdirplus, req,
			(xdrproc_t)xdr_nfs_get_dirlist, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fsi->nfs, fsi->srv_name);
			printf("nfs readdirplus failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_FSSTAT:
		if (clnt_call(fsi->nfs, NFSPROC3_FSSTAT,
			(xdrproc_t)xdr_nfs_name_fh, req,
			(xdrproc_t)xdr_void, 0,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fsi->nfs, fsi->srv_name);
			printf("nfs fsstat failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_FSINFO:
		if (clnt_call(fsi->nfs, NFSPROC3_FSINFO,
			(xdrproc_t)xdr_nfs_name_fh, req,
			(xdrproc_t)xdr_void, 0,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fsi->nfs, fsi->srv_name);
			printf("nfs fsinfo failed. errno=%d\n", errno);
			return -1;
		}
		break;
	default:
		return -1;
	}
	return 0;
}

static int nfs_lookup(struct nas *nas) {
	struct inode *dir_node;
	struct nas *dir_nas;
	nfs_file_info_t *fi, *dir_fi;
	lookup_req_t req;
	lookup_reply_t reply;

	fi = nas->fi->privdata;
	if(NULL == (dir_node = vfs_subtree_get_parent(nas->node))) {
		return -1;
	}
	dir_nas = dir_node->nas;

	/* set lookup structure */
	req.fname = &fi->name_dsc.name;
	dir_fi = (nfs_file_info_t *) dir_nas->fi->privdata;
	req.dir_fh = &dir_fi->fh.name_fh;

	reply.fh = &fi->fh.name_fh;

	/* send read command */
	return nfs_call_proc_nfs(nas, NFSPROC3_LOOKUP,
			(char *) &req, (char *) &reply);
}

static int nfs_mount(struct nas *nas) {
	char *point;
	nfs_filehandle_t *p_fh;
	mount_service_t mnt_svc;
	struct nfs_fs_info *fsi;
	export_dir_t export;

	fsi = nas->fs->sb_data;

	/* get server mount directory name*/
	memset((void *)&export, 0, sizeof(export));
	if (0 > nfs_call_proc_mnt(nas, MOUNTPROC3_EXPORT,
		0, (char *)&export)){
		return -1;
	}
	if(0 != strcmp(fsi->srv_dir, export.dir_name)) {
		return -1;
	}
	/* send NULL procedure*/
	if (0 > nfs_call_proc_mnt(nas, MOUNTPROC3_NULL, 0, 0)) {
		return -1;
	}

	/* send MOUNT procedure */
	point = export.dir_name;
	p_fh = &fsi->fh;
	memset(&mnt_svc, 0, sizeof(mnt_svc));

	if (0 > nfs_call_proc_mnt(nas, MOUNTPROC3_MNT,
			(char *)&point, (char *)&mnt_svc)) {
		return -1;
	}
	mnt_svc.fh.count = DIRCOUNT;
	mnt_svc.fh.maxcount = DIRCOUNT;
	/* copy filehandle of new filesystem*/
	memcpy(p_fh, &mnt_svc.fh, sizeof(mnt_svc.fh));

	/* read info about filesystem */
	if ((0 >  nfs_call_proc_nfs(nas, NFSPROC3_NULL, 0, 0)) ||
	(0 >  nfs_call_proc_nfs(nas, NFSPROC3_FSSTAT, (char *)&fsi->fh, 0)) ||
	(0 >  nfs_call_proc_nfs(nas, NFSPROC3_FSINFO, (char *)&fsi->fh, 0))) {
		return -1;
	}
	return 0;
}
