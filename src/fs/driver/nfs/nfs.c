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
#include <cmd/mount.h>
#include <fs/vfs.h>
#include <fs/nfs.h>
#include <fs/xdr_nfs.h>
#include <net/rpc/clnt.h>
#include <net/rpc/xdr.h>


static int nfs_mount_proc(void);
static int create_filechain (void);

static int nfs_nfs_readdirplus(char *point, char *buff);
static int nfs_lookup(node_t *node, nfs_file_description_t *fd);

char snd_buf[MAXDIRCOUNT];
char rcv_buf[MAXDIRCOUNT];

nfs_fs_description_t *p_fs_fd;

/* nfs filesystem description pool */

typedef struct nfs_fs_description_head {
	struct list_head *next;
	struct list_head *prev;
	nfs_fs_description_t desc;
} nfs_fs_description_head_t;

static nfs_fs_description_head_t nfs_fs_pool[QUANTITY_RAMDISK];
static LIST_HEAD(nfs_free_fs);

#define param_to_head_fs(fs_param) \
	(uint32_t)(fs_param - offsetof(nfs_fs_description_head_t, desc))

static void init_nfs_fsinfo_pool(void) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(nfs_fs_pool); i++) {
		list_add((struct list_head *) &nfs_fs_pool[i], &nfs_free_fs);
	}
}

static nfs_fs_description_t *nfs_fsinfo_alloc(void) {
	nfs_fs_description_head_t *head;
	nfs_fs_description_t *desc;

	if (list_empty(&nfs_free_fs)) {
		return NULL;
	}
	head = (nfs_fs_description_head_t *) (&nfs_free_fs)->next;
	list_del((&nfs_free_fs)->next);
	desc = &(head->desc);
	return desc;
}

/*
static void nfs_fsinfo_free(nfs_fs_description_t *desc) {
	if (NULL == desc) {
		return;
	}
	list_add((struct list_head*) param_to_head_fs(desc), &nfs_free_fs);
}
*/

/* nfs file description pool */

typedef struct nfs_file_description_head {
	struct list_head *next;
	struct list_head *prev;
	nfs_file_description_t desc;
} nfs_file_description_head_t;

static nfs_file_description_head_t nfs_files_pool[MAX_FILE_QUANTITY];
static LIST_HEAD(nfs_free_file);

#define param_to_head_file(file_param) \
	(uint32_t)(file_param - offsetof(nfs_file_description_head_t, desc))

static void init_nfs_fileinfo_pool(void) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(nfs_files_pool); i++) {
		list_add((struct list_head *) &nfs_files_pool[i], &nfs_free_file);
	}
}

static nfs_file_description_t *nfs_fileinfo_alloc(void) {
	nfs_file_description_head_t *head;
	nfs_file_description_t *desc;

	if (list_empty(&nfs_free_file)) {
		return NULL;
	}
	head = (nfs_file_description_head_t *) (&nfs_free_file)->next;
	list_del((&nfs_free_file)->next);
	desc = &(head->desc);
	return desc;
}

static void nfs_fileinfo_free(nfs_file_description_t *desc) {
	if (NULL == desc) {
		return;
	}
	list_add((struct list_head*) param_to_head_file(desc), &nfs_free_file);
}

/* File operations */

static void *nfsfs_fopen(struct file_desc *desc,  const char *mode);
static int nfsfs_fclose(struct file_desc *desc);
static size_t nfsfs_fread(void *buf, size_t size, size_t count, void *file);
static size_t nfsfs_fwrite(const void *buf, size_t size, size_t count,
		void *file);
static int nfsfs_fseek(void *file, long offset, int whence);
static int nfsfs_ioctl(void *file, int request, va_list args);

static file_operations_t nfsfs_fop = { nfsfs_fopen, nfsfs_fclose, nfsfs_fread,
		nfsfs_fwrite, nfsfs_fseek, nfsfs_ioctl };
/*
 * file_operation
 */
static void *nfsfs_fopen(struct file_desc *desc, const char *mode) {

	node_t *nod;
	nfs_file_description_t *fd;

	nod = desc->node;
	fd = (nfs_file_description_t *)nod->attr;

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
	fd = (nfs_file_description_t *)desc->node->attr;

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
	struct timeval timeout = { 25, 0 };

	size_t size_to_read;
	struct file_desc *desc;
	nfs_file_description_t *fd;
	read_req_t req;
	read_reply_t reply;

	size_to_read = size * count;
	desc = (struct file_desc *) file;
	fd = (nfs_file_description_t *)desc->node->attr;

	/* set read structure */
	req.count = size_to_read;
	req.offset = fd->fi.offset;
	req.fh = &fd->fh.name_fh;
	reply.datalen = 0;
	reply.data = (char *) buf;

	/* send read command */
	if (clnt_call(p_fs_fd->nfs, NFSPROC3_READ,
		(xdrproc_t)xdr_nfs_read_file, (char *) &req,
		(xdrproc_t)xdr_nfs_read_file, (char *) &reply,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		printf("read file failed. errno=%d\n", errno);
		return 0;
	}
	fd->fi.offset += size_to_read;

	return reply.datalen;
}

static size_t nfsfs_fwrite(const void *buf, size_t size,
	size_t count, void *file) {
	struct timeval timeout = { 25, 0 };

	size_t size_to_write;
	struct file_desc *desc;
	nfs_file_description_t *fd;
	write_req_t req;
	write_reply_t reply;

	size_to_write = size * count;
	desc = (struct file_desc *) file;
	fd = (nfs_file_description_t *)desc->node->attr;

	/* set read structure */
	req.count = req.datalen = size_to_write;
	req.data = (char *) buf;
	req.offset = fd->fi.offset;
	req.fh = &fd->fh.name_fh;
	req.stable = FILE_SYNC;

	reply.attr = &fd->attr;

	/* send read command */
	if (clnt_call(p_fs_fd->nfs, NFSPROC3_WRITE,
		(xdrproc_t)xdr_nfs_write_file, (char *) &req,
		(xdrproc_t)xdr_nfs_write_file, (char *) &reply,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		printf("write file failed. errno=%d\n", errno);
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
	init_nfs_fsinfo_pool();
	init_nfs_fileinfo_pool();
	return 0;
}

static int nfsfs_format(void *par) {
	ramdisk_params_t *params;
	node_t *nod;

	params = (ramdisk_params_t *) par;
	if (NULL == (nod = vfs_find_node(params->path, NULL))) {
		return -ENODEV;/*device not found*/
	}

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

static int nfs_client_init(char *dev) {
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

	if ((NULL == (p_fs_fd = nfs_fsinfo_alloc())) ||
			(NULL == (fd = nfs_fileinfo_alloc()))) {
		return -ENOMEM;
	}
	fd->p_fs_dsc = p_fs_fd;

	dir_node->fs_type = &nfsfs_drv;
	dir_node->file_info = (void *) &nfsfs_fop;
	dir_node->attr = (void *) fd;
	params->dev_node = dir_node;

	strcpy(p_fs_fd->mnt_point, params->dir);

	if(0 >  nfs_client_init(params->dev)) {
		return -1;
	}
	if((0 >  nfs_mount_proc()) || (0 >  create_filechain())) {
		nfs_clnt_destroy(p_fs_fd);
		return -1;
	}

	/* copy filesystem filehandle to root directory filehandle */
	memcpy(&fd->fh, &fd->p_fs_dsc->fh, sizeof(fd->fh));
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
		if(NULL == (fd = nfs_fileinfo_alloc())) {
			return NULL;
		}
	}
	else {
		fd = (nfs_file_description_t *) node->attr;
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
	node->attr = (void *)fd;
	return node;
}

static int create_dir_entry(char *parent) {
	node_t *parent_node, *node;
	__u32 vf;
	char *point;
	nfs_file_description_t *parent_fd, *fd;
	nfs_filehandle_t *fh;
	readdir_desc_t *predesc;

	char full_path[CONFIG_MAX_LENGTH_PATH_NAME];
	char *rcv_buf;

	if (NULL == (parent_node = vfs_find_node(parent, NULL))) {
		return -1;/*device not found*/
	}

	parent_fd = (nfs_file_description_t *) parent_node->attr;
	fh = &parent_fd->fh;
	fh->count = fh->maxcount = DIRCOUNT;
	fh->cookie = 0;

	if(NULL == (rcv_buf = malloc(MAXDIRCOUNT * 2))) {
		return -1;
	}

	while (1) {

		if(0 >  nfs_nfs_readdirplus((char *)fh, rcv_buf)) {
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

			fd = (nfs_file_description_t *) node->attr;
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

static int create_filechain (void) {
	char *parent;
	node_t *node;
	nfs_file_description_t *parent_fd;

	parent = p_fs_fd->mnt_point;
	if (NULL == (node = vfs_find_node(parent, NULL))) {
		return -1;/*device not found*/
	}

	parent_fd = (nfs_file_description_t *) node->attr;

	/* copy filesystem filehandle to root filesystem mountpoint directory */
	memcpy(&parent_fd->fh, &p_fs_fd->fh, sizeof(p_fs_fd->fh));

	if(0 >  create_dir_entry(parent)) {
		return -1;
	}
	return 0;
}

static int nfsfs_create(void *par) {
	struct timeval timeout = { 25, 0 };

	file_create_param_t *param;
	nfs_file_description_t *par_fd, *fd;
	node_t *node, *parents_node;
	create_req_t  req;
	rpc_string_t name;
	create_reply_t reply;
	__u32 procnum;
	char path[CONFIG_MAX_LENGTH_FILE_NAME];
	char tail[CONFIG_MAX_LENGTH_FILE_NAME];

	param = (file_create_param_t *) par;

	node = (node_t *)param->node;
	parents_node = (node_t *)param->parents_node;
	par_fd = (nfs_file_description_t *) parents_node->attr;

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
			req.set_atime =	req.set_mtime =  0x00000001;
	req.uid = req.gid = 0;

	/* send nfs CREATE command   */
	if (clnt_call(p_fs_fd->nfs, procnum,
		(xdrproc_t)xdr_nfs_create, (char *) &req,
		(xdrproc_t)xdr_nfs_create, (char *) &reply,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		printf("nfs create failed. errno=%d\n", errno);
		return -1;
	}

	if(NULL == (fd = nfs_fileinfo_alloc())) {
		return -1;
	}
	node->attr = (void *) fd;
	strcpy(path, param->path);
	nip_tail(path, tail);

	return create_dir_entry (path);
}

static int nfsfs_delete(const char *fname) {
	struct timeval timeout = { 25, 0 };

	nfs_file_description_t *fd;
	node_t *node;
	node_t *dir_node;
	nfs_file_description_t *dir_fd;
	lookup_req_t req;
	delete_reply_t reply;
	__u32 procnum;

	node = vfs_find_node(fname, NULL);
	fd = (nfs_file_description_t *) node->attr;

	if(NULL ==
			(dir_node = vfs_find_parent((const char *) &node->name, node))) {
		return -1;
	}

	/* set delete structure */
	req.fname = &fd->name_dsc.name;
	dir_fd = (nfs_file_description_t *) dir_node->attr;
	req.dir_fh = &dir_fd->fh.name_fh;

	if (DIRECTORY_NODE_TYPE == (node->properties & DIRECTORY_NODE_TYPE)) {
		procnum = NFSPROC3_RMDIR;
	}
	else {
		procnum = NFSPROC3_REMOVE;
	}

	reply.dir_attr = &dir_fd->attr;

	/* send delete command */
	if (clnt_call(p_fs_fd->nfs, procnum,
		(xdrproc_t)xdr_nfs_delete, (char *) &req,
		(xdrproc_t)xdr_nfs_delete, (char *) &reply,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->mnt, p_fs_fd->srv_name);
		printf("nfs delete failed. errno=%d\n", errno);
		return -1;
	}

	nfs_fileinfo_free(fd);
	vfs_del_leaf(node);
	return 0;
}

DECLARE_FILE_SYSTEM_DRIVER(nfsfs_drv);

static int nfs_mnt_export(void) {
	struct timeval timeout = { 25, 0 };

	memset((void *)&p_fs_fd->export, 0, sizeof(p_fs_fd->export));
	if (clnt_call(p_fs_fd->mnt, MOUNTPROC3_EXPORT,
		(xdrproc_t)xdr_void, 0,
		(xdrproc_t)xdr_mnt_export, (char *)&p_fs_fd->export,
		timeout) != RPC_SUCCESS) {
		/*
		 * An error occurred while calling the server.
		 * Print error message and die
		 */
		clnt_perror(p_fs_fd->mnt, p_fs_fd->srv_name);
		/* error code in client, now */
		printf("mnt export failed. errno=%d\n", errno);
		return -1;
	}
	if(0 == strcmp(p_fs_fd->srv_dir, p_fs_fd->export.dir.data)) {
		return 0;
	}
	return -1;
}


static int nfs_mnt_null(void) {
	struct timeval timeout = { 25, 0 };

	if (clnt_call(p_fs_fd->mnt, MOUNTPROC3_NULL,
		(xdrproc_t)xdr_void, 0,
		(xdrproc_t)xdr_void, 0,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->mnt, p_fs_fd->srv_name);
		printf("mnt null failed. errno=%d\n", errno);
		return -1;
	}
	return 0;
}

static int nfs_mnt_mount(void) {
	struct timeval timeout = { 25, 0 };
	char *point;
	nfs_filehandle_t *p_fh;

	struct mount_service mnt_svc;
	memset(&mnt_svc, 0, sizeof(mnt_svc));

	point = p_fs_fd->export.dir.data;
	p_fh = &p_fs_fd->fh;

	if (clnt_call(p_fs_fd->mnt, MOUNTPROC3_MNT,
		(xdrproc_t)xdr_wrapstring, (char *)&point,
		(xdrproc_t)xdr_mnt_service, (char *)&mnt_svc,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->mnt, p_fs_fd->srv_name);
		printf("mnt mount failed. errno=%d\n", errno);
		return -1;
	}

	mnt_svc.fh.count = DIRCOUNT;
	mnt_svc.fh.maxcount = DIRCOUNT;
	memcpy(p_fh, &mnt_svc.fh, sizeof(mnt_svc.fh));


	return 0;
}

static int nfs_nfs_null(char *point) {
	struct timeval timeout = { 25, 0 };

	if (clnt_call(p_fs_fd->nfs, NFSPROC3_NULL,
		(xdrproc_t)xdr_void, 0,
		(xdrproc_t)xdr_void, 0,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		printf("nfs null failed. errno=%d\n", errno);
		return -1;
	}

	return 0;
}

static int nfs_nfs_fsstat(char *point) {
	struct timeval timeout = { 25, 0 };

	if (clnt_call(p_fs_fd->nfs, NFSPROC3_FSSTAT,
		(xdrproc_t)xdr_nfs_name_fh, point,
		(xdrproc_t)xdr_void, 0,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		printf("nfs null failed. errno=%d\n", errno);
		return -1;
	}

	return 0;
}

static int nfs_nfs_fsinfo(char *point) {
	struct timeval timeout = { 25, 0 };

	if (clnt_call(p_fs_fd->nfs, NFSPROC3_FSINFO,
		(xdrproc_t)xdr_nfs_name_fh, point,
		(xdrproc_t)xdr_void, 0,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		printf("nfs null failed. errno=%d\n", errno);
		return -1;
	}

	return 0;
}

static int nfs_lookup(node_t *node, nfs_file_description_t *fd){
	struct timeval timeout = { 25, 0 };

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
	dir_fd = (nfs_file_description_t *) dir_node->attr;
	req.dir_fh = &dir_fd->fh.name_fh;

	reply.fh = &fd->fh.name_fh;

	/* send read command */
	if (clnt_call(p_fs_fd->nfs, NFSPROC3_LOOKUP,
		(xdrproc_t)xdr_nfs_lookup, (char *) &req,
		(xdrproc_t)xdr_nfs_lookup, (char *) &reply,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->mnt, p_fs_fd->srv_name);
		printf("nfs lookup failed. errno=%d\n", errno);
		return -1;
	}
	return 0;
}

static int nfs_nfs_readdirplus(char *point, char *buff) {
	struct timeval timeout = { 25, 0 };

	memset(buff, 0, sizeof(buff));

	if (clnt_call(p_fs_fd->nfs, NFSPROC3_READDIRPLUS,
		(xdrproc_t)xdr_nfs_readdirplus, point,
		(xdrproc_t)xdr_nfs_get_dirlist, buff,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		printf("nfs readdirplus failed. errno=%d\n", errno);
		return -1;
	}

	return 0;
}

static int nfs_mount_proc(void) {

	if (0 > nfs_mnt_export()) {
		return -1;
	}
	if (0 > nfs_mnt_null()) {
		return -1;
	}
	if (0 > nfs_mnt_mount()) {
		return -1;
	}
	if(0 >  nfs_nfs_null((char *)&p_fs_fd->fh)) {
		return -1;
	}
	if(0 >  nfs_nfs_fsstat((char *)&p_fs_fd->fh)) {
		return -1;
	}
	if(0 >  nfs_nfs_fsinfo((char *)&p_fs_fd->fh)) {
		return -1;
	}

	return 0;
}
