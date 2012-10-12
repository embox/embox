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
#include <fs/mount.h>
#include <fs/vfs.h>
#include <fs/nfs.h>
#include <fs/xdr_nfs.h>
#include <mem/misc/pool.h>
#include <net/rpc/clnt.h>
#include <net/rpc/xdr.h>

static int create_dir_entry(char *parent);

static int nfs_mount_proc(void);
static int nfs_call_proc(nfs_fs_description_t *p_fs_fd,
		__u32 procnum, char *req, char *reply);
static int nfs_lookup(node_t *node, nfs_file_description_t *fd);

nfs_fs_description_t *p_fs_fd;

/* nfs filesystem description pool */
POOL_DEF (nfs_fs_pool, struct nfs_fs_description, OPTION_GET(NUMBER,nfs_descriptor_quantity));

/* nfs file description pool */
POOL_DEF (nfs_file_pool, struct nfs_file_description, OPTION_GET(NUMBER,inode_quantity));

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
	nfs_file_description_t *fd;

	nod = desc->node;
	fd = (nfs_file_description_t *)nod->fd;

	if ('r' == *mode) {
		fd->fi.mode = O_RDONLY;
	}
	else if ('w' == *mode) {
		fd->fi.mode = O_WRONLY;
	}
	else {
		fd->fi.mode = O_RDONLY;
	}
	fd->fi.offset = 0;

	if(0 == nfs_lookup(nod, fd)) {
		return desc;
	}
	return NULL;
}

static int nfsfs_fseek(void *file, long offset, int whence) {

	struct file_desc *desc;
	nfs_file_description_t *fd;

	desc = (struct file_desc *) file;
	fd = (nfs_file_description_t *) desc->node->fd;

	switch (whence) {
	case SEEK_SET:
		fd->fi.offset = offset;
		break;
	case SEEK_CUR:
		fd->fi.offset += offset;
		break;
	case SEEK_END:
		fd->fi.offset = fd->attr.size;
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
	nfs_file_description_t *fd;
	read_req_t req;
	read_reply_t reply;
	size_t datalen;

	read_size = size * count;
	desc = (struct file_desc *) file;
	fd = (nfs_file_description_t *)desc->node->fd;
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
		req.offset = fd->fi.offset;
		req.fh = &fd->fh.name_fh;
		reply.datalen = 0;
		reply.data = (char *) buf + datalen;

		/* send read command */
		if (0 > nfs_call_proc(p_fs_fd, NFSPROC3_READ,
				(char *) &req, (char *) &reply)) {
			return 0;
		}

		fd->fi.offset += size_to_read;
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
	nfs_file_description_t *fd;
	write_req_t req;
	write_reply_t reply;

	size_to_write = size * count;
	desc = (struct file_desc *) file;
	fd = (nfs_file_description_t *)desc->node->fd;

	/* set read structure */
	req.count = req.datalen = size_to_write;
	req.data = (char *) buf;
	req.offset = fd->fi.offset;
	req.fh = &fd->fh.name_fh;
	req.stable = FILE_SYNC;

	reply.attr = &fd->attr;

	/* send read command */
	if (0 > nfs_call_proc(p_fs_fd, NFSPROC3_WRITE,
			(char *) &req, (char *) &reply)) {
		return 0;
	}

	fd->fi.offset += reply.count;

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

static int nfs_clnt_destroy (nfs_fs_description_t *p_fsfd) {

	if (NULL != p_fsfd->mnt) {
		clnt_destroy(p_fsfd->mnt);
		p_fsfd->mnt = NULL;
	}
	if (NULL != p_fsfd->nfs) {
		clnt_destroy(p_fsfd->nfs);
		p_fsfd->nfs = NULL;
	}
	return 0;
}

static int nfs_prepare(char *dev) {
	char *src, *dst;

	/* copy name of server and mount filesystem server directory*/
	src = dev;
	dst = p_fs_fd->srv_name;

	do {
		if ('\0' == *src) {
			return -1;
		}
		*dst++ = *src++;
	} while (':' != *src);

	src++;
	dst = p_fs_fd->srv_dir;
	do {
		*dst++ = *src++;
	} while ('\0' != *src);
	return 0;
}

static int nfs_client_init(void) {

	nfs_clnt_destroy(p_fs_fd);
	/* Create nfs programm clients */
	p_fs_fd->mnt = clnt_create(p_fs_fd->srv_name,
			MOUNT_PROGNUM, MOUNT_VER, "tcp");
	if (p_fs_fd->mnt == NULL) {
		clnt_pcreateerror(p_fs_fd->srv_name);
		return -1;
	}
	if(0 != nfs_unix_auth_set(p_fs_fd->mnt)) {
		return -1;
	}

	p_fs_fd->nfs = clnt_create(p_fs_fd->srv_name, NFS_PROGNUM, NFS_VER, "tcp");
	if (p_fs_fd->nfs == NULL) {
		clnt_pcreateerror(p_fs_fd->srv_name);
		return -1;
	}

	return nfs_unix_auth_set(p_fs_fd->nfs);
}

static int nfsfs_mount(void *par) {
	mount_params_t *params;
	node_t *dir_node;
	nfs_file_description_t *fd;

	params = (mount_params_t *) par;

	if (NULL == (dir_node = vfs_find_node(params->dir, NULL))) {
		/*TODO usually mount doesn't create a directory*/
		if (NULL == (dir_node = vfs_add_path (params->dir, NULL))) {
			return -ENODEV;/*device not found*/
		}
		dir_node->properties = DIRECTORY_NODE_TYPE;
	}

	if ((NULL == (p_fs_fd = pool_alloc(&nfs_fs_pool))) ||
			(NULL == (fd = pool_alloc(&nfs_file_pool)))) {
		return -ENOMEM;
	}
	fd->p_fs_dsc = p_fs_fd;

	dir_node->fs_type = &nfsfs_drv;
	dir_node->fd = (void *) fd;
	dir_node->dev_attr = params->dev_node->dev_attr;
	dir_node->dev_type = params->dev_node->dev_type;
	params->dev_node = dir_node;

	strcpy(p_fs_fd->mnt_point, params->dir);

	if(0 >  nfs_prepare(params->dev)) {
		return -1;
	}
	if(0 >  nfs_client_init()) {
		return -1;
	}

	if(0 >  nfs_mount_proc()) {
		nfs_clnt_destroy(p_fs_fd);
		//nfs_fsinfo_free(p_fs_fd);
		pool_free(&nfs_fs_pool, p_fs_fd);
		return -1;
	}

	/* copy filesystem filehandle to root directory filehandle */
	memcpy(&fd->fh, &fd->p_fs_dsc->fh, sizeof(fd->fh));

	if(0 >  create_dir_entry(p_fs_fd->mnt_point)) {
		nfs_clnt_destroy(p_fs_fd);
		//nfs_fsinfo_free(p_fs_fd);
		pool_free(&nfs_fs_pool, p_fs_fd);
		return -1;
	}
	return 0;
}

static node_t  *create_nfs_file (char *full_name, readdir_desc_t *predesc) {
	node_t  *node;
	nfs_file_description_t *fd;

	if (NULL == (node = vfs_find_node(full_name, NULL))) {
		/*TODO usually mount doesn't create a directory*/
		if (NULL == (node = vfs_add_path (full_name, NULL))) {
			return NULL;/*device not found*/
		}
		if(NULL == (fd = pool_alloc(&nfs_file_pool))) {
			return NULL;
		}
	}
	else {
		fd = (nfs_file_description_t *) node->fd;
	}

	/* copy read the description in the created file*/
	memcpy(&fd->name_dsc, &predesc->file_name,
			sizeof(predesc->file_name));

	if(VALUE_FOLLOWS_YES == predesc->vf_attr) {
		memcpy(&fd->attr, &predesc->file_attr,
				sizeof(predesc->file_attr));
	}
	if(VALUE_FOLLOWS_YES == predesc->vf_fh) {
		memcpy(&fd->fh, &predesc->file_handle,
				sizeof(predesc->file_handle));
		fd->fh.count = fd->fh.maxcount = DIRCOUNT;
		fd->fh.cookie = 0;
	}

	fd->p_fs_dsc = p_fs_fd;
	node->fs_type = &nfsfs_drv;
	node->file_info = (void *) &nfsfs_fop;
	node->dev_type = node->dev_attr = NULL;
	node->fd = (void *)fd;
	return node;
}

static int create_dir_entry(char *parent) {
	node_t *parent_node, *node;
	__u32 vf;
	char *point;
	nfs_file_description_t *parent_fd, *fd;
	nfs_filehandle_t *fh;
	readdir_desc_t *predesc;

	char full_path[MAX_LENGTH_PATH_NAME];
	char *rcv_buf;

	if (NULL == (parent_node = vfs_find_node(parent, NULL))) {
		return -1;/*device not found*/
	}

	parent_fd = (nfs_file_description_t *) parent_node->fd;
	fh = &parent_fd->fh;
	fh->count = fh->maxcount = DIRCOUNT;
	fh->cookie = 0;

	if(NULL == (rcv_buf = malloc(MAXDIRCOUNT * 2))) {
		return -1;
	}

	while (1) {

		memset(rcv_buf, 0, sizeof(rcv_buf));

		if(0 >  nfs_call_proc(p_fs_fd, NFSPROC3_READDIRPLUS,
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

			strcpy(full_path, parent);
			strcat(full_path, "/");
			strcat(full_path, (const char *) predesc->file_name.name.data);

			if (NULL == (node = create_nfs_file (full_path, predesc))) {
				free(rcv_buf);
				return -1;
			}

			fd = (nfs_file_description_t *) node->fd;
			if (NFS_DIRECTORY_NODE_TYPE == fd->attr.type) {
				node->properties = DIRECTORY_NODE_TYPE;
				if((0 != strcmp(fd->name_dsc.name.data, "."))
					&& (0 != strcmp(fd->name_dsc.name.data, ".."))) {
					create_dir_entry(full_path);
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
	nfs_file_description_t *par_fd, *fd;
	node_t *node, *parents_node;
	create_req_t  req;
	rpc_string_t name;
	create_reply_t reply;
	__u32 procnum;
	char path[MAX_LENGTH_FILE_NAME];
	char tail[MAX_LENGTH_FILE_NAME];

	param = (file_create_param_t *) par;

	node = (node_t *)param->node;
	parents_node = (node_t *)param->parents_node;
	par_fd = (nfs_file_description_t *) parents_node->fd;

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
	req.new.dir_fh = &par_fd->fh.name_fh;
	/* set new file name */
	memset((void *) &name, 0, sizeof(name));
	strcpy(name.data, node->name);
	name.len = strlen(node->name);
	req.new.fname = &name;
	/* set attribute of new file */
	req.mode_vf = req.uid_vf = req.gid_vf = req.size_vf =
			req.set_atime =	req.set_mtime =  VALUE_FOLLOWS_YES;
	req.uid = req.gid = 0;

	/* send nfs CREATE command   */
	if (0 > nfs_call_proc(p_fs_fd, procnum, (char *) &req, (char *) &reply)) {
		return -1;
	}

	if(NULL == (fd = pool_alloc(&nfs_file_pool))) {
		return -1;
	}
	node->fd = (void *) fd;
	strcpy(path, param->path);
	nip_tail(path, tail);

	return create_dir_entry (path);
}

static int nfsfs_delete(const char *fname) {
	nfs_file_description_t *fd;
	node_t *node;
	node_t *dir_node;
	nfs_file_description_t *dir_fd;
	lookup_req_t req;
	delete_reply_t reply;
	__u32 procnum;

	node = vfs_find_node(fname, NULL);
	fd = (nfs_file_description_t *) node->fd;

	if(NULL ==
			(dir_node = vfs_find_parent((const char *) &node->name, node))) {
		return -1;
	}

	/* set delete structure */
	req.fname = &fd->name_dsc.name;
	dir_fd = (nfs_file_description_t *) dir_node->fd;
	req.dir_fh = &dir_fd->fh.name_fh;

	if (DIRECTORY_NODE_TYPE == (node->properties & DIRECTORY_NODE_TYPE)) {
		procnum = NFSPROC3_RMDIR;
	}
	else {
		procnum = NFSPROC3_REMOVE;
	}

	reply.dir_attr = &dir_fd->attr;

	/* send delete command */
	if (0 > nfs_call_proc(p_fs_fd, procnum, (char *) &req, (char *) &reply)) {
		return -1;
	}

	pool_free(&nfs_file_pool, fd);
	vfs_del_leaf(node);
	return 0;
}

DECLARE_FILE_SYSTEM_DRIVER(nfsfs_drv);

static int mnt_call_proc(nfs_fs_description_t *p_fs_fd,
		__u32 procnum, char *req, char *reply) {
	struct timeval timeout = { 25, 0 };

	if(NULL == p_fs_fd->mnt){
		if(0 >  nfs_client_init()) {
			return -1;
		}
	}

	switch (procnum) {
	case MOUNTPROC3_NULL:
		if (clnt_call(p_fs_fd->mnt, MOUNTPROC3_NULL,
			(xdrproc_t)xdr_void, 0,
			(xdrproc_t)xdr_void, 0,
			timeout) != RPC_SUCCESS) {
			/*
			 * An error occurred while calling the server.
			 * Print error message and die
			 */
			clnt_perror(p_fs_fd->mnt, p_fs_fd->srv_name);
			/* error code in client, now */
			printf("mnt null failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case MOUNTPROC3_MNT:
		if (clnt_call(p_fs_fd->mnt, MOUNTPROC3_MNT,
			(xdrproc_t)xdr_wrapstring, req,
			(xdrproc_t)xdr_mnt_service, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(p_fs_fd->mnt, p_fs_fd->srv_name);
			printf("mnt mount failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case MOUNTPROC3_EXPORT:
		if (clnt_call(p_fs_fd->mnt, MOUNTPROC3_EXPORT,
			(xdrproc_t)xdr_void, 0,
			(xdrproc_t)xdr_mnt_export, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(p_fs_fd->mnt, p_fs_fd->srv_name);
			printf("mnt export failed. errno=%d\n", errno);
			return -1;
		}
		break;

	default:
		return -1;
	}
	return 0;
}

static int nfs_call_proc(nfs_fs_description_t *p_fs_fd,
		__u32 procnum, char *req, char *reply) {
	struct timeval timeout = { 25, 0 };

	if(NULL == p_fs_fd->nfs){
		if(0 >  nfs_client_init()) {
			return -1;
		}
	}

	switch (procnum) {
	case NFSPROC3_NULL:
		if (clnt_call(p_fs_fd->nfs, NFSPROC3_NULL,
			(xdrproc_t)xdr_void, 0,
			(xdrproc_t)xdr_void, 0,
			timeout) != RPC_SUCCESS) {
			clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
			printf("nfs null failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_LOOKUP:
		if (clnt_call(p_fs_fd->nfs, NFSPROC3_LOOKUP,
			(xdrproc_t)xdr_nfs_lookup, req,
			(xdrproc_t)xdr_nfs_lookup, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
			printf("nfs lookup failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_READ:
		if (clnt_call(p_fs_fd->nfs, NFSPROC3_READ,
			(xdrproc_t)xdr_nfs_read_file, req,
			(xdrproc_t)xdr_nfs_read_file, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
			printf("read file failed. errno=%d\n", errno);
			return 0;
		}
		break;
	case NFSPROC3_WRITE:
		if (clnt_call(p_fs_fd->nfs, NFSPROC3_WRITE,
			(xdrproc_t)xdr_nfs_write_file, req,
			(xdrproc_t)xdr_nfs_write_file, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
			printf("write file failed. errno=%d\n", errno);
			return 0;
		}
		break;
	case NFSPROC3_CREATE:
	case NFSPROC3_MKDIR:
		if (clnt_call(p_fs_fd->nfs, procnum,
			(xdrproc_t)xdr_nfs_create, req,
			(xdrproc_t)xdr_nfs_create, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
			printf("nfs create failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_REMOVE:
	case NFSPROC3_RMDIR:
		if (clnt_call(p_fs_fd->nfs, procnum,
			(xdrproc_t)xdr_nfs_delete, req,
			(xdrproc_t)xdr_nfs_delete, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
			printf("nfs delete failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_READDIRPLUS:
		if (clnt_call(p_fs_fd->nfs, NFSPROC3_READDIRPLUS,
			(xdrproc_t)xdr_nfs_readdirplus, req,
			(xdrproc_t)xdr_nfs_get_dirlist, reply,
			timeout) != RPC_SUCCESS) {
			clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
			printf("nfs readdirplus failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_FSSTAT:
		if (clnt_call(p_fs_fd->nfs, NFSPROC3_FSSTAT,
			(xdrproc_t)xdr_nfs_name_fh, req,
			(xdrproc_t)xdr_void, 0,
			timeout) != RPC_SUCCESS) {
			clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
			printf("nfs fsstat failed. errno=%d\n", errno);
			return -1;
		}
		break;
	case NFSPROC3_FSINFO:
		if (clnt_call(p_fs_fd->nfs, NFSPROC3_FSINFO,
			(xdrproc_t)xdr_nfs_name_fh, req,
			(xdrproc_t)xdr_void, 0,
			timeout) != RPC_SUCCESS) {
			clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
			printf("nfs fsinfo failed. errno=%d\n", errno);
			return -1;
		}
		break;
	default:
		return -1;
	}
	return 0;
}

static int nfs_lookup(node_t *node, nfs_file_description_t *fd) {
	node_t *dir_node;
	nfs_file_description_t *dir_fd;
	lookup_req_t req;
	lookup_reply_t reply;
	if(NULL ==
			(dir_node = vfs_find_parent((const char *) &node->name, node))) {
		return -1;
	}

	/* set lookup structure */
	req.fname = &fd->name_dsc.name;
	dir_fd = (nfs_file_description_t *) dir_node->fd;
	req.dir_fh = &dir_fd->fh.name_fh;

	reply.fh = &fd->fh.name_fh;

	/* send read command */
	return nfs_call_proc(p_fs_fd, NFSPROC3_LOOKUP,
			(char *) &req, (char *) &reply);
}

static int nfs_mount_proc(void) {
	char *point;
	nfs_filehandle_t *p_fh;
	mount_service_t mnt_svc;

	/* get server mount directory name*/
	memset((void *)&p_fs_fd->export, 0, sizeof(p_fs_fd->export));
	if (0 > mnt_call_proc(p_fs_fd, MOUNTPROC3_EXPORT,
		0, (char *)&p_fs_fd->export)){
		return -1;
	}
	if(0 != strcmp(p_fs_fd->srv_dir, p_fs_fd->export.dir_name)) {
		return -1;
	}
	/* send NULL procedure*/
	if (0 > mnt_call_proc(p_fs_fd, MOUNTPROC3_NULL, 0, 0)) {
		return -1;
	}

	/* send MOUNT procedure */
	point = p_fs_fd->export.dir_name;
	p_fh = &p_fs_fd->fh;
	memset(&mnt_svc, 0, sizeof(mnt_svc));

	if (0 > mnt_call_proc(p_fs_fd, MOUNTPROC3_MNT,
			(char *)&point, (char *)&mnt_svc)) {
		return -1;
	}
	mnt_svc.fh.count = DIRCOUNT;
	mnt_svc.fh.maxcount = DIRCOUNT;
	/* copy filehandle of new filesystem*/
	memcpy(p_fh, &mnt_svc.fh, sizeof(mnt_svc.fh));

	/* read info about filesystem */
	if ((0 >  nfs_call_proc(p_fs_fd, NFSPROC3_NULL, 0, 0)) ||
	(0 >  nfs_call_proc(p_fs_fd, NFSPROC3_FSSTAT, (char *)&p_fs_fd->fh, 0)) ||
	(0 >  nfs_call_proc(p_fs_fd, NFSPROC3_FSINFO, (char *)&p_fs_fd->fh, 0))) {
		return -1;
	}
	return 0;
}
