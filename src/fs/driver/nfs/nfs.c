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

#include <fs/mount.h>
#include <fs/vfs.h>
#include <fs/nfs.h>
#include <fs/xdr_nfs.h>
#include <fs/path.h>
#include <fs/node.h>
#include <fs/file_desc.h>

#include <mem/misc/pool.h>
#include <net/rpc/clnt.h>
#include <net/rpc/xdr.h>


static int nfs_create_dir_entry(char *parent);

static int nfs_mount(void);
static int nfs_lookup(node_t *node, nfs_file_info_t *fi);
static int nfs_call_proc_nfs(nfs_filesystem_t *fs,
		__u32 procnum, char *req, char *reply);
struct nfs_filesystem *fs;

/* nfs filesystem description pool */
POOL_DEF (nfs_fs_pool, struct nfs_filesystem, OPTION_GET(NUMBER,nfs_descriptor_quantity));

/* nfs file description pool */
POOL_DEF (nfs_file_pool, struct nfs_file_info, OPTION_GET(NUMBER,inode_quantity));

/* File operations */

static void *nfsfs_fopen(struct file_desc *desc,  const char *mode);
static int nfsfs_fclose(struct file_desc *desc);
static size_t nfsfs_fread(void *buf, size_t size, size_t count, void *file);
static size_t nfsfs_fwrite(const void *buf, size_t size, size_t count,
		void *file);
static int nfsfs_fseek(void *file, long offset, int whence);
static int nfsfs_ioctl(void *file, int request, va_list args);

static file_operations_t nfsfs_fop = { nfsfs_fopen, nfsfs_fclose, nfsfs_fread,
		nfsfs_fwrite, nfsfs_fseek, nfsfs_ioctl, NULL };
/*
 * file_operation
 */
static void *nfsfs_fopen(struct file_desc *desc, const char *mode) {

	node_t *nod;
	nfs_file_info_t *fi;

	nod = desc->node;
	fi = (nfs_file_info_t *)nod->fi;

	if ('r' == *mode) {
		fi->mode = O_RDONLY;
	}
	else if ('w' == *mode) {
		fi->mode = O_WRONLY;
	}
	else {
		fi->mode = O_RDONLY;
	}
	fi->offset = 0;

	if(0 == nfs_lookup(nod, fi)) {
		return desc;
	}
	return NULL;
}

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

static int nfsfs_fclose(struct file_desc *desc) {
	return 0;
}

static size_t nfsfs_fread(void *buf, size_t size, size_t count, void *file) {
	size_t size_to_read, read_size;
	struct file_desc *desc;
	nfs_file_info_t *fi;
	read_req_t req;
	read_reply_t reply;
	size_t datalen;

	read_size = size * count;
	desc = (struct file_desc *) file;
	fi = (nfs_file_info_t *)desc->node->fi;
	datalen = 0;

	while(1) {
		if (read_size > DIRCOUNT) {
			size_to_read = DIRCOUNT;
		}
		else {
			size_to_read = read_size;
		}
		read_size -= size_to_read;
		/* set read structure */
		req.count = size_to_read;
		req.offset = fi->offset;
		req.fh = &fi->fh.name_fh;
		reply.datalen = 0;
		reply.data = (char *) buf + datalen;

		/* send read command */
		if (0 > nfs_call_proc_nfs(fs, NFSPROC3_READ,
				(char *) &req, (char *) &reply)) {
			return 0;
		}

		fi->offset += size_to_read;
		datalen += reply.datalen;

		if (reply.eof || (0 >= read_size )) {
			break;
		}
	}
	return datalen;
}

static size_t nfsfs_fwrite(const void *buf, size_t size,
	size_t count, void *file) {

	size_t size_to_write;
	struct file_desc *desc;
	nfs_file_info_t *fi;
	write_req_t req;
	write_reply_t reply;

	size_to_write = size * count;
	desc = (struct file_desc *) file;
	fi = (nfs_file_info_t *)desc->node->fi;

	/* set read structure */
	req.count = req.datalen = size_to_write;
	req.data = (char *) buf;
	req.offset = fi->offset;
	req.fh = &fi->fh.name_fh;
	req.stable = FILE_SYNC;

	reply.attr = &fi->attr;

	/* send read command */
	if (0 > nfs_call_proc_nfs(fs, NFSPROC3_WRITE,
			(char *) &req, (char *) &reply)) {
		return 0;
	}

	fi->offset += reply.count;

	return reply.count;
}

static int nfsfs_ioctl(void *file, int request, va_list args) {
	return 0;
}

/* File system operations */

static int nfsfs_init(void * par);
static int nfsfs_format(void * par);
static int nfsfs_mount(void * par);
static int nfsfs_create(void *par);
static int nfsfs_delete(const char *fname);

static fsop_desc_t nfsfs_fsop = { nfsfs_init, nfsfs_format, nfsfs_mount,
		nfsfs_create, nfsfs_delete };

static fs_drv_t nfsfs_drv = { "nfs", &nfsfs_fop, &nfsfs_fsop };

static int nfsfs_init(void * par) {
	//init_nfs_fsinfo_pool();
	//init_nfs_fileinfo_pool();
	return 0;
}

static int nfsfs_format(void *path) {
	node_t *nod;

	if (NULL == (nod = vfs_find_node((char *) path, NULL))) {
		return -ENODEV;/*device not found*/
	}
	/* TODO format command support */
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

static int nfs_clnt_destroy (nfs_filesystem_t *fs) {

	if (NULL != fs->mnt) {
		clnt_destroy(fs->mnt);
		fs->mnt = NULL;
	}
	if (NULL != fs->nfs) {
		clnt_destroy(fs->nfs);
		fs->nfs = NULL;
	}
	return 0;
}

static int nfs_prepare(char *dev) {
	char *src, *dst;

	/* copy name of server and mount filesystem server directory*/
	src = dev;
	dst = fs->srv_name;

	do {
		if ('\0' == *src) {
			return -1;
		}
		*dst++ = *src++;
	} while (':' != *src);

	src++;
	dst = fs->srv_dir;
	do {
		*dst++ = *src++;
	} while ('\0' != *src);
	return 0;
}

static int nfs_client_init(void) {

	nfs_clnt_destroy(fs);
	/* Create nfs programm clients */
	fs->mnt = clnt_create(fs->srv_name,
			MOUNT_PROGNUM, MOUNT_VER, "tcp");
	if (fs->mnt == NULL) {
		clnt_pcreateerror(fs->srv_name);
		return -1;
	}
	if(0 != nfs_unix_auth_set(fs->mnt)) {
		return -1;
	}

	fs->nfs = clnt_create(fs->srv_name, NFS_PROGNUM, NFS_VER, "tcp");
	if (fs->nfs == NULL) {
		clnt_pcreateerror(fs->srv_name);
		return -1;
	}

	return nfs_unix_auth_set(fs->nfs);
}

static int nfsfs_mount(void *par) {
	mount_params_t *params;
	node_t *dir_node;
	nfs_file_info_t *fi;

	params = (mount_params_t *) par;

	if (NULL == (dir_node = vfs_find_node(params->dir, NULL))) {
		/*TODO usually mount doesn't create a directory*/
		if (NULL == (dir_node = vfs_add_path (params->dir, NULL))) {
			return -ENODEV;/*device not found*/
		}
		dir_node->properties = DIRECTORY_NODE_TYPE;
	}

	if ((NULL == (fs = pool_alloc(&nfs_fs_pool))) ||
			(NULL == (fi = pool_alloc(&nfs_file_pool)))) {
		if(NULL != fs) {
			pool_free(&nfs_fs_pool, fs);
		}
		return -ENOMEM;
	}
	fi->fs = fs;

	dir_node->fs_type = &nfsfs_drv;
	dir_node->fi = (void *) fi;
	//dir_node->dev_id = params->dev_node->dev_id;
	params->dev_node = dir_node;

	strncpy(fs->mnt_point, params->dir, MAX_LENGTH_PATH_NAME);

	if(0 >  nfs_prepare(params->ext)) {
		return -1;
	}
	if(0 >  nfs_client_init()) {
		return -1;
	}

	if(0 >  nfs_mount()) {
		nfs_clnt_destroy(fs);
		//nfs_fsinfo_free(fs);
		pool_free(&nfs_fs_pool, fs);
		return -1;
	}

	/* copy filesystem filehandle to root directory filehandle */
	memcpy(&fi->fh, &fi->fs->fh, sizeof(fi->fh));

	if(0 >  nfs_create_dir_entry(fs->mnt_point)) {
		nfs_clnt_destroy(fs);
		//nfs_fsinfo_free(fs);
		pool_free(&nfs_fs_pool, fs);
		return -1;
	}
	return 0;
}

static node_t  *nfs_create_file (char *full_name, readdir_desc_t *predesc) {
	node_t  *node;
	nfs_file_info_t *fi;

	if (NULL == (node = vfs_find_node(full_name, NULL))) {
		/*TODO usually mount doesn't create a directory*/
		if (NULL == (node = vfs_add_path (full_name, NULL))) {
			return NULL;/*device not found*/
		}
		if(NULL == (fi = pool_alloc(&nfs_file_pool))) {
			return NULL;
		}
	}
	else {
		fi = (nfs_file_info_t *) node->fi;
	}

	/* copy read the description in the created file*/
	memcpy(&fi->name_dsc, &predesc->file_name,
			sizeof(predesc->file_name));

	if(VALUE_FOLLOWS_YES == predesc->vf_attr) {
		memcpy(&fi->attr, &predesc->file_attr,
				sizeof(predesc->file_attr));
	}
	if(VALUE_FOLLOWS_YES == predesc->vf_fh) {
		memcpy(&fi->fh, &predesc->file_handle,
				sizeof(predesc->file_handle));
		fi->fh.count = fi->fh.maxcount = DIRCOUNT;
		fi->fh.cookie = 0;
	}

	fi->fs = fs;
	node->fs_type = &nfsfs_drv;
	//node->file_info = (void *) &nfsfs_fop;
	//node->dev_id = NULL;
	node->fi = (void *)fi;
	return node;
}

static int nfs_create_dir_entry(char *parent) {
	node_t *parent_node, *node;
	__u32 vf;
	char *point;
	nfs_file_info_t *parent_fi, *fi;
	nfs_filehandle_t *fh;
	readdir_desc_t *predesc;

	char full_path[MAX_LENGTH_PATH_NAME];
	char *rcv_buf;

	if (NULL == (parent_node = vfs_find_node(parent, NULL))) {
		return -1;/*device not found*/
	}

	parent_fi = (nfs_file_info_t *) parent_node->fi;
	fh = &parent_fi->fh;
	fh->count = fh->maxcount = DIRCOUNT;
	fh->cookie = 0;

	if(NULL == (rcv_buf = malloc(MAXDIRCOUNT * 2))) {
		return -1;
	}

	while (1) {

		memset(rcv_buf, 0, sizeof(rcv_buf));

		if(0 >  nfs_call_proc_nfs(fs, NFSPROC3_READDIRPLUS,
			(char *)fh, rcv_buf)) {
			free(rcv_buf);
			return -1;
		}

		point = rcv_buf;
		/* check status */
		vf = *(__u32 *)point;
		if(STATUS_OK != vf) {
			free(rcv_buf);
			return -1;
		}
		point += sizeof(vf);

		/* check if a directory attributes */
		vf = *(__u32 *)point;
		if(VALUE_FOLLOWS_YES != vf) {
			break;
		}
		point += sizeof(vf);

		/*TODO copy root dir attr*/
		point += sizeof(dir_attribute_rep_t);

		/* check if a new files attributes */
		vf = *(__u32 *)point;
		if(VALUE_FOLLOWS_YES != vf) {
			break;
		}

		while (VALUE_FOLLOWS_YES == (vf = *(__u32 *)point)) {
			point += sizeof(vf);
			predesc = (readdir_desc_t *) point;

			memset(full_path, 0, sizeof(full_path));

			strncpy(full_path, parent, MAX_LENGTH_PATH_NAME);
			strcat(full_path, "/");
			strcat(full_path, (const char *) predesc->file_name.name.data);

			if (NULL == (node = nfs_create_file(full_path, predesc))) {
				free(rcv_buf);
				return -1;
			}

			fi = (nfs_file_info_t *) node->fi;
			if (NFS_DIRECTORY_NODE_TYPE == fi->attr.type) {
				node->properties = DIRECTORY_NODE_TYPE;
				if((0 != strcmp(fi->name_dsc.name.data, "."))
					&& (0 != strcmp(fi->name_dsc.name.data, ".."))) {
					nfs_create_dir_entry(full_path);
				}
			}
			else {
				node->properties = FILE_NODE_TYPE;
			}
			point += sizeof(*predesc);
		}
		point += sizeof(vf);
		if(NFS_EOF == *(__u32 *)point) {
			fh->cookie = 0;
			break;
		}
		else {
			fh->cookie = predesc->file_name.cookie;
		}
	}
	free(rcv_buf);
	return 0;
}

static int nfsfs_create(void *par) {

	file_create_param_t *param;
	nfs_file_info_t *parent_fi, *fi;
	node_t *node, *parent_node;
	create_req_t  req;
	rpc_string_t name;
	create_reply_t reply;
	__u32 procnum;
	char path[MAX_LENGTH_PATH_NAME];
	char tail[MAX_LENGTH_PATH_NAME];

	param = (file_create_param_t *) par;

	node = (node_t *)param->node;
	parent_node = (node_t *)param->parents_node;
	parent_fi = (nfs_file_info_t *) parent_node->fi;

	if (DIRECTORY_NODE_TYPE == (node->properties & DIRECTORY_NODE_TYPE)) {
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
	strncpy(name.data, node->name, MAX_LENGTH_FILE_NAME);
	name.len = strlen(node->name);
	req.new.fname = &name;
	/* set attribute of new file */
	req.mode_vf = req.uid_vf = req.gid_vf = req.size_vf =
			req.set_atime =	req.set_mtime =  VALUE_FOLLOWS_YES;
	req.uid = req.gid = 0;

	/* send nfs CREATE command   */
	if (0 > nfs_call_proc_nfs(fs, procnum, (char *) &req, (char *) &reply)) {
		return -1;
	}

	if(NULL == (fi = pool_alloc(&nfs_file_pool))) {
		return -1;
	}
	node->fi = (void *) fi;
	strncpy(path, param->path, MAX_LENGTH_PATH_NAME);
	path_nip_tail(path, tail);

	return nfs_create_dir_entry (path);
}

static int nfsfs_delete(const char *fname) {
	nfs_file_info_t *fi;
	node_t *node;
	node_t *dir_node;
	nfs_file_info_t *dir_fi;
	lookup_req_t req;
	delete_reply_t reply;
	__u32 procnum;

	node = vfs_find_node(fname, NULL);
	fi = (nfs_file_info_t *) node->fi;

	if(NULL ==
			(dir_node = vfs_find_parent((const char *) &node->name, node))) {
		return -1;
	}

	/* set delete structure */
	req.fname = &fi->name_dsc.name;
	dir_fi = (nfs_file_info_t *) dir_node->fi;
	req.dir_fh = &dir_fi->fh.name_fh;

	if (DIRECTORY_NODE_TYPE == (node->properties & DIRECTORY_NODE_TYPE)) {
		procnum = NFSPROC3_RMDIR;
	}
	else {
		procnum = NFSPROC3_REMOVE;
	}

	reply.dir_attr = &dir_fi->attr;

	/* send delete command */
	if (0 > nfs_call_proc_nfs(fs, procnum, (char *) &req, (char *) &reply)) {
		return -1;
	}

	pool_free(&nfs_file_pool, fi);
	vfs_del_leaf(node);
	return 0;
}

DECLARE_FILE_SYSTEM_DRIVER(nfsfs_drv);

static int nfs_call_proc_mnt(nfs_filesystem_t *fs,
		__u32 procnum, char *req, char *reply) {
	struct timeval timeout = { 25, 0 };

	if(NULL == fs->mnt){
		if(0 >  nfs_client_init()) {
			return -1;
		}
	}

	switch (procnum) {
	case MOUNTPROC3_NULL:
		if (clnt_call(fs->mnt, MOUNTPROC3_NULL,
			(xdrproc_t)xdr_void, 0,
			(xdrproc_t)xdr_void, 0,
			timeout) != RPC_SUCCESS) {
			/*
			 * An error occurred while calling the server.
			 * Print error message and die
			 */
			clnt_perror(fs->mnt, fs->srv_name);
			/* error code in client, now */
			printf("mnt null failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case MOUNTPROC3_MNT:
		if (clnt_call(fs->mnt, MOUNTPROC3_MNT,
			(xdrproc_t)xdr_wrapstring, req,
			(xdrproc_t)xdr_mnt_service, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fs->mnt, fs->srv_name);
			printf("mnt mount failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case MOUNTPROC3_EXPORT:
		if (clnt_call(fs->mnt, MOUNTPROC3_EXPORT,
			(xdrproc_t)xdr_void, 0,
			(xdrproc_t)xdr_mnt_export, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fs->mnt, fs->srv_name);
			printf("mnt export failed. errno=%d\n", errno);
			return -1;
		}
		break;

	default:
		return -1;
	}
	return 0;
}

static int nfs_call_proc_nfs(nfs_filesystem_t *fs,
		__u32 procnum, char *req, char *reply) {
	struct timeval timeout = { 25, 0 };

	if(NULL == fs->nfs){
		if(0 >  nfs_client_init()) {
			return -1;
		}
	}

	switch (procnum) {
	case NFSPROC3_NULL:
		if (clnt_call(fs->nfs, NFSPROC3_NULL,
			(xdrproc_t)xdr_void, 0,
			(xdrproc_t)xdr_void, 0,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fs->nfs, fs->srv_name);
			printf("nfs null failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_LOOKUP:
		if (clnt_call(fs->nfs, NFSPROC3_LOOKUP,
			(xdrproc_t)xdr_nfs_lookup, req,
			(xdrproc_t)xdr_nfs_lookup, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fs->nfs, fs->srv_name);
			printf("nfs lookup failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_READ:
		if (clnt_call(fs->nfs, NFSPROC3_READ,
			(xdrproc_t)xdr_nfs_read_file, req,
			(xdrproc_t)xdr_nfs_read_file, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fs->nfs, fs->srv_name);
			printf("read file failed. errno=%d\n", errno);
			return 0;
		}
		break;
	case NFSPROC3_WRITE:
		if (clnt_call(fs->nfs, NFSPROC3_WRITE,
			(xdrproc_t)xdr_nfs_write_file, req,
			(xdrproc_t)xdr_nfs_write_file, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fs->nfs, fs->srv_name);
			printf("write file failed. errno=%d\n", errno);
			return 0;
		}
		break;
	case NFSPROC3_CREATE:
	case NFSPROC3_MKDIR:
		if (clnt_call(fs->nfs, procnum,
			(xdrproc_t)xdr_nfs_create, req,
			(xdrproc_t)xdr_nfs_create, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fs->nfs, fs->srv_name);
			printf("nfs create failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_REMOVE:
	case NFSPROC3_RMDIR:
		if (clnt_call(fs->nfs, procnum,
			(xdrproc_t)xdr_nfs_delete, req,
			(xdrproc_t)xdr_nfs_delete, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fs->nfs, fs->srv_name);
			printf("nfs delete failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_READDIRPLUS:
		if (clnt_call(fs->nfs, NFSPROC3_READDIRPLUS,
			(xdrproc_t)xdr_nfs_readdirplus, req,
			(xdrproc_t)xdr_nfs_get_dirlist, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fs->nfs, fs->srv_name);
			printf("nfs readdirplus failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_FSSTAT:
		if (clnt_call(fs->nfs, NFSPROC3_FSSTAT,
			(xdrproc_t)xdr_nfs_name_fh, req,
			(xdrproc_t)xdr_void, 0,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fs->nfs, fs->srv_name);
			printf("nfs fsstat failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_FSINFO:
		if (clnt_call(fs->nfs, NFSPROC3_FSINFO,
			(xdrproc_t)xdr_nfs_name_fh, req,
			(xdrproc_t)xdr_void, 0,
			timeout) != RPC_SUCCESS) {
			clnt_perror(fs->nfs, fs->srv_name);
			printf("nfs fsinfo failed. errno=%d\n", errno);
			return -1;
		}
		break;
	default:
		return -1;
	}
	return 0;
}

static int nfs_lookup(node_t *node, nfs_file_info_t *fi) {
	node_t *dir_node;
	nfs_file_info_t *dir_fi;
	lookup_req_t req;
	lookup_reply_t reply;
	if(NULL ==
			(dir_node = vfs_find_parent((const char *) &node->name, node))) {
		return -1;
	}

	/* set lookup structure */
	req.fname = &fi->name_dsc.name;
	dir_fi = (nfs_file_info_t *) dir_node->fi;
	req.dir_fh = &dir_fi->fh.name_fh;

	reply.fh = &fi->fh.name_fh;

	/* send read command */
	return nfs_call_proc_nfs(fs, NFSPROC3_LOOKUP,
			(char *) &req, (char *) &reply);
}

static int nfs_mount(void) {
	char *point;
	nfs_filehandle_t *p_fh;
	mount_service_t mnt_svc;

	/* get server mount directory name*/
	memset((void *)&fs->export, 0, sizeof(fs->export));
	if (0 > nfs_call_proc_mnt(fs, MOUNTPROC3_EXPORT,
		0, (char *)&fs->export)){
		return -1;
	}
	if(0 != strcmp(fs->srv_dir, fs->export.dir_name)) {
		return -1;
	}
	/* send NULL procedure*/
	if (0 > nfs_call_proc_mnt(fs, MOUNTPROC3_NULL, 0, 0)) {
		return -1;
	}

	/* send MOUNT procedure */
	point = fs->export.dir_name;
	p_fh = &fs->fh;
	memset(&mnt_svc, 0, sizeof(mnt_svc));

	if (0 > nfs_call_proc_mnt(fs, MOUNTPROC3_MNT,
			(char *)&point, (char *)&mnt_svc)) {
		return -1;
	}
	mnt_svc.fh.count = DIRCOUNT;
	mnt_svc.fh.maxcount = DIRCOUNT;
	/* copy filehandle of new filesystem*/
	memcpy(p_fh, &mnt_svc.fh, sizeof(mnt_svc.fh));

	/* read info about filesystem */
	if ((0 >  nfs_call_proc_nfs(fs, NFSPROC3_NULL, 0, 0)) ||
	(0 >  nfs_call_proc_nfs(fs, NFSPROC3_FSSTAT, (char *)&fs->fh, 0)) ||
	(0 >  nfs_call_proc_nfs(fs, NFSPROC3_FSINFO, (char *)&fs->fh, 0))) {
		return -1;
	}
	return 0;
}
